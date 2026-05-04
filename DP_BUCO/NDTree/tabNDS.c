#include <stdio.h>
#include <stdlib.h>
#include "tabNDS.h"
#include "tabSol.h"
#include "dominance.h"

//tabNDS is managed as a dynamic array structure, we multiply by two the size of the array each time we need more spaces 

//Initialization of the structure
TabNDS* initializationTabNDS(int nbCrit,int nbVar,int maxSize){
  TabNDS* t = (TabNDS*) malloc(sizeof(TabNDS));
  t->nbCrit=nbCrit;
  t->nbVar=nbVar;
  t->currentSize=0;
  t->maxSize=maxSize;		
  t->tab=(CaseSolution*)malloc(sizeof(CaseSolution)*t->maxSize);
  return t; 
}

//we add at the head of the tab 
void addHeadTab(TabNDS* t,int *x,int *obj){
  int i;
  //we multiply by 2 the space allocated if the maxsize has been reached
  if (t->currentSize>=t->maxSize){
    t->maxSize=t->maxSize*2;
    t->tab=realloc(t->tab,sizeof(CaseSolution)*t->maxSize);
  }
  for (i=(t->currentSize);i>0;i--)
    t->tab[i]=t->tab[i-1];	
  CaseSolution c; 
  c.x = (int*)malloc(sizeof(int)*(t->nbVar));
  c.obj = (int*)malloc(sizeof(int)*(t->nbCrit));
  for (i=0;i<(t->nbVar);i++)
    c.x[i]=x[i];
  for (i=0;i<(t->nbCrit);i++)
    c.obj[i]=obj[i];
 
  t->tab[0]=c;	
  t->currentSize=t->currentSize+1;
}

//we add at the end of the tab 
void addEnd(TabNDS* t,int *x,int *obj){
  int i=0;
  //we multiply by 2 the space allocevaated if the maxsize has been reached
  if (t->currentSize>=t->maxSize){
    t->maxSize=t->maxSize*2;
    t->tab=realloc(t->tab,sizeof(CaseSolution)*t->maxSize);
  }
  CaseSolution c;
  c.x = (int*)malloc(sizeof(int)*(t->nbVar));
  c.obj = (int*)malloc(sizeof(int)*(t->nbCrit));
  for (i=0;i<(t->nbVar);i++)
    c.x[i]=x[i];
  for (i=0;i<(t->nbCrit);i++)
    c.obj[i]=obj[i];
  t->tab[t->currentSize]=c;
  t->currentSize=t->currentSize+1;
}

//we delete an element at position "position"
void deleteElement(int position,TabNDS* t){
  int i;
  if (t->tab[position].x)
    free(t->tab[position].x);
  if (t->tab[position].obj)
    free(t->tab[position].obj);	
  for (i=position;i<((t->currentSize)-1);i++)  
    t->tab[i]=t->tab[i+1];	
  t->currentSize=t->currentSize-1;	
}


int checkSolutionTab(TabNDS* t,int *obj){  
  int i, result;
  if (t->currentSize==0){
    return 1;     
  }
  else{ //we browse the tab
     i=0;
     while (i<(t->currentSize)){	
       result=dominatePareto(obj,t->tab[i].obj,t->nbCrit);
       if ((result==2)||(result==4)){
         return 0; 
       }
       if (result==2){
         return 0; 
       }
       if (result==1){ //we delete the solution
         deleteElement(i,t);
       }
       if (result==3){
         i++;
       }
     }
     return 1;          
  }   
}

int addSolutionTab(TabNDS* t,int *x,int *obj){  
  int i, result;
  if (t->currentSize==0){
    addHeadTab(t,x,obj);  
    return 1;     
  }
  else{ //we browse the tab
     i=0;
     while (i<(t->currentSize)){	
       result=dominatePareto(obj,t->tab[i].obj,t->nbCrit);
       if ((result==2)||(result==4)){
         return 0;
       }
       if (result==1){ //we delete the solution
         deleteElement(i,t); 
       }
       if (result==3){
         i++;
       }
     }
     //we have to add the solution  
     addEnd(t,x,obj);  
     return 1;          
  }   
}


void deleteTabNDS(TabNDS* t){
  deleteTab(t->tab,t->currentSize);
  if (t->tab)
    free(t->tab);
  t->currentSize=0;
  if (t)
    free(t);	
}

void displayTabNDS(TabNDS* t,int all){
  printf("Size of the tab = %d\n",t->currentSize);
  displayTab(t->tab,t->currentSize,t->nbCrit,t->nbVar,all);
}

void writeTabNDS(TabNDS* t,int all,char* name){
  FILE *fListNDS = fopen(name,"w");
  fprintf(fListNDS,"Length of the list   = %d\n",t->currentSize);
  writeTab(fListNDS,t->currentSize,t->tab,t->nbCrit,t->nbVar,all);
  fclose(fListNDS);
}

void writeTabNDSWithoutLength(TabNDS* t,int all,FILE *fListNDS){
  writeTab(fListNDS,t->currentSize,t->tab,t->nbCrit,t->nbVar,all);
}
  
  




     


