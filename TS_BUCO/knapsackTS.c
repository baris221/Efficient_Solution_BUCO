#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "NDTree/treeNDS.h"
#include "knapsackTS.h"

void TreeSearch(TreeNDS* archive,int** items_per_category,int *nb_item_per_category,int j,int nb_categories,int *g_S,const int* weights){

    // We check if we have reached the last category, if so we add the solution to the archive
    if (j==nb_categories){
        int *obj=(int*)calloc(nb_categories+1, sizeof(int));
        for (int j=0;j<nb_categories+1;j++){
            obj[j]=g_S[j];
        }
        addSolutionTree(archive,g_S,obj);
        return;
    }
    for (int i=nb_item_per_category[j];i>=0;i--){
        if (i==0){
            TreeSearch(archive,items_per_category,nb_item_per_category,j+1,nb_categories,g_S,weights);  
            continue;
        }

        int *new_g_S=malloc((nb_categories+1)*sizeof(int));
        for(int k=0;k<nb_categories+1;k++){
            new_g_S[k]=g_S[k];
        }
        for(int k=j;k<nb_categories;k++){
            new_g_S[k]=new_g_S[k]+i;
        }

        if (j == 1){
            // Prune if the lightest item we take in the second category is heavier than the heaviest item we do not take in the first category
            if (new_g_S[0] < nb_item_per_category[0]){
                int item_non_taken = items_per_category[0][new_g_S[0]+1];
                int item_taken = items_per_category[1][i];

                if (weights[item_taken] >= weights[item_non_taken]){
                    continue;
                }
            }
        }
        
        if (j > 1){
            // Prune if the lightest item we take in the j+1 category is heavier than the heaviest item we do not take in the j category
            int taken_in_class = new_g_S[j-1] - new_g_S[j-2];

            if (taken_in_class < nb_item_per_category[j-1]){
                int item_non_taken = items_per_category[j-1][taken_in_class + 1];
                int item_taken= items_per_category[j][i];

                if (weights[item_taken] >= weights[item_non_taken]){
                    continue;
                }
            }
        }

        // We compute the new profile of the solution after taking the i items in the j category
        int weight_to_remove=0;

        for (int l=1;l<=i;l++){
            weight_to_remove=weight_to_remove+weights[items_per_category[j][l]];
        }
        new_g_S[nb_categories]=new_g_S[nb_categories]-weight_to_remove;

        TreeSearch(archive,items_per_category,nb_item_per_category,j+1,nb_categories,new_g_S,weights);
    }

}