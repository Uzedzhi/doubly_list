#ifndef LIST_H
#define LIST_H

#include "error_manage.h"
#include "../stack/stack.h"

const  size_t START_LIST_SIZE   = 10;
static size_t count             = 0;
const  size_t MAX_STR_SIZE      = 200;
const  int    POISON            = -1;
const  size_t INITIAL_HEAD_VAL  = 1;
const  size_t INITIAL_TAIL_VAL  = 1;
const  size_t INITIAL_FREE_VAL  = 1;
const  size_t MAX_TESTING_SIZE  = 100;
string unitest_file_name    = "unitest.txt";
string dump_graph_file_name = "graph.txt";
string dump_site_file_name  = "dump.html";
string operations_descriptions[]    = {"added after index to a list ", "before operation happened ", "after removal from list ", "added before index to the list "};
string operations_classes[]         = {"add_after", "start", "remove", "add_before"};
typedef struct {
    int *data;
    int *next;
    int *prev;
    int tail;
    int head;
    stack_t * free;
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

#ifndef NDEBUG
#define listCtor(list_name) \
    list_t *list_name = (list_t *) calloc(1, sizeof(list_t));\
    listCtor_internal(list_name);\
    print_site_headers();
#define listDtor(list) \
    listDtor_internal(list);\
    print_site_toes();

#define add_element_after(list, index, value) {\
    create_dot_image_dump(list);\
    print_to_html(list, START, index, value);\
\
    add_element_after_internal(list, index, value);\
\
    create_dot_image_dump(list);\
    print_to_html(list, ADD_AFTER, index, value);\
\
    add_error_to_html(list, index, value);}
    
    

#define add_element_before(list, index, value) {\
    create_dot_image_dump(list);\
    print_to_html(list, START, index, value);\
\
    add_element_before_internal(list, index, value);\
\
    create_dot_image_dump(list);\
    print_to_html(list, ADD_BEFORE, index, value);\
\
    add_error_to_html(list, index, value);}


#define remove_element(list, index) {\
    print_to_html(list, START, index, POISON);\
    create_dot_image_dump(list);\
\
    remove_element_internal(list, index);\
    print_to_html(list, REMOVE, index, POISON);\
\
    create_dot_image_dump(list);\
    add_error_to_html(list, index, value);}

#else
#define listCtor(list_name) \
    list_t *list_name = (list_t *) calloc(1, sizeof(list_t));\
    listCtor_internal(list_name);
#define listDtor(list) \
    listDtor_internal(list);
#define remove_element(list, index) \
    remove_element_internal(list, index);

#define add_element_before(list, index, value) \
    add_element_before_internal(list, index, value);

#define add_element_after(list, index, value) \
    add_element_after_internal(list, index, value);
#endif 

void initialize_with_poison(int * array);
error_t listDtor_internal(list_t *list);
error_t remove_element_internal(list_t *list, int index);
error_t add_element_after_internal(list_t * list, size_t index, int value);
error_t add_element_before_internal(list_t *list, size_t index, int value);
void print_site_headers();
error_t listCtor_internal(list_t *list);
void print_order_of_data(FILE * fp, list_t *list);
#endif // LIST_H