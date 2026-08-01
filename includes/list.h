#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdio.h>

typedef const char * const string;
#ifdef STRING_TYPE
    typedef char * list_el_t;
    typedef string list_el_const_t;
#else
    typedef int list_el_t;
    typedef int list_el_const_t;
#endif // STRING_TYPE

#ifndef _LIST_DUMP_SITE_FILE_PATH
#define _LIST_DUMP_SITE_FILE_PATH "list_dump.html"
#endif

#ifndef _LIST_DUMP_GRAPH_DIR_NAME
#define _LIST_DUMP_GRAPH_DIR_NAME "graph/graph"
#endif

enum LstErrors {
    _LST_OK                             =    0,
    _LST_ERR_CMD_INVALID                =   -1,
    _LST_ERR_INVALID_INDEX              =   -2,
    _LST_ERR_OUT_OF_BOUNDS              =   -3,
    _LST_ERR_NOTHING_TO_POP             =   -4,
    _LST_ERR_INVALID_OPERATION          =   -5,
    _LST_ERR_REALLOC_FAIL               =   -6,
    _LST_ERR_INCORRECT_LIST             =   -7,
    _LST_ERR_CYCLING_LIST               =   -8,
    _LST_ERR_INVALID_ARGUMENTS          =   -9,
    _LST_ERR_INVALID_PREV               =   -10,
    _LST_ERR_INVALID_NEXT               =   -11,
    _LST_ERR_INVALID_SIZE               =   -12,
    _LST_ERR_INVALID_RELATION_WITH_PREV =   -13,
    _LST_ERR_INVALID_RELATION_WITH_NEXT =   -14,
    _LST_ERR_PIPE_FAIL                  =   -15,
    _LST_ERR_PTR_NULL                   =   -16,
    _LST_NOK                            =   -17,
    _LST_ERR_FILE_DOES_NOT_EXIST        =   -18
};

typedef struct {
    list_el_t *data;
    int *next;
    int *prev;
    int *free;

    int tail;
    int head;
    int last_free;

    int size;
    int capacity;
} list_t;

void listDtor(list_t *list);
void listCtor(list_t *list);
void print_order_of_data(FILE * fp, list_t *list);
LstErrors remove(list_t *list, int index);
LstErrors add_after(list_t *list, int index,  list_el_const_t value);
LstErrors add_before(list_t *list, int index, list_el_const_t value);
LstErrors append(list_t *list, list_el_const_t value);

void lst_perror(void);
void lst_perror_file(FILE* out);

#endif // LIST_H