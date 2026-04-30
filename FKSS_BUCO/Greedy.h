#ifndef __Greedy_h__
#define __Greedy_h__
#include "NDTree/treeNDS.h"

void GreedyAlgorithm(TreeNDS* archive,int** items_per_category,int *nb_item_per_category,int sum_weight,int nb_categories,const int* weights,int *V,size_t count);

#endif