#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "NDTree/treeNDS.h"
#include "knapsackDP.h"

/// Copy all solutions from tree t to tResult
void scanTreeToTreeSimple(TreeNDS* t, TreeNDS* tResult){
    if(!t) return;
    if (t->nbChildren==0){
        for (int i=0;i<t->localTabNDS->currentSize;i++){
            addSolutionTree(tResult,
                            t->localTabNDS->tab[i].x,
                            t->localTabNDS->tab[i].obj);
        }
    } else {
        for (int i=0;i<t->maxChildren;i++)
            if (t->treeChildren[i]!=NULL)
                scanTreeToTreeSimple(t->treeChildren[i], tResult);
    }
}

void scanTreeToTreeSimpleBUCO(TreeNDS* t, TreeNDS* tResult, int W, int capacity){
    if (!t) return;

    if (t->nbChildren == 0){
        for (int i = 0; i < t->localTabNDS->currentSize; i++){
            int *x   = malloc(tResult->nbVar * sizeof(int));
            int *obj = malloc(tResult->nbCrit * sizeof(int));

            if (!x || !obj) {
                free(x);
                free(obj);
                fprintf(stderr, "Memory allocation failed in scanTreeToTreeSimpleBUCO\n");
                exit(EXIT_FAILURE);
            }

            memcpy(x, t->localTabNDS->tab[i].x, tResult->nbVar * sizeof(int));

            for (int j = 0; j < t->nbCrit; j++) {
                obj[j] = t->localTabNDS->tab[i].obj[j];
            }

            obj[tResult->nbCrit - 1] = capacity - W;

            addSolutionTree(tResult, x, obj);

            free(x);
            free(obj);
        }
    } else {
        for (int i = 0; i < t->maxChildren; i++){
            if (t->treeChildren[i] != NULL) {
                scanTreeToTreeSimpleBUCO(t->treeChildren[i], tResult, W, capacity);
            }
        }
    }
}


/// Transition: add a packet of k lightest objects of category c
void scanTreeWithPacket(
    int w, int n, int m, int capacity,
    int pw, const int *pp,
    const int *catIndices, int k,   // first k objects of this category
    TreeNDS* tprev, TreeNDS** tcurr
){
    if(!tprev) return;
    int *new_x   = (int*)malloc(n * sizeof(int));
    int *new_obj = (int*)malloc(m * sizeof(int));

    if (tprev->nbChildren==0){
        for (int s=0; s<tprev->localTabNDS->currentSize; s++){
            // add packet
            int new_w = w + pw;
            if (new_w <= capacity){
                memcpy(new_x,   tprev->localTabNDS->tab[s].x,   n*sizeof(int));
                memcpy(new_obj, tprev->localTabNDS->tab[s].obj, m*sizeof(int));

                // mark first k objects of this category
                for (int u=0; u<k; u++){
                    int idx = catIndices[u];
                    new_x[idx] = 1;
                }
                // add profits
                for (int j=0;j<m;j++)
                    new_obj[j] += pp[j];

                addSolutionTree(tcurr[new_w], new_x, new_obj);
            }
        }
    } else {
        for (int s=0;s<tprev->maxChildren;s++)
            if (tprev->treeChildren[s]!=NULL)
                scanTreeWithPacket(w,n,m,capacity,pw,pp,catIndices,k,tprev->treeChildren[s],tcurr);
    }

    free(new_x);
    free(new_obj);
}




/// Main solver
int knapsack_DP_BUCO(
    int n, int m, int C,
    const int *weights,
    const int *profits,
    const int *categories_by_item,   // category of each object (0..m-1), already sorted by weight
    TreeNDS **out_result
){
    if(!weights || !profits || !categories_by_item || !out_result || n<=0 || m<=0 || C<0) return -1;

    int maxChildren=m+1; // max children per node in DP tree (one for each category + one for "no more categories")
    int sizeMaxLocalArray=10000; // max size of local Pareto front at each node (tune for performance)

    // Count objects per category
    int *count = calloc(m,sizeof(int));
    for(int i=0;i<n;i++) count[categories_by_item[i]]++;

    // Build catIndices
    int **catIndices = malloc(m*sizeof(int*));
    int *pos = calloc(m,sizeof(int));
    for(int c=0;c<m;c++)
        catIndices[c] = malloc(count[c]*sizeof(int));
    for(int i=0;i<n;i++){
        int c = categories_by_item[i];
        catIndices[c][pos[c]++] = i;
    }
    free(pos);

    // Prefix sums for weight and profits
    int **prefixW = malloc(m*sizeof(int*));
    int ***prefixP = malloc(m*sizeof(int**));

    for(int c=0;c<m;c++){
        int nc = count[c];
        prefixW[c] = calloc(nc+1,sizeof(int));
        prefixP[c] = malloc((nc+1)*sizeof(int*));
        for(int k=0;k<=nc;k++)
            prefixP[c][k] = calloc(m,sizeof(int));

        for(int k=1;k<=nc;k++){
            int idx = catIndices[c][k-1];
            prefixW[c][k] = prefixW[c][k-1] + weights[idx];
            for(int j=0;j<m;j++)
                prefixP[c][k][j] = prefixP[c][k-1][j] + profits[idx*m+j];
        }
    }

    // DP arrays
    TreeNDS **dp_prev = malloc((C+1)*sizeof(TreeNDS*));
    TreeNDS **dp_curr = malloc((C+1)*sizeof(TreeNDS*));
    for (int w=0; w<=C; ++w){
        dp_prev[w] = initializationTreeNDS(m, n, maxChildren, sizeMaxLocalArray);
        dp_curr[w] = initializationTreeNDS(m, n, maxChildren, sizeMaxLocalArray);
    }

    // initial state
    int *x0 = calloc(n,sizeof(int));
    int *o0 = calloc(m,sizeof(int));
    addSolutionTree(dp_prev[0], x0, o0);
    free(x0); free(o0);

    // ---- DP loop by category ----
    for (int c=0;c<m;c++){
        int nc = count[c];

        // clear dp_curr before using
        for (int w=0; w<=C; ++w){
            deleteTreeNDS(dp_curr[w]);
            dp_curr[w] = initializationTreeNDS(m, n, maxChildren, sizeMaxLocalArray);
        }

        for (int w=0; w<=C; ++w){
            //printf("Processing category %d/%d, weight %d/%d  \n", c+1, m, w, C);
            if (!dp_prev[w]) continue;

            // case 1: take nothing from this category
            scanTreeToTreeSimple(dp_prev[w], dp_curr[w]);

            // case 2: take a packet of k lightest
            for (int k=1;k<=nc;k++){
                int pw = prefixW[c][k];
                int *pp = prefixP[c][k];

                if (pw > C) break; // can't fit

                scanTreeWithPacket(
                    w, n, m, C,
                    pw, pp,
                    catIndices[c], k,
                    dp_prev[w], dp_curr
                );
            }
        }

        // swap for next category
        TreeNDS **tmp = dp_prev; dp_prev = dp_curr; dp_curr = tmp;
    }

    // Final aggregation
    TreeNDS *result = initializationTreeNDS(m+1,n, maxChildren+1, sizeMaxLocalArray);
    for (int w=0; w<=C; ++w){
        if (dp_prev[w]) scanTreeToTreeSimpleBUCO(dp_prev[w], result,w,C);
    }

    //printf("Total non-dominated solutions before final pruning: %d\n", totalSizeTree(result));

    // Cleanup
    for (int w=0; w<=C; ++w){ deleteTreeNDS(dp_prev[w]); deleteTreeNDS(dp_curr[w]); }
    free(dp_prev); free(dp_curr);

    for(int c=0;c<m;c++){
        for(int k=0;k<=count[c];k++) free(prefixP[c][k]);
        free(prefixP[c]);
        free(prefixW[c]);
        free(catIndices[c]);
    }
    free(prefixP); free(prefixW); free(catIndices); free(count);

    *out_result = result;
    return 0;
}