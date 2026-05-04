#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256

int nb_categories;
int *nb_item_per_category;
int *weights;
int *profits;
int *categories_by_item;
int nb_items=0;

void read_instance(const char* filename){
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }


    int nb_variables = 0;

    char line[MAX_LINE];

    // Lire VARIABLES
    fgets(line, MAX_LINE, file);
    sscanf(line, "VARIABLES %d", &nb_variables);

    // Lire NB_OBJECTIF
    fgets(line,MAX_LINE,file);
    sscanf(line, "NB_OBJ %d",&nb_categories);


    // Lire ITEMS
    fgets(line, MAX_LINE, file);  // "ITEMS"

    // Allouer la mémoire pour les objets
    profits = (int*)malloc(nb_variables*nb_categories * sizeof(int));
    weights = (int*)malloc(nb_variables * sizeof(int));
    categories_by_item = (int*)malloc(nb_variables * sizeof(int));
    nb_item_per_category=(int*)malloc(nb_categories*sizeof(int));

    for (int j=0;j < nb_categories;j++){
        nb_item_per_category[j]=0;
    }

    // Lire les objets
    nb_items = 0;
    while (fgets(line, MAX_LINE, file) && nb_items < nb_variables) {
        char* start = strchr(line, '[');
        char* end   = strchr(line, ']');
        if (!start || !end) continue;

        *end = '\0'; // terminate inside bracket
        char* token = strtok(start + 1, ", ");
        int obj_idx = 0;

        while (token && obj_idx < nb_categories) {
            profits[nb_items * nb_categories + obj_idx] = atoi(token);
            token = strtok(NULL, ", ");
            obj_idx++;
        }

        // Read weight (after ']')
        int p = 0;
        sscanf(end + 1, "%d", &p);
        weights[nb_items] = p;

        // Compute cls as sum of profits for example (adjust to your need)
        int sum = 0;
        for (int k = 0; k < nb_categories; k++)
            sum += profits[nb_items * nb_categories + k];
        categories_by_item[nb_items] = nb_categories - sum;
        nb_item_per_category[categories_by_item[nb_items]]++;

        nb_items++;
    }

    fclose(file);
}

