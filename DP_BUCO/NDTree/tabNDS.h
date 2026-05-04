#ifndef __TABNDS_H__
#define __TABNDS_H__	

#include "tabSol.h"
#include "dominance.h"

extern double nDCTab;
 
typedef struct tabNDS{
   int nbCrit;
   int nbVar;
   int currentSize;
   int maxSize;
   CaseSolution* tab;	
} TabNDS;
   
TabNDS* initializationTabNDS(int nbCrit,int nbVar,int maxSize);
void addHeadTab(TabNDS* t,int *x,int *obj);
void addEnd(TabNDS* t,int *x,int *obj);;
void deleteElement(int position,TabNDS* t);
int addSolutionTab(TabNDS* t,int *x,int *obj);
void displayTabNDS(TabNDS* l,int all);
void writeTabNDS(TabNDS* l,int all,char* name);
void deleteTabNDS(TabNDS* t);
int checkSolutionTab(TabNDS* t,int *obj);
void writeTabNDSWithoutLength(TabNDS* t,int all,FILE *fListNDS);


#endif




