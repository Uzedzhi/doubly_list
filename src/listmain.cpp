#include "../includes/list.h"

#define CHECK_FUNC(list, func) {\
    int res = func;\
    if (res < 0) {\
        lst_perror();\
        listDtor(&list);\
        return res;\
    }\
}

int main(void) {
    list_t list = {};
    listCtor(&list);

    CHECK_FUNC(list, add_after(&list, 0, 0);)
    CHECK_FUNC(list, add_after(&list, 1, 1);)
    CHECK_FUNC(list, add_after(&list, 2, 2);)
    CHECK_FUNC(list, add_after(&list, 3, 3);)
    CHECK_FUNC(list, add_after(&list, 4, 4);)
    CHECK_FUNC(list, add_after(&list, 5, 5);)
    CHECK_FUNC(list, add_after(&list, 1, 6);)
    CHECK_FUNC(list, add_after(&list, 1, 6);)
    CHECK_FUNC(list, add_after(&list, 1, 6);)
    CHECK_FUNC(list, add_after(&list, 1, 6);)
    CHECK_FUNC(list, add_after(&list, 2, 7);)

    remove(&list, 3);
    remove(&list, 2);
    remove(&list, 1);

    listDtor(&list);
    return 0;
}