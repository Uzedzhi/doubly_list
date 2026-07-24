#include "list.h"
#include "my_libs/error_manage.hpp"

extern error_t error;

int main(void) {
    list_t list = {};
    listCtor(&list);

    add_after(&list, 0, 9);
    add_before(&list, 0, 10);
    add_before(&list, 1, 11);
    add_before(&list, 2, 12);
    add_before(&list, 3, 13);
    add_before(&list, 4, 14);
    add_before(&list, 0, 20);
    add_before(&list, 1, 100);

    remove(&list, 3);
    remove(&list, 2);
    remove(&list, 1);

    listDtor(&list);

    if (error.is_error)
        print_error(error_text, error);
    // open_live_server(dump_site_file_name);
    return 0;
}