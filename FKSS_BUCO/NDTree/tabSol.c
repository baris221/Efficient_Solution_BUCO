#include <stdio.h>
#include <stdlib.h>
#include "tabSol.h"

void displayTab(CaseSolution* t,int currentSize, int nbCrit,int nbVar,int all){
  int i;
  int j=0;
  while (j<currentSize){
    for (i=0;i<nbCrit-1;i++)
      printf("%d ",t[j].obj[i]);
    printf("%d\n",t[j].obj[nbCrit-1]); 
    if (all==1){ //We also display the solutions
      for (i=0;i<nbVar-1;i++)
        printf("%d ",t[j].x[i]);
      printf("%d\n",t[j].x[nbVar-1]);    
    }
    j++;
  }
}


void writeTab(FILE* fListNDS,int currentSize, CaseSolution* t,int nbCrit,int nbVar,int all){
  int i;
  int j=0;
  while (j<currentSize){
    for (i=0;i<nbCrit-1;i++)
      fprintf(fListNDS,"%d ",t[j].obj[i]);
    fprintf(fListNDS,"%d\n",t[j].obj[nbCrit-1]); 
    if (all==1){ //We also display the solutions
      for (i=0;i<nbVar-1;i++)
        fprintf(fListNDS,"%d ",t[j].x[i]);
      fprintf(fListNDS,"%d\n",t[j].x[nbVar-1]);    
    }
    j++;
  }  
}

void deleteTab(CaseSolution* t,int currentSize){
  int i;
  CaseSolution c;
  for (i=0;i<currentSize;i++){
    c=t[i];
    if (c.x)
      free(c.x);
    if (c.obj)
      free(c.obj);
  }
}
  
   
    
      
    
  
