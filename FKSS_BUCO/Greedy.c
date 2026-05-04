#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "NDTree/treeNDS.h"
#include "Greedy.h"

int get_weight(int *g_S,int **items_per_category,int nb_categories,const int* weights){
    // We calculate the weight of the solution by summing the weights of the selected items in each category
    int *items=malloc(nb_categories*sizeof(int));

    items[0]=g_S[0];

    for (int i=1;i<nb_categories;i++){
        items[i]=g_S[i]-g_S[i-1];
    }

    int weight_sol=0;

    for (int j=0;j < nb_categories;j++){
        for(int i=1;i<= items[j];i++){
            weight_sol=weight_sol+weights[items_per_category[j][i]];
        }
    }

    free(items);
    return weight_sol;
}

int is_realisable(int *g_S,int *nb_item_per_category,int num_obj){
    // We compute the number of items selected in each category and check if it is less than the number of items available in that category
    int realisable=1;

    int *items=malloc(num_obj*sizeof(int));

    items[0]=g_S[0];

    for (int i=1;i<num_obj;i++){
        items[i]=g_S[i]-g_S[i-1];
    }

    for (int i=0;i<num_obj;i++){
        if (items[i] > nb_item_per_category[i]) {
            realisable=0;
        }
    }

    free(items);
    return realisable;
}

void GreedyAlgorithm(TreeNDS* archive,int** items_per_category,int *nb_item_per_category,int sum_weight,int nb_categories,const int* weights,int * V,size_t count){

    for (size_t i=0;i<count;i++){
        int *g_S=malloc(nb_categories*sizeof(int));
        for(int j=0;j<nb_categories;j++){
            g_S[j]=V[i*nb_categories+j];
        }

        int realisable=is_realisable(g_S,nb_item_per_category,nb_categories);
        
        // Check if the solution is realisable, if it is, we compute its weight and add it to the archive with the corresponding objective values
        if (realisable==1){
            int weight_sol=get_weight(g_S,items_per_category,nb_categories,weights);
            int *g_S_new=malloc((nb_categories+1)*sizeof(int));
            for (int j=0;j<nb_categories;j++){
                g_S_new[j]=g_S[j];
            }
            g_S_new[nb_categories]=sum_weight-weight_sol;

            int *obj=(int*)calloc(nb_categories+1, sizeof(int));
            for (int j=0;j<nb_categories+1;j++){
                obj[j]=g_S_new[j];
            }
            // We add the solution to the archive
            addSolutionTree(archive,g_S_new,obj);
            //printf("REALISABLE \n");
        }
    }
    return;

}