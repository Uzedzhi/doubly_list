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

LstErrors ListRealloc(list_t *list) {
    sassert(list, _LST_ERR_PTR_NULL);

    int old_capacity = list->capacity;
    list->capacity *= 2;

    if(reallocate_array((void **) (&(list->data)), old_capacity, list->capacity * sizeof(list_el_t))  == NULL |
       reallocate_array((void **) (&(list->next)), old_capacity, list->capacity * sizeof(int))        == NULL |
       reallocate_array((void **) (&(list->prev)), old_capacity, list->capacity * sizeof(int))        == NULL |
       reallocate_array((void **) (&(list->free)), old_capacity, list->capacity * sizeof(int))        == NULL)
        return _LST_ERR_REALLOC_FAIL;

    
    for (size_t i = old_capacity; i < list->capacity; i++) {
        list->free[i] = i + 1;
        list->next[i] = POISON;
        list->prev[i] = POISON;
        list->data[i] = DATA_POISON;
    }

    return _LST_OK;
}

LstErrors add_element_after_internal(list_t *list, size_t index, list_el_t value) {
    sassert(list, _LST_ERR_PTR_NULL);

    int capacity = list->capacity;
    if (list->size >= capacity - 1) {
        _LST_RETURN_ERR(ListRealloc(list) == _LST_OK, _LST_ERR_REALLOC_FAIL,
                        "не удалось увеличить размер списка, capacity: %d", capacity);
    }

    _LST_RETURN_ERR(index < (size_t) capacity, _LST_ERR_INVALID_INDEX,
                    "попытка вставки после несуществующего индекса %zu (capacity=%d)", index, capacity);

    _LST_RETURN_ERR(index == 0 || list->data[index] != DATA_POISON, _LST_ERR_INVALID_INDEX,
                    "попытка вставки после удалённого узла %zu", index);

    int free = list->last_free;
    list->last_free = list->free[free];

    list->data[free] = value;
    list->prev[free] = (int) index;
    list->next[free] = list->next[index];

    list->prev[list->next[index]] = free;
    list->next[index] = free;

    list->size++;
    return _LST_OK;
}

LstErrors add_element_before_internal(list_t *list, size_t index, list_el_t value) {
    sassert(list, _LST_ERR_PTR_NULL);

    int capacity = list->capacity;
    if (list->size >= capacity - 1) {
        _LST_RETURN_ERR(ListRealloc(list), _LST_ERR_REALLOC_FAIL,
                        "не удалось увеличить размер списка");
    }

    _LST_RETURN_ERR(index < (size_t) capacity, _LST_ERR_INVALID_INDEX,
                    "попытка вставки перед несуществующим индексом %zu (capacity=%d)", index, capacity);

    _LST_RETURN_ERR(index == 0 || list->data[index] != DATA_POISON, _LST_ERR_INVALID_INDEX,
                    "попытка вставки перед удалённым узлом %zu", index);

    int free = list->last_free;
    list->last_free = list->free[free];

    list->data[free] = value;
    list->prev[free] = list->prev[index];
    list->next[free] = (int) index;

    list->next[list->prev[index]] = free;
    list->prev[index] = free;

    list->size++;
    return _LST_OK;
}

LstErrors remove_element_internal(list_t *list, size_t index) {
    sassert(list, _LST_ERR_PTR_NULL);

    int size     = list->size;
    int capacity = list->capacity;

    // index == 0 удалять нельзя - это служебный узел кольца
    _LST_RETURN_ERR(index > 0 && index < (size_t) capacity, _LST_ERR_INVALID_INDEX,
                    "попытка удалить служебный узел или неверный индекс %zu, capacity=%d", index, capacity);

    _LST_RETURN_ERR(list->data[index] != DATA_POISON, _LST_ERR_INVALID_INDEX,
                    "попытка удалить уже удалённый узел %zu (size=%d)", index, size);

    int prev_node = list->prev[index];
    int next_node = list->next[index];

    list->next[prev_node] = next_node;
    list->prev[next_node] = prev_node;

    #ifdef STRING_TYPE
        SMART_FREE(list->data[index]);
    #endif
    list->data[index] = DATA_POISON;
    list->next[index] = POISON;
    list->prev[index] = POISON;

    list->free[index] = list->last_free;
    list->last_free = index;

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

    int current = list->next[0];
    for (size_t i = 0; i < list->size; i++) {
        if (current > list->capacity || current == POISON)
            break;
        fprintf(fp, "[%-4" DATA_SPEC "]", list->data[current]);
        current = list->next[current];
    }
}

void listCtor_internal(list_t *list) {
    sassert(list, _LST_ERR_PTR_NULL);

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

    list->next[0]   = INITIAL_TAIL_VAL;
    list->prev[0]   = INITIAL_HEAD_VAL;
    list->last_free = INITIAL_FREE_VAL;

    list->free[0] = POISON;
    for (size_t i = 1; i < START_LIST_SIZE; i++) {
        list->free[i] = i + 1;
    }

    return;
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
        if (print_to_html(list, START, list->prev[0], (list_el_t) DATA_POISON) != _LST_OK)
            lst_perror();
    #endif

    LstErrors result = add_element_after_internal(list, list->prev[0], data);

    #ifdef DEBUG
        if (create_dot_image_dump(list) != _LST_OK)
            lst_perror();
        if (print_to_html(list, ADD_AFTER, list->prev[0], data) != _LST_OK)
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