#ifndef LIST_DUMP_H
#define LIST_DUMP_H

#include "list.h"
#include "list_values.h"

LstErrors open_live_server(const char * site_name);
LstErrors print_to_html(list_t *list, operations operation, size_t index, list_el_t value);
LstErrors create_dot_image_next_array_dump(list_t *list);
LstErrors create_dot_image_prev_array_dump(list_t *list);
LstErrors create_dot_image_dump(list_t *list);
LstErrors add_error_to_html(list_t *list, int index, list_el_t value);
LstErrors create_dot_main_array_dump(list_t *list, FILE * fp);
LstErrors verify_list(list_t *list);
void print_site_toes();
void print_site_headers();
void print_info(list_t *list);

#endif // LIST_DUMP_H