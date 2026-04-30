#include "Greedy.h"
#include "read_instance.h"
#include "VectorEnum.h"
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
    //printf("Read instance: %d items, %d categories, capacity %d\n", nb_items, num_obj,sum_weight);

    int **items_per_category = malloc(nb_categories * sizeof(int *));
    int *counter = malloc(nb_categories * sizeof(int));
    
    size_t count = 0;
    int *V=generate_vectors_flat(nb_categories,nb_items,&count);

    //print_vectors_flat(V,count,num_obj);

    for (int j = 0; j < nb_categories; j++) {
        //printf("NUMBER OF OBJETS PER CLASS %d\n", niveaux_per_objets[j]);

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

    

    //printf("ALL IZZ WELL");


    TreeNDS* archive=initializationTreeNDS(nb_categories+1,nb_categories+1,4,100);
    GreedyAlgorithm(archive,items_per_category,nb_item_per_category,sum_weight,nb_categories,weights,V,count);

    end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", time_spent);
    printf("Number of non dominated solutions : %d\n", totalSizeTree(archive));

    char ft[100];
    snprintf(ft,100, "%s", argv[2]);
    writeTreeNDS(archive,0,ft);

    return 0;
}