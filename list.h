#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdio.h>
#include "my_libs/error_manage.hpp"

typedef const char * const string;
#ifdef STRING_TYPE
    typedef char * list_el_t;
    typedef string list_el_const_t;
#else
    typedef int list_el_t;
    typedef int list_el_const_t;
#endif // STRING_TYPE

typedef struct {
    list_el_t *data;
    int *next;
    int *prev;
    int *free;

    int tail;
    int head;
    int last_free;

    size_t size;
    size_t capacity;
} list_t;

void listDtor(list_t *list);
void listCtor(list_t *list);
void remove(list_t *list, int index);
void add_after(list_t *list, int index,  list_el_const_t value);
void add_before(list_t *list, int index, list_el_const_t value);
void print_order_of_data(FILE * fp, list_t *list);
#endif // LIST_H