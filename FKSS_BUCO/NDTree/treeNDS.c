#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "treeNDS.h"
#include "tabSol.h"
#include "dominance.h"

/*POUR LA MAXIMISATION*/

double nbCallUpdate = 0;

double max(double a,double b){
  if (a>b) return a;
  return b;
}

double euclideanDistance(int* obj1,int* obj2,int nbCrit){
  int i;
  double dist=0;
  for (i=0;i<nbCrit;i++){
    dist=dist+pow(obj1[i]-obj2[i],2);
  }
  return dist;
}

//Initilization of the structure
//ICI POUR MAXIMIZATION
TreeNDS* initializationTreeNDS(int nbCrit,int nbVar,int maxChildren,int maxLocalTabSize){
  TreeNDS* t = (TreeNDS*) malloc(sizeof(TreeNDS));
  t->nbCrit=nbCrit;
  t->nbVar=nbVar;
  t->nbChildren=0;
  t->maxChildren=maxChildren;
  t->localNadir=(int *)malloc(sizeof(int)*nbCrit);
  t->localIdeal=(int *)malloc(sizeof(int)*nbCrit);
  for (int i=0;i<nbCrit;i++){
		t->localNadir[i]=-1; //worst value of all objectives
		t->localIdeal[i]=INT_MAX; //best value of all objectives
	}
  t->maxLocalTabSize=maxLocalTabSize;
  t->localTabNDS=initializationTabNDS(nbCrit,nbVar,maxLocalTabSize+1);
  t->treeChildren=(TreeNDS**)malloc(sizeof(TreeNDS*)*maxChildren);  
  t->parent=NULL;
  return t;
}	

void writeTreeNDS(TreeNDS* t,int all,char* name){
  FILE *fListNDS = fopen(name,"w");
  writeScanTreeNDS(t,all,fListNDS);
  fclose(fListNDS);
}
  
void writeScanTreeNDS(TreeNDS* t,int all,FILE *fListNDS){
  int i;
  if (t->nbChildren==0){
    writeTabNDSWithoutLength(t->localTabNDS,all,fListNDS);
  }else{
    for (i=0;i<t->maxChildren;i++){
      if (t->treeChildren[i]!=NULL){
        writeScanTreeNDS(t->treeChildren[i],all,fListNDS);
      }
    }
  }
}

void displayTree(TreeNDS* t){
  int i;
  printf("Nadir =");
  for (i=0;i<t->nbCrit;i++){ 
    printf("%d ",t->localNadir[i]);
  } 	
  printf("\nIdeal =");
  for (i=0;i<t->nbCrit;i++){ 
    printf("%d ",t->localIdeal[i]);
  }  
  if (t->nbChildren==0){//leaf
    printf("Leaf\n");
    displayTabNDS(t->localTabNDS,0);
  }else{
    printf("Node\n");
    for (i=0;i<t->maxChildren;i++){
      if (t->treeChildren[i]!=NULL){
        printf("child %d\n",i);
        displayTree(t->treeChildren[i]);
      }else{
        printf("child null\n");
      }
    }
  }
}

int totalSizeTree(TreeNDS* t){
  int i;
  int size=0;
  if (t->nbChildren==0){
    return t->localTabNDS->currentSize;
  }else{
    for (i=0;i<t->maxChildren;i++){
      if (t->treeChildren[i]!=NULL){
        size=size+totalSizeTree(t->treeChildren[i]);
      }
    }
  }
  return size;
}

int totalNbLeaves(TreeNDS* t){
  int i;
  int nbLeaves = 0;
  if (t->nbChildren==0){
    return 1;
  }else{
    for (i=0;i<t->maxChildren;i++){
      if (t->treeChildren[i]!=NULL){
        nbLeaves=nbLeaves+totalNbLeaves(t->treeChildren[i]);
      }
    }
  }
  return nbLeaves;
}

int totalNbInternalNodes(TreeNDS* t){
  int i;
  int nbInternalNodes = 0;
  if (t->nbChildren==0){
    return -1;
  }else{
    for (i=0;i<t->maxChildren;i++){
      if (t->treeChildren[i]!=NULL){
        nbInternalNodes=nbInternalNodes+1+totalNbInternalNodes(t->treeChildren[i]);
      }
    }
  }
  return nbInternalNodes;
}


void initializationNadirIdealTreeNDS(TreeNDS* t,int* nadir,int* ideal){
  int i;
  for (i=0;i<t->nbCrit;i++){
    t->localNadir[i]=nadir[i];
    t->localIdeal[i]=ideal[i];
  }
}

int addSolutionTreeFirst(TreeNDS* t,int *x,int *obj){
  int i;
  for (i=0;i<t->nbCrit;i++){
    t->localNadir[i]=obj[i];
    t->localIdeal[i]=obj[i];
  }
  addSolutionTab(t->localTabNDS,x,obj);
  return 1;
}

int addSolutionTree(TreeNDS* t,int *x,int *obj){
  int added=1;
  upDateNode(&t,x,obj,&added);
  if (added==1){
    insertTree(t,x,obj);
    return 1;
  }
  return 0;
}

void upDateNode(TreeNDS** t,int *x,int *obj,int* added){
  nbCallUpdate++;
  int dn,di,i;
  
  dn=dominatePareto((*t)->localNadir,obj,(*t)->nbCrit);   //pas tjrs bien initialisé, à vérifier (afficher)

  if ((dn==1) || (dn==4)){
    *added=0;
    return;
  }
  
  di=dominatePareto(obj,(*t)->localIdeal,(*t)->nbCrit);
  if ((di==1) || (di==4)){
    if ((*t)->parent!=NULL){
      (*t)->parent->nbChildren=(*t)->parent->nbChildren-1;
    }
    (*t)=deleteTreeNDS((*t)); 
    *added=1;
    return;
  }
  if ((di==2) || (dn==2)){  
    if ((*t)->nbChildren==0){ //leaf
      *added=checkSolutionTab((*t)->localTabNDS,obj);
      if  (((*t)->localTabNDS->currentSize==0) && ((*t)->parent!=NULL)){
        (*t)->parent->nbChildren=(*t)->parent->nbChildren-1;
        (*t)=deleteTreeNDS(*t);
       }
    }else{
      for (i=0;i<(*t)->maxChildren;i++){
        if ((*t)->treeChildren[i]!=NULL){ 
          upDateNode(&((*t)->treeChildren[i]),x,obj,added);
          if (*added==0) return; //I stop to explore for the other children
        }
      }
    }
  }
}

void insertTree(TreeNDS* t,int *x,int *obj){
   int nodeMin=0,i,j,iStart;
   double distMiddle,distMinMiddle;
   if (t->nbChildren==0){
     addEnd(t->localTabNDS,x,obj);
     updateIdealNadir(t,obj);
     if (t->localTabNDS->currentSize > t->maxLocalTabSize){
       splitTree(t);
     }
   }else{
     i=0;
     while (t->treeChildren[i]==NULL){
       i++;
     }
     nodeMin=i;
     iStart=i;
     distMinMiddle=0;
     for (j=0;j<t->nbCrit;j++){
       distMinMiddle=distMinMiddle+pow((((t->treeChildren[nodeMin]->localNadir[j]+t->treeChildren[nodeMin]->localIdeal[j])/2)-obj[j]),2);
     }
     for (i=iStart+1;i<t->maxChildren;i++){
       if (t->treeChildren[i]!=NULL){
         distMiddle=0;
         for (j=0;j<t->nbCrit;j++){
           distMiddle=distMiddle+pow((((t->treeChildren[i]->localNadir[j]+t->treeChildren[i]->localIdeal[j])/2)-obj[j]),2);
         }
         if (distMiddle<distMinMiddle){
           nodeMin=i;
           distMinMiddle=distMiddle;
         }
       }
     }
     insertTree(t->treeChildren[nodeMin],x,obj);
   }
}

void updateIdealNadir(TreeNDS* t,int* obj){
  int i;
  int changed=0;
  for (i=0;i<t->nbCrit;i++){
    if (obj[i]<t->localNadir[i]){
      changed=1;
      t->localNadir[i]=obj[i];
    }
    if (obj[i]>t->localIdeal[i]){
      changed=1;
      t->localIdeal[i]=obj[i];
    }
  }

  if (changed==1){
    if (t->parent!=NULL){
      updateIdealNadir(t->parent,obj);
    }
  }
}

void splitTree(TreeNDS* t){
  int i,j,k,cpt;
  double distAvgMax;
  double distAvg;
  double somDist;
  int solMax;

  distAvgMax=0;
    
  solMax=0;
  for (i=0;i<t->localTabNDS->currentSize;i++){
    somDist=0;
    for (j=0;j<t->localTabNDS->currentSize;j++){
      if (i!=j){
        somDist=somDist+euclideanDistance(t->localTabNDS->tab[i].obj,t->localTabNDS->tab[j].obj,t->nbCrit);
      }
    }
    distAvg=somDist/(t->localTabNDS->currentSize-1);
    if (distAvg>distAvgMax){
      distAvgMax=distAvg;
      solMax=i;
    }
  }
  
  t->treeChildren[t->nbChildren]=initializationTreeNDS(t->nbCrit,t->nbVar,t->maxChildren,t->maxLocalTabSize);
  t->treeChildren[t->nbChildren]->parent=t;
  addEnd(t->treeChildren[t->nbChildren]->localTabNDS,t->localTabNDS->tab[solMax].x,t->localTabNDS->tab[solMax].obj);
  for (j=0;j<t->nbCrit;j++){
    t->treeChildren[t->nbChildren]->localNadir[j]=(t->localTabNDS->tab[solMax].obj[j]);
    t->treeChildren[t->nbChildren]->localIdeal[j]=(t->localTabNDS->tab[solMax].obj[j]);
  }
	
  t->nbChildren++;
  deleteElement(solMax,t->localTabNDS);
  
  while (t->nbChildren<t->maxChildren){
    distAvgMax=0;
    solMax=0;
    for (i=0;i<t->localTabNDS->currentSize;i++){
      somDist=0;
      cpt = 0; 
      for (j=0;j<t->nbChildren;j++){
        for (k=0;k<t->treeChildren[j]->localTabNDS->currentSize;k++){
          somDist=somDist+euclideanDistance(t->localTabNDS->tab[i].obj,t->treeChildren[j]->localTabNDS->tab[k].obj,t->nbCrit);
          cpt++;
        }
      }
      distAvg=somDist/cpt;
			
      if (distAvg>distAvgMax){
        distAvgMax=distAvg;
        solMax=i;
      }
    }

    t->treeChildren[t->nbChildren]=initializationTreeNDS(t->nbCrit,t->nbVar,t->maxChildren,t->maxLocalTabSize);
    t->treeChildren[t->nbChildren]->parent=t;
    addEnd(t->treeChildren[t->nbChildren]->localTabNDS,t->localTabNDS->tab[solMax].x,t->localTabNDS->tab[solMax].obj);
    for (j=0;j<t->nbCrit;j++){
      t->treeChildren[t->nbChildren]->localNadir[j]=(t->localTabNDS->tab[solMax].obj[j]);
      t->treeChildren[t->nbChildren]->localIdeal[j]=(t->localTabNDS->tab[solMax].obj[j]);
    }

    t->nbChildren++;
    deleteElement(solMax,t->localTabNDS);
  }
  CaseSolution c;
  int nodeMin;
  double distMiddle,distMinMiddle;

  while (t->localTabNDS->currentSize>0){
    c=t->localTabNDS->tab[t->localTabNDS->currentSize-1];
    nodeMin=0;
    distMinMiddle=0;
    for (j=0;j<t->nbCrit;j++){
      distMinMiddle=distMinMiddle+pow((((t->treeChildren[0]->localNadir[j]+t->treeChildren[0]->localIdeal[j])/2)-c.obj[j]),2);
    }
   
    for (i=1;i<t->maxChildren;i++){
      if (t->treeChildren[i]!=NULL){
         distMiddle=0;
         for (j=0;j<t->nbCrit;j++){
           distMiddle=distMiddle+pow((((t->treeChildren[i]->localNadir[j]+t->treeChildren[i]->localIdeal[j])/2)-c.obj[j]),2);
         }
         if (distMiddle<distMinMiddle){
           nodeMin=i;
           distMinMiddle=distMiddle;
         }
      }
    }
    addEnd(t->treeChildren[nodeMin]->localTabNDS,c.x,c.obj);
    for (j=0;j<t->nbCrit;j++){
      if (c.obj[j] < t->treeChildren[nodeMin]->localNadir[j])
        t->treeChildren[nodeMin]->localNadir[j]=c.obj[j];
      if (c.obj[j] > t->treeChildren[nodeMin]->localIdeal[j])
        t->treeChildren[nodeMin]->localIdeal[j]=c.obj[j];
    }
    deleteElement(t->localTabNDS->currentSize-1,t->localTabNDS);
     
  }
}

TreeNDS* deleteTreeNDS(TreeNDS* t){
  int i=0;
  if (t->localNadir)
    free(t->localNadir);	
  if (t->localIdeal)
    free(t->localIdeal);
  if (t->localTabNDS)
    deleteTabNDS(t->localTabNDS);
  
  if (t->nbChildren>0){
    for (i=0;i<t->maxChildren;i++){   
      if (t->treeChildren[i]!=NULL)
        deleteTreeNDS(t->treeChildren[i]);
    }
  }
  if (t->treeChildren)
  	free(t->treeChildren);
  if (t)
  	free(t);
  return NULL;
}

