#ifndef LIST_UNITEST_H
#define LIST_UNITEST_H

#include "list.h"

int unitest_list();
void reinitialize_list(list_t *list);
bool check_order(list_t *list, FILE *fp);
void skip_line(FILE * fp);
operations get_operation(char * operation);

#endif // LIST_UNITEST_H