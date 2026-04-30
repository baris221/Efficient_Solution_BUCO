#ifndef __TREENDS_H__
#define __TREENDS_H__

#include "tabNDS.h"


extern double nDCTree;
extern double nbCallUpdate;

typedef struct treeNDS{
   int nbCrit;
   int nbVar;
   int nbChildren;
   int maxChildren;
   int* localNadir;
   int* localIdeal;
   int maxLocalTabSize;
   TabNDS* localTabNDS;
   struct treeNDS** treeChildren;
   struct treeNDS* parent;
}TreeNDS;

TreeNDS* initializationTreeNDS(int nbCrit,int nbVar,int maxChildren,int maxLocalTabSize);
double max(double a,double b);
void updateIdealNadir(TreeNDS* t,int* obj);
void displayTree(TreeNDS* t);
int totalSizeTree(TreeNDS* t);
int totalNbLeaves(TreeNDS* t);
int totalNbInternalNodes(TreeNDS* t);
double euclideanDistance(int* obj1,int* obj2,int nbCrit);
void initializationNadirIdealTreeNDS(TreeNDS* t,int* nadir,int* ideal);
int addSolutionTreeFirst(TreeNDS* t,int *x,int *obj);
int addSolutionTree(TreeNDS* t,int *x,int *obj);
void upDateNode(TreeNDS** t,int *x,int *obj,int* added);
void insertTree(TreeNDS* t,int *x,int *obj);
void splitTree(TreeNDS* t);
TreeNDS* deleteTreeNDS(TreeNDS* t);
void writeTreeNDS(TreeNDS* t,int all,char* name);
void writeScanTreeNDS(TreeNDS* t,int all,FILE *fListNDS);


#endif
