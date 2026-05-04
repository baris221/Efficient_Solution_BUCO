#include "knapsackDP.h"
#include "read_instance.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <instance_file> <result_file>\n", argv[0]);
        return 1;
    }

    clock_t start = clock();
    read_instance(argv[1]);
    int sum_weight = 0;
    for (int i=0;i<nb_items;i++) sum_weight += weights[i];
    //printf("Read instance: %d items, %d categories, capacity %d\n", nb_items, num_obj, sum_weight);
    TreeNDS *result;
    int res = knapsack_DP_BUCO(nb_items, nb_categories, sum_weight, weights, profits, categories_by_item, &result);
    if (res==-1) {
        printf("Error in knapsack_DP_BUCO\n");
        return -1;
    }

    clock_t end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", cpu_time_used);
    printf("Number of non dominated solutions: %d\n", totalSizeTree(result));

    writeTreeNDS(result,0, argv[2]);

    deleteTreeNDS(result);
    return 0;
}