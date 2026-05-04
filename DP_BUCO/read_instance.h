#ifndef readInstance_h
#define readInstance_h

extern int nb_categories;
extern int *nb_item_per_category;
extern int *weights;
extern int *profits;
extern int *categories_by_item;
extern int nb_items;

void read_instance(const char* filename);

#endif