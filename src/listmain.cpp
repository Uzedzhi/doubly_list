#include "../includes/list.h"

int main(void) {
    list_t list = {};
    listCtor(&list);

    add_after(&list,  0, 0);
    add_before(&list, 0, 1);
    add_before(&list, 1, 2);
    add_before(&list, 2, 3);
    add_before(&list, 3, 4);
    add_before(&list, 4, 5);
    add_before(&list, 0, 6);
    add_before(&list, 1, 7);

    remove(&list, 3);
    remove(&list, 2);
    remove(&list, 1);

    listDtor(&list);
    return 0;
}