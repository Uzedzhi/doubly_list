#include <stdio.h>
#include <string.h>

#include "my_libs/sassert.hpp"
#include "my_libs/error_manage.hpp"
#include "list_dump.h"
#include "list.h"

#define SAFE_FREE(el) \
    if (el)\
        free(el);

error_t error = {};

void * safe_realloc(void ** memory, size_t new_size) {
    sassert(memory, ERR_PTR_NULL);

    void *temp = realloc(*memory, new_size);
    if (temp == NULL) {
        add_error(ERR_REALLOC_FAIL, "couldn't enlarge your array");
        return NULL;
    }
    *memory = temp;
    return *memory;
}

void add_element_after_internal(list_t * list, size_t index, list_el_t value) {
    sassert(list, ERR_PTR_NULL);
    
    if (index > list->size)
        return;

    if (list->size == list->capacity) {
        list->capacity *= 2;
        safe_realloc((void **) &list->data, list->capacity * sizeof(list_el_t));
        safe_realloc((void **) &list->next, list->capacity * sizeof(list_el_t));
        safe_realloc((void **) &list->prev, list->capacity * sizeof(list_el_t));
    }

    int free = list->last_free;
    list->last_free     = list->free[free];
    list->data[free]    = value;

    int next = list->next[index];
    list->next[free]    = next;
    list->prev[next]    = free;
    list->next[index]   = free;

    if (list->size)
        list->prev[free] = index;
    if (index == list->head)
        list->head = free;
        
    list->size++;
    return;
}

void remove_element_internal(list_t *list, int index) {
    sassert(list, ERR_PTR_NULL);
    
    if (list->size == 0 || index < 0 || index >= list->capacity || list->data[index] == (list_el_t) DATA_POISON)
        return;

    list->free[index] = list->last_free;
    list->last_free   = index;

    int prev = list->prev[index];
    int next = list->next[index];
    list->next[prev] = list->next[index];
    list->prev[next] = prev;

    #ifdef STRING_TYPE
        SAFE_FREE(list->data[index]);
    #endif
    list->data[index] = (list_el_t) DATA_POISON;
    list->next[index] = POISON;
    list->prev[index] = POISON;
    list->size--;
    return;
}

void listDtor_internal(list_t *list) {
    if (list == NULL)
        return;
    
    #ifdef STRING_TYPE
        for (int i = 0; i < list->capacity; i++) {
            SAFE_FREE(list->data[i]);
        }
        SAFE_FREE(list->data);
    #else
        SAFE_FREE(list->data);
    #endif

    SAFE_FREE(list->prev);
    SAFE_FREE(list->next);
    SAFE_FREE(list->free);
    return;
}

void print_order_of_data(FILE * fp, list_t *list) {
    sassert(fp, ERR_PTR_NULL);

    int current = list->tail;
    for (size_t i = 0; i < list->size; i++) {
        fprintf(fp, "[%-4" DATA_SPEC "]", list->data[current]);
        current = list->next[current];
    }
}

void listCtor_internal(list_t *list) {
    sassert(list, ERR_PTR_NULL);

    list->tail = INITIAL_TAIL_VAL;
    list->head = INITIAL_HEAD_VAL;

    list->size = 0;
    list->capacity = START_LIST_SIZE;


    list->data = (list_el_t *) calloc(START_LIST_SIZE, sizeof(list_el_t));
    sassert(list->data, ERR_PTR_NULL);
    initialize_with_poison(list->data, (list_el_t) DATA_POISON);


    list->next = (int *) calloc(START_LIST_SIZE, sizeof(int));
    sassert(list->next, ERR_PTR_NULL);
    initialize_with_poison(list->next, POISON);
    list->next[0] = 1;


    list->prev = (int *) calloc(START_LIST_SIZE, sizeof(int));
    sassert(list->prev, ERR_PTR_NULL);
    initialize_with_poison(list->prev, POISON);


    list->free = (int *) calloc(START_LIST_SIZE, sizeof(int));
    sassert(list->free, ERR_PTR_NULL);
    for (size_t i = 0; i < START_LIST_SIZE - 1; i++) {
        list->free[i] = i + 1;
    }
    list->free[START_LIST_SIZE - 1] = POISON;
    list->last_free = INITIAL_FREE_VAL;


    return;
}

void add_element_before_internal(list_t *list, size_t index, list_el_t value) {
    sassert(list, ERR_PTR_NULL);

    if (index > list->size) {
        return;
    }

    if (list->size == list->capacity) {
        safe_realloc((void **) &list->data, list->capacity * 2);
        safe_realloc((void **) &list->next, list->capacity * 2);
        safe_realloc((void **) &list->prev, list->capacity * 2);
        list->capacity *= 2;
    }

    int free            = list->last_free;
    list->last_free     = list->free[list->last_free];
    list->data[free]    = value;

    list->prev[free]    = list->prev[index];
    list->next[free]    = index;

    if (index != list->tail) 
        list->next[list->prev[index]] = free;
    else
        list->tail = free;

    list->prev[index]   = free;
    list->size++;
    return;
}

void listCtor(list_t *list) {
    listCtor_internal(list);\

    #ifndef NDEBUG
        print_site_headers();
    #endif
}

void listDtor(list_t *list) {
    listDtor_internal(list);\

    #ifndef NDEBUG
        print_site_toes();
    #endif
}

void add_before(list_t *list, int index, list_el_const_t value) {
    sassert(list, ERR_PTR_NULL);

    #ifdef STRING_TYPE
        list_el_t data = strdup(value);
    #else
        list_el_t data = value;
    #endif

    #ifndef NDEBUG
        create_dot_image_dump(list);
        print_to_html(list, START, index, data);
    #endif

    add_element_before_internal(list, index, data);    

    #ifndef NDEBUG
        create_dot_image_dump(list);
        print_to_html(list, ADD_BEFORE, index, data);
    #endif
}

void add_after(list_t *list, int index, list_el_const_t value) {
    sassert(list, ERR_PTR_NULL);

    #ifdef STRING_TYPE
        list_el_t data = strdup(value);
    #else
        list_el_t data = value;
    #endif

    #ifndef NDEBUG
        create_dot_image_dump(list);
        print_to_html(list, START, index, data);
    #endif

    add_element_after_internal(list, index, data);    

    #ifndef NDEBUG
        create_dot_image_dump(list);
        print_to_html(list, ADD_AFTER, index, data);
    #endif
}

void append(list_t *list, list_el_const_t value) {
    sassert(list, ERR_PTR_NULL);

    #ifdef STRING_TYPE
        list_el_t data = strdup(value);
    #else
        list_el_t data = value;
    #endif

    add_element_after_internal(list, list->head, data);
    return;
}

void remove(list_t *list, int index) {
    sassert(list, ERR_PTR_NULL);

    #ifndef NDEBUG
        create_dot_image_dump(list);
        print_to_html(list, START, index, (list_el_t) DATA_POISON);
    #endif

    remove_element_internal(list, index);    

    #ifndef NDEBUG
        create_dot_image_dump(list);
        print_to_html(list, REMOVE, index, (list_el_t) DATA_POISON);
    #endif
}

#undef SAFE_FREE