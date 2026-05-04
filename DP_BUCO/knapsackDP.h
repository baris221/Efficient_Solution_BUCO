#ifndef __knapsackDP_h__
#define __knapsackDP_h__
#include "NDTree/treeNDS.h"

void scanTreeToTreeSimpleBUCO(TreeNDS* t, TreeNDS* tResult,int W,int capacity);
void scanTreeWithPacket(int w, int n, int m, int capacity,int pw,const int *pp,const int *catIndices,int k,TreeNDS* tprev, TreeNDS** tcurr);
int knapsack_DP_BUCO(int n, int m, int C,const int *weights, const int *profits, const int *cls,TreeNDS **out_result);

#endif