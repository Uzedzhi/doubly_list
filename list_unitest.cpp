#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "sassert.h"
#include "error_manage.h"
#include "better_output.h"
#include "sys/stat.h"
#include "../stack/stack.h"
#include "list.h"
#include "list_dump.h"

operations get_operation(char * operation) {
    for (size_t i = 0; i < sizeof(operations_classes) / sizeof(char *); i++) {
        if (strcmp(operations_classes[i], operation) == 0) {
            return (operations) i;
        }
    }
    return UNDEF_OPERATION;
}

void skip_line(FILE * fp) {
    sassert(fp, ERR_PTR_NULL);

    int ch = 0;
    while ((ch = getc(fp)) != '\n' && ch != '\0' && ch != EOF) {
        ;
    }
    getc(fp);
}

bool check_order(list_t *list, FILE *fp) {
    sassert(list,   ERR_PTR_NULL);
    sassert(fp,     ERR_PTR_NULL);

    int value = 0;
    int count = 0;
    int current = list->tail;
    while ((fscanf(fp, "%d ", &value) == 1)) {
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
    sassert(list, ERR_PTR_NULL);

    list->tail = INITIAL_TAIL_VAL;
    list->head = INITIAL_HEAD_VAL;

    stackDtor(list->free);

    init_stack(free, START_LIST_SIZE);
    list->free = free;
    for (size_t i = START_LIST_SIZE; i > 0; i--) {
        stackPush(list->free, i);
    }

    list->size = 0;
    list->capacity = START_LIST_SIZE;
    initialize_with_poison(list->data);
    initialize_with_poison(list->next);
    initialize_with_poison(list->prev);
}

int unitest_list() {
    int exp_order[MAX_TESTING_SIZE] = {};
    
    FILE * fp = fopen(unitest_file_name, "r+");
    sassert(fp, ERR_PTR_NULL);

    char command_line[MAX_STR_SIZE] = {};
    listCtor(list);
    int count = 0;
    while (fgets(command_line, MAX_STR_SIZE, fp) != NULL) {
        char operation_str[MAX_STR_SIZE] = {};
        int index = 0;
        int value = 0;
        if (sscanf(command_line, "%s %d %d", operation_str, &index, &value) >= 2) {
            operations operation = get_operation(operation_str);
            switch(operation) {
                case ADD_AFTER:
                    add_element_after(list, index, value);
                    break;
                case ADD_BEFORE:
                    add_element_before(list, index, value);
                    break;
                case REMOVE:
                    remove_element(list, index);
                    break;
                case UNDEF_OPERATION:
                    break;
                case START:
                default:
                    sassert(0, ERR_INVALID_OPERATION);
            }
        }
        else {
            bool is_right = check_order(list, fp);
            if (is_right)
                printf(GREEN "test %d was done correctly\n" WHITE, count);
            else {
                printf(RED "test %d failed miserably", count);
                char line_with_results[MAX_STR_SIZE] = {};
                fgets(line_with_results, MAX_STR_SIZE - 1, fp);

                printf("expected: ");
                printf("%s", line_with_results);
                printf("\ngot:");
                print_order_of_data(stdout, list);
                printf("\n");
            }
            reinitialize_list(list);
            count++;
        }
    }
    printf(WHITE);
    listDtor(list);
    return true;
}