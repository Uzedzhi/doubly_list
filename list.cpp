#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define ERROR_ADD_DEBUG
#include "sassert.h"
#include "error_manage.h"
#include "better_output.h"
#include "sys/stat.h"
#include "../stack/stack.h"
#include "list_dump.h"
#include "list_unitest.h"
#include "list.h"

void initialize_with_poison(int * array) {
    for (size_t i = 0; i < START_LIST_SIZE; i++) {
        array[i] = POISON;
    }
}

void print_order_of_data(FILE * fp, list_t *list) {
    sassert(fp, ERR_PTR_NULL);

    int current = list->tail;
    for (size_t i = 0; i < list->size; i++) {
        fprintf(fp, "[%-4d] ", list->data[current]);
        current = list->next[current];
    }
}

error_t listCtor_internal(list_t *list) {
    sassert(list, ERR_PTR_NULL);

    list->tail = INITIAL_TAIL_VAL;
    list->head = INITIAL_HEAD_VAL;

    init_stack(free, START_LIST_SIZE);
    list->free = free;
    for (size_t i = START_LIST_SIZE; i > 0; i--) {
        stackPush(list->free, i);
    }

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

error_t add_element_before_internal(list_t *list, size_t index, int value) {
    sassert(list, ERR_PTR_NULL);

    if (index > list->size || index < 1) {
        add_error(ERR_INVALID_INDEX, " index=%d size=%d", index, list->size);
        return error;
    }
    if (list->size == list->capacity) {
        safe_realloc((void **) &list->data, list->capacity * 2);
        safe_realloc((void **) &list->next, list->capacity * 2);
        safe_realloc((void **) &list->prev, list->capacity * 2);
        list->capacity *= 2;
    }
    int free = 0;
    stackPop(list->free, (stack_var_t *) &free);

    list->data[free] = value;

    if (list->size++ == 0) {
        return error;
    }

    if (index != list->tail) {
        list->prev[free]                = list->prev[index];
        list->next[list->prev[index]]   = free;
    }
    else
        list->tail = free;

    list->prev[index] = free;

    if (index != list->head + 1)
        list->next[free] = index;

    return error;
}

error_t add_in_head(list_t *list, int value) {
    sassert(list, ERR_PTR_NULL);

    add_element_after_internal(list, list->head, value);
    return error;
}

error_t add_element_after_internal(list_t * list, size_t index, int value) {
    sassert(list, ERR_PTR_NULL);

    verify_list(list);
    if (error.is_error == true)
        return error;
    
    if (index == 0)
        index = list->head;
    
    if (index > list->size + 1 || index < 0) {
        add_error(ERR_INVALID_INDEX, " index=%d size=%d", index, list->size);
        return error;
    }

    if (list->size == list->capacity) {
        safe_realloc((void **) &list->data, list->capacity * 2);
        safe_realloc((void **) &list->next, list->capacity * 2);
        safe_realloc((void **) &list->prev, list->capacity * 2);
        list->capacity *= 2;
    }


    int free = 0;
    stackPop(list->free, &free);

    list->data[free] = value;
    if (list->size++ == 0) {
        return error;
    }

    if (index == list->head) {
        list->next[list->head] = free;
        list->prev[free] = list->head;
        list->head = free;
        return error;
    }

    list->next[free]                = list->next[index];
    list->prev[list->next[index]]   = free;
    list->next[index]               = free;
    list->prev[free]                = index;
    return error;
}

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

    stackPush(list->free, index);

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

error_t listDtor_internal(list_t *list) {
    if (list == NULL)
        return error;
    
    if (list->data != NULL)
        free(list->data);
    if (list->prev != NULL)
        free(list->prev);
    if (list->next != NULL)
        free(list->next);
    if (list->free != NULL)
        stackDtor(list->free);
    free(list);
    return error;
}

int main(void) {
    listCtor(list);
    add_element_after(list, 0, 10);
    add_element_after(list, 1, 11);
    add_element_after(list, 2, 12);
    add_element_after(list, 3, 13);
    add_element_after(list, 4, 14);
    add_element_after(list, 0, 20);
    add_element_before(list, 1, 100);

    listDtor(list);
    // open_live_server(dump_site_file_name);
    return 0;
}