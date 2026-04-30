#ifndef __knapsackTS_h__
#define __knapsackTS_h__
#include "NDTree/treeNDS.h"

void TreeSearch(TreeNDS* archive,int** objets_per_classe,int *niveaux_per_objets,int classe_actuelle,int num_obj,int *p,const int* weights);

#endif