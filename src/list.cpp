#include <cstdlib>
#include <stdio.h>
#include <string.h>

#include "../MyLibs/sassert.hpp"
#include "../MyLibs/helper_funcs.hpp"
#include "../includes/list_values.h"
#include "../includes/list_dump.h"
#include "../includes/list.h"

char lst_last_error[_LST_ERROR_BUF_MAX_SIZE] = {}; 
void lst_perror_file(FILE* out) {
    if (out)
        fprintf(out, RED "[ОШИБКА]: " RESET "%s\n", lst_last_error);
}
void lst_perror() {
    fprintf(stderr,  RED "[ОШИБКА]: " RESET "%s\n", lst_last_error);
}

LstErrors add_element_after_internal(list_t * list, size_t index, list_el_t value) {
    sassert(list, _LST_ERR_PTR_NULL);

    int size     = list->size;
    int capacity = list->capacity;
    _LST_RETURN_ERR(index <= size, _LST_ERR_INVALID_INDEX,
                    "вы пытаетесь вставить после индекса %zu, но размер %zu", index, size);
    _LST_RETURN_ERR(list->data[index] != DATA_POISON || !size, _LST_ERR_INVALID_INDEX,
                    "вы пытаетесь вставить после некорректного индекса, возможно он уже был удален?");

    if (size == capacity) {
        reallocate_array((void **) &list->data, capacity, capacity * 2 * sizeof(list_el_t));
        reallocate_array((void **) &list->next, capacity, capacity * 2 * sizeof(list_el_t));
        reallocate_array((void **) &list->prev, capacity, capacity * 2 * sizeof(list_el_t));
        list->capacity *= 2;
    }

    int free = list->last_free;
    list->last_free     = list->free[free];
    list->data[free]    = value;

    int next = list->next[index];
    list->next[free]    = next;
    list->prev[next]    = free;
    list->next[index]   = free;

    if (size)
        list->prev[free] = index;
    if (index == list->head)
        list->head = free;
        
    list->size++;
    return _LST_OK;
}

LstErrors remove_element_internal(list_t *list, size_t index) {
    sassert(list, _LST_ERR_PTR_NULL);
    
    size_t size     = list->size;
    size_t capacity = list->capacity;
    _LST_RETURN_ERR(index <= size && list->size != 0, _LST_ERR_INVALID_INDEX,
                    "вы пытаетесь вставить после индекса %zu, но размер %zu", index, size);
    _LST_RETURN_ERR(list->data[index] != DATA_POISON || !size, _LST_ERR_INVALID_INDEX,
                    "вы пытаетесь вставить после некорректного индекса %zu, возможно он уже был удален?", index);

    list->free[index] = list->last_free;
    list->last_free   = index;

    int prev = list->prev[index];
    int next = list->next[index];
    list->next[prev] = list->next[index];
    list->prev[next] = prev;

    #ifdef STRING_TYPE
        SMART_FREE(list->data[index]);
    #endif
    list->data[index] = (list_el_t) DATA_POISON;
    list->next[index] = POISON;
    list->prev[index] = POISON;
    list->size--;

    return _LST_OK;
}

void listDtor_internal(list_t *list) {
    if (list == NULL)
        return;
    
    #ifdef STRING_TYPE
        for (int i = 0; i < list->capacity; i++) {
            SMART_FREE(list->data[i]);
        }
        SMART_FREE(list->data);
    #else
        SMART_FREE(list->data);
    #endif

    SMART_FREE(list->prev);
    SMART_FREE(list->next);
    SMART_FREE(list->free);
    return;
}

void print_order_of_data(FILE * fp, list_t *list) {
    sassert(fp, _LST_ERR_PTR_NULL);

    int current = list->tail;
    for (size_t i = 0; i < list->size; i++) {
        if (current > list->capacity || current == POISON)
            break;
        fprintf(fp, "[%-4" DATA_SPEC "]", list->data[current]);
        current = list->next[current];
    }
}

void listCtor_internal(list_t *list) {
    sassert(list, _LST_ERR_PTR_NULL);

    list->tail = INITIAL_TAIL_VAL;
    list->head = INITIAL_HEAD_VAL;

    list->size = 0;
    list->capacity = START_LIST_SIZE;


    list->data = (list_el_t *) calloc(START_LIST_SIZE, sizeof(list_el_t));
    sassert(list->data, _LST_ERR_PTR_NULL);
    initialize_with_poison(list->data, (list_el_t) DATA_POISON);


    list->next = (int *) calloc(START_LIST_SIZE, sizeof(int));
    sassert(list->next, _LST_ERR_PTR_NULL);
    initialize_with_poison(list->next, POISON);


    list->prev = (int *) calloc(START_LIST_SIZE, sizeof(int));
    sassert(list->prev, _LST_ERR_PTR_NULL);
    initialize_with_poison(list->prev, POISON);


    list->free = (int *) calloc(START_LIST_SIZE, sizeof(int));
    sassert(list->free, _LST_ERR_PTR_NULL);
    for (size_t i = 0; i < START_LIST_SIZE - 1; i++) {
        list->free[i] = i + 1;
    }
    list->free[START_LIST_SIZE - 1] = POISON;
    list->last_free = INITIAL_FREE_VAL;

    return;
}

LstErrors add_element_before_internal(list_t *list, size_t index, list_el_t value) {
    sassert(list, _LST_ERR_PTR_NULL);

    size_t size     = list->size;
    size_t capacity = list->capacity;

    
    _LST_RETURN_ERR(index <= size, _LST_ERR_INVALID_INDEX,
                    "вы пытаетесь вставить перед индексом %zu, но размер %zu", index, size);
    _LST_RETURN_ERR(list->data[index] != DATA_POISON || !size, _LST_ERR_INVALID_INDEX,
                    "вы пытаетесь вставить после некорректного индекса %zu, возможно он уже был удален?", index);

    if (size == capacity) {
        reallocate_array((void **) &list->data, capacity, capacity * 2 * sizeof(list_el_t));
        reallocate_array((void **) &list->next, capacity, capacity * 2 * sizeof(list_el_t));
        reallocate_array((void **) &list->prev, capacity, capacity * 2 * sizeof(list_el_t));
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
    return _LST_OK;
}

void listCtor(list_t *list) {
    listCtor_internal(list);\

    #ifdef DEBUG
        print_site_headers();
    #endif
}

void listDtor(list_t *list) {
    listDtor_internal(list);\

    #ifdef DEBUG
        print_site_toes();
    #endif
}

LstErrors add_before(list_t *list, int index, list_el_const_t value) {
    sassert(list, _LST_ERR_PTR_NULL);

    #ifdef STRING_TYPE
        list_el_t data = strdup(value);
    #else
        list_el_t data = value;
    #endif

    #ifdef DEBUG
        if (create_dot_image_dump(list) != _LST_OK)
            lst_perror();
        if (print_to_html(list, START, index, data) != _LST_OK)
            lst_perror();
    #endif

    LstErrors result = add_element_before_internal(list, index, data); 

    #ifdef DEBUG
        if (create_dot_image_dump(list) != _LST_OK)
            lst_perror();
        if (print_to_html(list, ADD_BEFORE, index, data) != _LST_OK)
            lst_perror();
    #endif

    return result;
}

LstErrors add_after(list_t *list, int index, list_el_const_t value) {
    sassert(list, _LST_ERR_PTR_NULL);

    #ifdef STRING_TYPE
        list_el_t data = strdup(value);
    #else
        list_el_t data = value;
    #endif

    #ifdef DEBUG
        if (create_dot_image_dump(list) != _LST_OK)
            lst_perror();
        if (print_to_html(list, START, index, data) != _LST_OK)
            lst_perror();
    #endif

    LstErrors result = add_element_after_internal(list, index, data);    
    #ifdef DEBUG
        if (create_dot_image_dump(list) != _LST_OK)
            lst_perror();
        if (print_to_html(list, ADD_AFTER, index, data) != _LST_OK)
            lst_perror();
    #endif

    return result;
}

LstErrors append(list_t *list, list_el_const_t value) {
    sassert(list, _LST_ERR_PTR_NULL);

    #ifdef STRING_TYPE
        list_el_t data = strdup(value);
    #else
        list_el_t data = value;
    #endif

    #ifdef DEBUG
        if (create_dot_image_dump(list) != _LST_OK)
            lst_perror();
        if (print_to_html(list, START, list->head, (list_el_t) DATA_POISON) != _LST_OK)
            lst_perror();
    #endif

    LstErrors result = add_element_after_internal(list, list->head, data);

    #ifdef DEBUG
        if (create_dot_image_dump(list) != _LST_OK)
            lst_perror();
        if (print_to_html(list, ADD_AFTER, list->head, data) != _LST_OK)
            lst_perror();
    #endif

    return result;
}

LstErrors remove(list_t *list, int index) {
    sassert(list, _LST_ERR_PTR_NULL);

    #ifdef DEBUG
        if (create_dot_image_dump(list) != _LST_OK)
            lst_perror();
        if (print_to_html(list, START, index, (list_el_t) DATA_POISON) != _LST_OK)
            lst_perror();
    #endif

    LstErrors result = remove_element_internal(list, index);    

    #ifdef DEBUG
        if (create_dot_image_dump(list) != _LST_OK)
            lst_perror();
        if (print_to_html(list, REMOVE, index, (list_el_t) DATA_POISON) != _LST_OK)
            lst_perror();
    #endif

    return result;
}