#ifndef LIST_DUMP_H
#define LIST_DUMP_H

#include "list.h"

error_t open_live_server(const char * site_name);
void print_to_html(list_t *list, operations operation, size_t index, int value);
error_t create_dot_image_next_array_dump(list_t *list);
error_t create_dot_image_prev_array_dump(list_t *list);
void add_error_to_html(list_t *list, int index, int value);
error_t create_dot_main_array_dump(list_t *list, FILE * fp);
void print_site_toes();
void print_info(list_t *list);

#endif // LIST_DUMP_H