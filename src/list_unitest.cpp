#include <stdio.h>
#include <string.h>

#include "../MyLibs/sassert.hpp"
#include "../MyLibs/better_output.hpp"
#include "../includes/list_values.h"
#include "../includes/list.h"

operations get_operation(char * operation) {
    for (size_t i = 0; i < sizeof(operations_classes) / sizeof(char *); i++) {
        if (strcmp(operations_classes[i], operation) == 0) {
            return (operations) i;
        }
    }
    return UNDEF_OPERATION;
}

void skip_line(FILE * fp) {
    sassert(fp, _LST_ERR_PTR_NULL);

    int ch = 0;
    while ((ch = getc(fp)) != '\n' && ch != '\0' && ch != EOF)
        ;
    getc(fp);
}

bool check_order(list_t *list, FILE *fp) {
    sassert(list,   _LST_ERR_PTR_NULL);
    sassert(fp,     _LST_ERR_PTR_NULL);

    list_el_t value = 0;
    int count = 0;
    int current = list->tail;
    #ifdef STRING_TYPE
        while ((fscanf(fp, "%s ", value) == 1)) {
    #else
        while ((fscanf(fp, "%d ", &value) == 1)) {
    #endif
        if (count > list->size || list->data[current] != value) {
            return false;
        }
        current = list->next[current];
        count++;
    }

    if (count != list->size)
        return false;
    return true;
}

void reinitialize_list(list_t *list) {
    sassert(list, _LST_ERR_PTR_NULL);

    list->tail = INITIAL_TAIL_VAL;
    list->head = INITIAL_HEAD_VAL;

    list->last_free = 0;
    for (size_t i = START_LIST_SIZE; i > 0; i--) {
        list->free[i] = i + 1;
    }

    list->size = 0;
    list->capacity = START_LIST_SIZE;
    initialize_with_poison(list->data, (list_el_t) DATA_POISON);
    initialize_with_poison(list->next, POISON);
    initialize_with_poison(list->prev, POISON);
}

int unitest_list(const char *unitest_file_name) {
    int exp_order[MAX_TESTING_SIZE] = {};
    
    FILE * fp = fopen(unitest_file_name, "r+");
    sassert(fp, _LST_ERR_PTR_NULL);

    char command_line[MAX_STR_SIZE] = {};

    list_t list = {};
    listCtor(&list);
    int count = 0;
    while (fgets(command_line, MAX_STR_SIZE, fp) != NULL) {
        char operation_str[MAX_STR_SIZE] = {};
        int index = 0;
        list_el_t value = 0;
        
    #ifdef STRING_TYPE
        if (sscanf(command_line, "%s %d %s", operation_str, &index, value) >= 2) {
    #else
        if (sscanf(command_line, "%s %d %d", operation_str, &index, &value) >= 2) {
    #endif
            operations operation = get_operation(operation_str);
            switch(operation) {
                case ADD_AFTER:
                    add_after(&list, index, value);
                    break;
                case ADD_BEFORE:
                    add_before(&list, index, value);
                    break;
                case REMOVE:
                    remove(&list, index);
                    break;
                case UNDEF_OPERATION:
                    break;
                case START:
                default:
                    sassert(0, _LST_ERR_INVALID_OPERATION);
            }
        }
        else {
            bool is_right = check_order(&list, fp);
            if (is_right)
                printf(GREEN "test %d was done correctly\n" WHITE, count);
            else {
                printf(RED "test %d failed miserably", count);
                char line_with_results[MAX_STR_SIZE] = {};
                fgets(line_with_results, MAX_STR_SIZE - 1, fp);

                printf("expected: ");
                printf("%s", line_with_results);
                printf("\ngot:");
                print_order_of_data(stdout, &list);
                printf("\n");
            }
            reinitialize_list(&list);
            count++;
        }
    }
    printf(WHITE);
    listDtor(&list);
    return true;
}