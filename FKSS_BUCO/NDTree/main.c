
/*Copyright 

Andrzej Jaszkiewicz, Thibaut Lust:
ND-Tree-Based Update: A Fast Algorithm for the Dynamic Nondominance Problem. IEEE Trans. Evol. Comput. 22(5): 778-791 (2018)

C Version of ND-Tree (Thibaut Lust, thibaut.lust at lip6.fr)

*/

#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include <time.h>	
#include <math.h>
#include <limits.h>

#include "tabNDS.h"
#include "treeNDS.h"
#include "dominance.h"

#define MAXFILENAME 100

	
int main(){
	printf("ND-Tree on min-knapsack problem\n");
	srand(5);
	int i,j,k;
	int nbCrit=5;
	int nbVar=100;
	//ND-Tree parameters
	int nbMaxChildren=nbCrit+1; //max number of children of a node (the value NbCrit+1 is recommaded)
  int sizeMaxLocalArray=max(nbMaxChildren,20); //max size of the local array >= nbMaxChildren (size 20 is recommanded, except when the number of criteria >20)
	
	TreeNDS *treeNDS; /* for minimization and for integer values*/
	treeNDS = initializationTreeNDS(nbCrit,nbVar,nbMaxChildren,sizeMaxLocalArray);
	
	/*example: min-knapsack problem: selecting a set of items such the total weight is >= capcity, and minimizing of "nbCrit" profits*/ 
	int MaxV=10;

	int** matCost = NULL;
	matCost = (int**)malloc(sizeof(int*)*nbVar);
	for (i=0;i<nbVar;i++){
		matCost[i]=(int*)malloc(sizeof(int)*nbCrit);
	}

	int* tabWeight = NULL;
	tabWeight = (int*)malloc(sizeof(int)*nbVar);
	for (i=0;i<nbVar;i++){
		for (j=0;j<nbCrit;j++){
			matCost[i][j]=rand()%MaxV;
		}
		tabWeight[i]=rand()%MaxV;	
	}
	int capacity=((MaxV/2)*nbVar)/2; //capacity is equal to about the total weight divided by 2
	/*a solution is encoded by an integer vector*/	
	int *x = NULL;
	x = (int*)malloc(sizeof(int)*nbVar);
	/*a solution is evaluated by an integer vector*/
	int *evalSol = NULL;
	evalSol = (int*)malloc(sizeof(int)*nbCrit);
	
	/* we randomly generate "nbTry" solutions*/
	
	int nbTry=100000;
	int sumW=0;
	
	for (i=1;i<=nbTry;i++){
		sumW=0;
		for (j=0;j<nbVar;j++){
			x[j]=0;
		}
		for (j=0;j<nbCrit;j++){
			evalSol[j]=0;
		}

		while (sumW<capacity){
			j=random()%nbVar;
			if (!x[j]){
				x[j]=1;
				sumW=sumW+tabWeight[j];
				for (k=0;k<nbCrit;k++){
					evalSol[k]=evalSol[k]+matCost[j][k];
				}
			}
		}
		
		if (sumW>=capacity){ //the solution is valid
			addSolutionTree(treeNDS,x,evalSol);
		}
	}
	
	char ft[MAXFILENAME+1];
	snprintf(ft,MAXFILENAME,"resultsTree_%d_%d.txt",nbCrit,nbVar);
	writeTreeNDS(treeNDS,0,ft);
	displayTree(treeNDS);
	printf("Size of the tree = %d\n",totalSizeTree(treeNDS));
	deleteTreeNDS(treeNDS);
	
	if (x)	
		free(x);
	if (evalSol) 	
		free(evalSol);
	if (tabWeight)
		free(tabWeight);

	if (matCost){
		for (i=0;i<nbVar;i++){
			free(matCost[i]);
		}
		free(matCost);
	}

	printf("End");
	
	return 0;

}





























