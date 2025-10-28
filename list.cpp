#include <stdio.h>
#include <string.h>

#include "sassert.h"
#include "error_manage.h"
#include "better_output.h"
#include "sys/stat.h"


const  size_t START_LIST_SIZE   = 10;
static size_t count             = 0;
const  size_t MAX_STR_SIZE      = 200;
const  int    POISON            = -1;
const  size_t INITIAL_HEAD_VAL  = 1;
const  size_t INITIAL_TAIL_VAL  = 1;
const  size_t INITIAL_FREE_VAL  = 1;
const char * const dump_graph_file_name = "graph.txt";
const char * const dump_site_file_name  = "dump.html";
const char * const operations_descriptions[]    = {"after adding to a list ", "before operation happened ", "after removal from list "};
const char * const operations_classes[]         = {"add_text", "start_text", "remove_text"};
const char * const error_text[]                 = {"your pointer is null", "command system is invalid", "incorrect index for a list", "oops your array is out of bounds", "there is nothing to pop in array"};

typedef struct {
    int *data;
    int *next;
    int *prev;
    int tail;
    int head;
    int free;
    size_t size;
    size_t capacity;
} list_t;

enum errors {
    ERR_PTR_NULL            = 0,
    ERR_CMD_INVALID         = 1,
    ERR_INVALID_INDEX       = 2,
    ERR_OUT_OF_BOUNDS       = 3,
    ERR_NOTHING_TO_POP      = 4
};

enum operations {
    ADD     = 0,
    START   = 1,
    REMOVE  = 2
};

void initialize_with_poison(int * array) {
    for (size_t i = 0; i < START_LIST_SIZE; i++) {
        array[i] = POISON;
    }
}

error_t listCtor_internal(list_t *list) {
    sassert(list, ERR_PTR_NULL);

    list->tail = INITIAL_TAIL_VAL;
    list->head = INITIAL_HEAD_VAL;
    list->free = INITIAL_FREE_VAL;
    list->size = 0;
    list->capacity = START_LIST_SIZE;
    list->data = (int *) calloc(START_LIST_SIZE, sizeof(int));
    sassert(list->data, ERR_PTR_NULL);
    initialize_with_poison(list->data);

    list->next = (int *) calloc(START_LIST_SIZE, sizeof(int));
    sassert(list->next, ERR_PTR_NULL);
    initialize_with_poison(list->next);

    list->prev = (int *) calloc(START_LIST_SIZE, sizeof(int));
    sassert(list->prev, ERR_PTR_NULL);
    initialize_with_poison(list->prev);

    return error;
}

void print_site_headers() {
    FILE * fp = fopen(dump_site_file_name, "w");
    sassert(fp, ERR_PTR_NULL);

    fprintf(fp, "<!DOCTYPE html>\n"
                "<html lang=\"ru\">\n"
                "<head>\n"
                "<style>\n"
                ".add_text, .remove_text {\n"
                "color: #00ff9dc5;\n"
                "}\n"
                ".start_text {\n"
                "color: #b2ec06ff;\n"
                "}\n"
                "h3 {\n"
                "color: #a30f7eff;\n"
                "font-weight: bold;\n"
                "}\n"
                "body {\n"
                "background-color: black;\n"
                "}\n"
                "</style>\n"
                "<title>my list dump</title>\n"
                "</head>\n"
                "<body width=\"device-width\">\n"
            );
    fclose(fp);
}

#define listCtor(list_name) \
    list_t *list_name = (list_t *) calloc(1, sizeof(list_t));\
    listCtor_internal(list_name);\
    print_site_headers();

#define add_element(list, index, value) \
    print_to_html(list, START, index, value);\
    create_dot_image_dump(list);\
\
    add_element_internal(list, index, value);\
    print_to_html(list, ADD, index, value);\
\
    create_dot_image_dump(list);\


void print_info(list_t *list) {
     fprintf(stderr, "\ndata:     ");
    for (size_t i = 0; i < list->capacity; i++) {
        fprintf(stderr, "[%-4d]", list->data[i]);
    }
    fprintf(stderr, "\nnext:     ");
    for (size_t i = 0; i < list->capacity; i++) {
        fprintf(stderr, "[%-4d]", list->next[i]);
    }
    fprintf(stderr, "\nprev:     ");
    for (size_t i = 0; i < list->capacity; i++) {
        fprintf(stderr, "[%-4d]", list->prev[i]);
    }
    fprintf(stderr, "\nin order: ");
    int current = list->tail;
    while(current != POISON) {
        fprintf(stderr, "[%-4d]", list->data[current]);
        current = list->next[current];
    }
    fprintf(stderr, "\nhead: %d\ntail: %d\nfree: %d\nsize: %d\n", list->head, list->tail, list->free, list->size);
}

error_t add_element_internal(list_t * list, size_t index, int value) {
    sassert(list, ERR_PTR_NULL);
    if (index > list->size + 1) {
        add_error(ERR_INVALID_INDEX, " index=%d size=%d", index, list->size);
        return error;
    }

    BEGIN
    list->data[list->free] = value;

    if (list->size == 0) {
        break;
    }
    if (index == list->head) {
        list->next[list->head] = list->free;
        list->prev[list->free] = list->head;
        list->head = list->free;
        break;
    }

    list->next[list->free]          = list->next[index];
    list->prev[list->next[index]]   = list->free;
    list->next[index]               = list->free;
    list->prev[list->free]          = index;

    END
    list->size++;
    list->free++;
    return error;
}

#define remove_element(list, index) \
    print_to_html(list, START, index, POISON);\
    create_dot_image_dump(list);\
\
    remove_element_internal(list, index);\
    print_to_html(list, REMOVE, index, POISON);\
\
    create_dot_image_dump(list);\

error_t remove_element_internal(list_t *list, int index) {
    sassert(list, ERR_PTR_NULL);
    
    if (index < 0 || index >= list->capacity || list->data[index] == POISON) {
        add_error(ERR_OUT_OF_BOUNDS, "index:%d", index);
        return error;
    }

    if (list->size == 0) {
        add_error(ERR_NOTHING_TO_POP, "size = 0");
        return error;
    }

    if (list->size == 1) {
        list->head = INITIAL_HEAD_VAL;
        list->tail = INITIAL_TAIL_VAL;
    }
    else if (index == list->head) {
        list->head = list->prev[list->head];
        list->next[list->prev[index]] = POISON;
    }
    else if (index == list->tail) {
        list->tail = list->next[index];
    }
    else {
        list->next[list->prev[index]] = list->next[index];
    }

    list->data[index] = POISON;
    list->next[index] = POISON;
    list->prev[index] = POISON;
    list->size--;
    return error;
}

void print_site_toes() {
    FILE * fp = fopen(dump_site_file_name, "a");\
    sassert(fp, ERR_PTR_NULL);\

    fprintf(fp, "<p style=\"color: #a30f7eff\">||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||</p>\n");
    fprintf(fp, "</body>\n"
                "</html");
    fclose(fp);
    
}

#define listDtor(list) \
    listDtor_internal(list);\
    print_site_toes();


error_t listDtor_internal(list_t *list) {
    if (list == NULL)
        return error;
    
    if (list->data != NULL)
        free(list->data);
    if (list->prev != NULL)
        free(list->prev);
    if (list->next != NULL)
        free(list->next);
    free(list);
    return error;
}

error_t create_dot_image_dump(list_t *list) {
    FILE * fp = fopen(dump_graph_file_name, "w");
    sassert(fp, ERR_PTR_NULL);

    fprintf(fp, "\ndigraph {\nrankdir=LR\nbgcolor=\"#000000\"\n"
                "ranksep=0.0\nsplines=ortho\n");
    for (size_t i = 0; i < list->capacity; i++) {
        if (list->data[i] == POISON)
            fprintf(fp, "data_array_info%zu[shape=record,color=\"#49994aff\", fontcolor=\"#49994aff\", label=<index in array: %zu | value: <FONT COLOR=\"gray\">PSN</FONT> | {prev: %d | next: %d}>]\n", i, i, -1, -1);
        else
            fprintf(fp, "data_array_info%zu[shape=record,color=\"#49994aff\", fontcolor=\"#49994aff\", label=<index in array: %zu | value: <FONT COLOR=\"red\">%d</FONT>   | {prev: %d | next: %d}>]\n", i, i, list->data[i], list->prev[i], list->next[i]);
        }
    for (size_t i = 0; i < list->capacity - 1; i++) {
        fprintf(fp, "data_array_info%zu->", i);
    }
    fprintf(fp, "data_array_info%zu [style=\"invis\", weight=500, minlen=4]", list->capacity - 1);

    int current = list->tail;
    while (current != POISON) {
        int end = list->next[current];
        if (end != POISON)
            fprintf(fp, "data_array_info%zu->data_array_info%zu [weight=1,color=\"#96ff99ff\", minlen=4]\n", current, end);
        current = end;
    }
    fprintf(fp, "}");
    fclose(fp);
    char command[MAX_STR_SIZE] = {};
    snprintf(command, MAX_STR_SIZE - 1, "dot graph.txt -Gdpi=80 -Tpng -o graph%zu.png", count++);
    if (system(command) != 0) {
        add_error(ERR_CMD_INVALID, "%s", command);
    }
    return error;
}

void print_to_html(list_t *list, operations operation, size_t index, int value) {
    sassert(list, ERR_PTR_NULL);

    FILE * fp = fopen(dump_site_file_name, "a");
    sassert(fp, ERR_PTR_NULL);

    if (operation == START)
        fprintf(fp, "<p style=\"color: #a30f7eff\">||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||</p>\n");

    if (value != POISON)
        fprintf(fp, "<h3>%s at index: %zu, value: %d</h3>\n", operations_descriptions[operation], index, value);
    else 
        fprintf(fp, "<h3>%s at index: %zu, value: NONE</h3>\n", operations_descriptions[operation], index, value);
    fprintf(fp, "<pre class = \"%s\">\n", operations_classes[operation]);
    fprintf(fp, "\ndata:     ");
    for (size_t i = 0; i < list->capacity; i++) {
        fprintf(fp, "[%-4d]", list->data[i]);
    }
    fprintf(fp, "\nnext:     ");
    for (size_t i = 0; i < list->capacity; i++) {
        fprintf(fp, "[%-4d]", list->next[i]);
    }
    fprintf(fp, "\nprev:     ");
    for (size_t i = 0; i < list->capacity; i++) {
        fprintf(fp, "[%-4d]", list->prev[i]);
    }
    fprintf(fp, "\nin order: ");
    int current = list->tail;
    while(current != POISON) {
        fprintf(fp, "[%-4d]", list->data[current]);
        current = list->next[current];
    }
    fprintf(fp, "\nhead: %d\ntail: %d\nfree: %d\nsize: %d\n", list->head, list->tail, list->free, list->size);
    fprintf(fp, "<img src = \"graph%zu.png\">\n</pre>\n", count);

    fclose(fp);
}

void print_floor(FILE *fp) {
    fprintf(fp, "}");
}

int main(void) {
    listCtor(list);

    add_element(list, 1, 10);
    add_element(list, 1, 20);
    add_element(list, 2, 30);
    add_element(list, 1, 15);
    add_element(list, 3, 70);
    add_element(list, 4, 71);
    add_element(list, 2, 50);
    remove_element(list, 4);
    listDtor(list);

    if (error.is_error) {
        print_error(error, error_text);
        return error.code;
    }

    return 0;
}