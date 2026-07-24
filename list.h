#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdio.h>
#include "my_libs/error_manage.hpp"

const  size_t START_LIST_SIZE   = 10;
static size_t count             = 0;
const  size_t MAX_STR_SIZE      = 200;
const  size_t INITIAL_HEAD_VAL  = 0;
const  size_t INITIAL_TAIL_VAL  = 0;
const  size_t INITIAL_FREE_VAL  = 0;
const  size_t MAX_TESTING_SIZE  = 100;
const  int    POISON            = -1;

typedef const char * const string;
string unitest_file_name    = "unitest.txt";
string dump_graph_file_name = "graph.txt";
string dump_site_file_name  = "dump.html";
string operations_descriptions[]    = {"added after index to a list ", "before operation happened ", "after removal from list ", "added before index to the list "};
string operations_classes[]         = {"add_after", "start", "remove", "add_before"};
typedef struct {
    int *data;
    int *next;
    int *prev;
    int *free;

    int tail;
    int head;
    int last_free;

    size_t size;
    size_t capacity;
} list_t;

#define x(n) \
    n(ERR_PTR_NULL                      ,   0) \
    n(ERR_CMD_INVALID                   ,   1) \
    n(ERR_INVALID_INDEX                 ,   2) \
    n(ERR_OUT_OF_BOUNDS                 ,   3) \
    n(ERR_NOTHING_TO_POP                ,   4) \
    n(ERR_INVALID_OPERATION             ,   5) \
    n(ERR_REALLOC_FAIL                  ,   6) \
    n(ERR_INCORRECT_LIST                ,   7) \
    n(ERR_CYCLING_LIST                  ,   8) \
    n(ERR_INVALID_ARGUMENTS             ,   9) \
    n(ERR_INVALID_PREV                  ,   10) \
    n(ERR_INVALID_NEXT                  ,   11) \
    n(ERR_INVALID_SIZE                  ,   12) \
    n(ERR_INVALID_RELATION_WITH_PREV    ,   13) \
    n(ERR_INVALID_RELATION_WITH_NEXT    ,   14)
#define init_enum(val, ind) \
    val = ind,
#define init_str(val, ...) \
    #val, 

enum lstErrors {
    x(init_enum)
};
string error_text[] = {x(init_str)};

enum operations {
    ADD_AFTER       = 0,
    START           = 1,
    REMOVE          = 2,
    ADD_BEFORE      = 3,
    UNDEF_OPERATION = 4
};

void listDtor(list_t *list);
void listCtor(list_t *list);
void remove(list_t *list, int index);
void add_after(list_t *list, int index, int value);
void add_before(list_t *list, int index, int value);
void print_order_of_data(FILE * fp, list_t *list);
#endif // LIST_H