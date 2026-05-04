#include "knapsackTS.h"
#include "read_instance.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <instance_file> <result_file>\n", argv[0]);
        return 1;
    }

    clock_t start, end;
    start = clock();
    read_instance(argv[1]);
    int sum_weight = 0;
    for (int i=0;i<nb_items;i++) sum_weight += weights[i];

    int **items_per_category = malloc(nb_categories * sizeof(int *));
    int *counter = malloc(nb_categories * sizeof(int));

    for (int j = 0; j < nb_categories; j++) {

        items_per_category[j] = malloc((nb_item_per_category[j] + 1) * sizeof(int));
        if (items_per_category[j] == NULL) {
            printf("malloc failed for class %d\n", j);
            return 1;
        }

        items_per_category[j][0] = -1;
        counter[j] = 1;
    }

    for (int i = 0; i < nb_items; i++) {

        if (categories_by_item[i] < 0 || categories_by_item[i] >= nb_categories) {
            printf("Invalid class index categories by item [%d] = %d\n", i, categories_by_item[i]);
            return 1;
        }

        if (counter[categories_by_item[i]] > nb_item_per_category[categories_by_item[i]]) {
            printf("Too many items in class %d\n", categories_by_item[i]);
            return 1;
        }

        items_per_category[categories_by_item[i]][counter[categories_by_item[i]]] = i;
        counter[categories_by_item[i]]++;
    }

    int *g_S=malloc((nb_categories+1)*sizeof(int));
    for(int i=0;i<nb_categories;i++){
        g_S[i]=0;
    }

    g_S[nb_categories]=sum_weight;



    TreeNDS* res=initializationTreeNDS(nb_categories+1,nb_categories+1,4,100);
    TreeSearch(res,items_per_category,nb_item_per_category,0,nb_categories,g_S,weights);

    end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", time_spent);
    printf("Number of non dominated solutions : %d\n", totalSizeTree(res));

    char ft[100];
    snprintf(ft,100, "%s", argv[2]);
    writeTreeNDS(res,0,ft);

    return 0;
}