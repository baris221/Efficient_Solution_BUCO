#include <stdio.h>
#include <stdlib.h>

#ifndef __TABSOL_H__
#define __TABSOL_H__	

typedef struct caseSolution{
  int *x;                  
  int *obj;       
} CaseSolution;

void displayTab(CaseSolution* t,int currentSize, int nbCrit,int nbVar,int all);
void writeTab(FILE* fListNDS,int currentSize, CaseSolution* t,int nbCrit,int nbVar,int all);
void deleteTab(CaseSolution* t,int currentSize);


#endif
