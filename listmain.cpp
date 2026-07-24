#include "list.h"
#include "my_libs/error_manage.hpp"

extern error_t error;

int main(void) {
    list_t list = {};
    listCtor(&list);

    add_after(&list, 0,  "hi0");
    add_before(&list, 0, "hi1");
    add_before(&list, 1, "hi2");
    add_before(&list, 2, "hi3");
    add_before(&list, 3, "hi4");
    add_before(&list, 4, "hi5");
    add_before(&list, 0, "hi6");
    add_before(&list, 1, "hi7");

    remove(&list, 3);
    remove(&list, 2);
    remove(&list, 1);

    listDtor(&list);

    if (error.is_error)
        print_error(error_text, error);
    // open_live_server(dump_site_file_name);
    return 0;
}