#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../MyLibs/sassert.hpp"
#include "../MyLibs/helper_funcs.hpp"
#include "../includes/list_values.h"
#include "../includes/list.h"
#include "../includes/list_dump.h"

static int count_graphs = 0;
extern char lst_last_error[_LST_ERROR_BUF_MAX_SIZE];

void print_info(list_t *list) {
    fprintf(stderr, "\ndata:     ");
    for (size_t i = 0; i < list->capacity; i++) {
        fprintf(stderr, "[%-4" DATA_SPEC "]", list->data[i]);
    }
    fprintf(stderr, "\nnext:     ");
    for (size_t i = 0; i < list->capacity; i++) {
        fprintf(stderr, "[%-4d]", list->next[i]);
    }
    fprintf(stderr, "\nprev:     ");
    for (size_t i = 0; i < list->capacity; i++) {
        fprintf(stderr, "[%-4d]", list->prev[i]);
    }
    fprintf(stderr, "\nfree:     ");
    for (size_t i = 0; i < list->capacity; i++) {
        fprintf(stderr, "[%-4d]", list->free[i]);
    }

    fprintf(stderr, "\nin order: ");
    print_order_of_data(stderr, list);
    fprintf(stderr, "\nhead: %d\ntail: %d\nlast_free: %d\nsize: %d\n", list->head, list->tail, list->last_free, list->size);
}

void print_site_toes() {
    FILE * fp = fopen_with_dirs(_LIST_DUMP_SITE_FILE_PATH, "a");\
    sassert(fp, _LST_ERR_PTR_NULL);\

    fprintf(fp, "<p style=\"color: #a30f7eff\">||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||</p>\n");
    fprintf(fp, "</body>\n"
                "</html");
    fclose(fp);
}

LstErrors create_dot_main_array_dump(list_t *list, FILE * fp) {
    fprintf(fp, "\ndigraph {\nrankdir=LR\nbgcolor=\"#ffffff\"\n"
                "ranksep=0.0\nsplines=ortho\n"
                "node[shape=record,color=\"#000000ff\", fontcolor=\"#000000ff\", style=\"filled\", fillcolor=\"#87ef8782\"]\n");
    const char * str_if_head = "| <FONT COLOR=\"#ff0000ff\"> HEAD</FONT> ";
    const char * str_if_tail = "| <FONT COLOR=\"#7300ffff\">TAIL</FONT> ";
    const char * str_if_free = "| <FONT COLOR=\"MAGENTA\">  LAST FREE</FONT> ";

    int head = list->head;
    int tail = list->tail;
    int free = list->last_free;
    for (size_t i = 0; i < list->capacity; i++) {
        if (list->data[i] == (list_el_t) DATA_POISON)
            fprintf(fp, "data_array_info%zu[label=<index in array: %zu | value: <FONT COLOR=\"magenta\">PSN</FONT> | {prev: %d | next: %d} | free: %d", i, i, list->data[i], list->prev[i], list->free[i]);
        else
            fprintf(fp, "data_array_info%zu[label=<index in array: %zu | value: <FONT COLOR=\"red\">%" DATA_SPEC "</FONT>   | {prev: %d | next: %d} | free: %d", i, i, list->data[i], list->prev[i], list->next[i], list->free[i]);
        if (i != 0)
            fprintf(fp, " %s %s %s>]\n", (i == head) ? str_if_head : " ", (i == tail) ? str_if_tail : " ", (i == free) ? str_if_free : " ");
        else
            fprintf(fp, " %s %s %s>, fillcolor=\"#8059596e\"]\n", (i == head) ? str_if_head : " ", (i == tail) ? str_if_tail : " ", (i == free) ? str_if_free : " ");
        }
    
    for (size_t i = 0; i < list->capacity - 1; i++) {
        fprintf(fp, "data_array_info%zu->", i);
    }
    fprintf(fp, "data_array_info%d [style=\"invis\", weight=500, minlen=4]", list->capacity - 1);
    return _LST_OK;
}

LstErrors add_error_to_html(FILE *fp, list_t *list) {
    sassert(fp,     _LST_ERR_PTR_NULL);
    sassert(list,   _LST_ERR_PTR_NULL);

    if (verify_list(list) == _LST_OK) 
        return _LST_OK;

    fprintf(fp, "<div class=\"error_div\">");
    int free        = list->last_free;
    int tail        = list->tail;
    int head        = list->head;
    int size        = list->size;
    int current     = tail;
    int previous    = 0;
    int count_els   = 0;
    if (tail < 0 || head < 0 || size < 0 || head > list->capacity || tail > list->capacity || size > list->capacity)
        fprintf(fp, "<p> неправильные параметры списка: tail: %d, head: %d, last_free: %d, size: %d </p>\n", tail, head, free, size);

    while (current != POISON && current < list->capacity) {
        if (count_els++ > size) {
            fprintf(fp, "<p> насчитанное количество элементов не соответствует размеру списка: size: %d, count_els: %d </p>\n", size, count_els - 1);
            break;
        }

        bool FirstErrors = true;
        if (list->next[current] > list->capacity || (list->next[current] == POISON && current != list->head)) {
            fprintf(fp, "<p> у элемента с индексом %d next неправильный (%d) </p>\n",  current, list->next[current]);
            FirstErrors = false;
        }

        if (list->prev[current] > list->capacity || (list->prev[current] == POISON && current != list->tail)) {
            fprintf(fp, "<p> у элемента с индексом %d prev неправильный (%d) </p>\n", current, list->prev[current]);
            FirstErrors = false;
        }

        if (FirstErrors && current != head && (current != list->prev[list->next[current]] || previous != list->prev[current]))
            fprintf(fp, "<p> у элементы с индексом %d (%d) неправильный prev[next]: %d, а должен быть %d  </p>\n", current, list->next[current], list->prev[list->next[current]], current);
        
        if (FirstErrors && current != tail && (current != list->next[list->prev[current]] || list->next[previous] != current))
            fprintf(fp, "<p> у элементы с индексом %d (%d) неправильный next[prev]: %d, а должен быть %d  </p>\n", current, list->prev[current], list->next[list->prev[current]], current);
        
        previous = current;
        current = list->next[current];
    }

    if (current >= (int) list->capacity) {
        fprintf(fp, "<p> дошли к элементу с индексом %d, большим чем capacity: %d </p>\n", current, list->capacity);
    }
    fprintf(fp, "</div>\n");

    return _LST_OK;
}

LstErrors verify_list(list_t *list) {
    sassert(list, _LST_ERR_PTR_NULL);

    int free = list->last_free;
    int tail = list->tail;
    int head = list->head;
    int current = tail;
    int size = list->size;
    int previous = 0;
    int count_els = 0;
    _LST_RETURN_ERR(tail >= 0 && head >= 1 && size >= 0, _LST_ERR_INVALID_ARGUMENTS,
                    "неправильные параметры списка: tail: %d, head: %d, last_free: %d, size: %d", tail, head, free, size);

    while (current != POISON) {
        _LST_RETURN_ERR(count_els++ <= size + 1, _LST_ERR_INVALID_SIZE,
                        "насчитанное количество элементов не соответствует размеру списка: size: %d, count_els: %d", size, count_els);

        _LST_RETURN_ERR(current != POISON && current <= size + 1 && list->next[current] <= size + 1 && list->next[current] != POISON, _LST_ERR_INVALID_NEXT,
                        "у элемента с индексом %d следующий элемент неправильно с ним связан", current);

        _LST_RETURN_ERR(list->prev[current] <= size + 1 && list->prev[current] != POISON, _LST_ERR_INVALID_PREV,
                        "у элемента с индексом %d предыдущий элемент неправильно с ним связан", current);
        
        _LST_RETURN_ERR(!(current != head && (list->next[current] == POISON || current != list->prev[list->next[current]])), _LST_ERR_INVALID_RELATION_WITH_NEXT,
                        "у элемента с индексом %d следующий элемент неправильно с ним связан", current);
        
        previous = current;
        current = list->next[current];
    }

    _LST_RETURN_ERR(count_els <= size + 1, _LST_ERR_INVALID_SIZE,
                    "насчитанное количество элементов не соответствует размеру списка: size: %d, count_els: %d", size, count_els);
    
    return _LST_OK;
}


#define ADD_ERROR_STR_ARR(condition, str) \
    if (condition) {\
        ErrStrArr[ErrStrArrInd] = str;\
        ErrStrArrInd++;\
    }

void print_error_to_dot_image(list_t *list, FILE *fp) {
    sassert(fp,     _LST_ERR_PTR_NULL);
    sassert(list,   _LST_ERR_PTR_NULL);

    int current     = list->tail;
    int previous    = 0;
    int count_els   = 0;
    int err_count   = 0;
    const char *str = NULL;
    bool Exit = false;

    while (current != POISON) {
        if (count_els++ > list->size || current > list->capacity)
            break;
        
        int ErrStrArrInd = 0;
        const char *ErrStrArr[10] = {};

        ADD_ERROR_STR_ARR((list->data[current] == POISON),                           "data shouldnt be POISON")
        ADD_ERROR_STR_ARR((list->next[current] > list->capacity + 1),                "next value of this element is more than capacity")
        ADD_ERROR_STR_ARR((list->next[current] == POISON && current != list->head),  "next value of non-head element is POISON")
        ADD_ERROR_STR_ARR((list->next[current] == list->last_free),                  "next value of this element is last free")
        ADD_ERROR_STR_ARR((list->prev[current] > list->capacity + 1),                "prev value of this element is more than capacity")
        ADD_ERROR_STR_ARR((list->prev[current] == POISON && current != list->tail),  "prev value of non-tail element is POISON")
        
        for (size_t i = 0; i < ErrStrArrInd; i++) {
            if (str != NULL) {
                fprintf(fp, "data_array_info%d[fillcolor=\"#e93131b4\"]\n", current);
                fprintf(fp, "error%d[weight=1,color=\"#ff0000ff\", minlen=4, style=\"filled\", fillcolor=\"#e93131b4\", label=\"invalid next\"]\n"
                            "data_array_info%d->error%d[weight=1,color=\"#ff0000ff\", minlen=4]\n", err_count, current, err_count);
            }
            err_count++;
        }
        if (current != list->head                       &&
            list->next[current] != POISON               &&
            list->next[current] <= list->capacity + 1   &&
            current != list->prev[list->next[current]]) {
                    fprintf(fp, "data_array_info%d[fillcolor=\"#e93131b4\"]\n", current);
                    fprintf(fp, "error%d[weight=1,color=\"#ff0000ff\", style=\"filled\", fillcolor=\"#e93131b4\", minlen=4, label=\"invalid relation\"]\n"
                                "data_array_info%d->error%d[weight=1,color=\"#ff0000ff\", minlen=4]\n", err_count, current, err_count);
                    if (list->next[current] > 0 && list->next[current] < list->capacity) {
                        fprintf(fp, "data_array_info%d[fillcolor=\"#e93131b4\"]\n", list->next[current]);
                        fprintf(fp, "data_array_info%d->error%d[weight=1,color=\"#ff0000ff\", minlen=4]\n", list->next[current], err_count);
                    }
            }
        previous = current;
        current = list->next[current];
    }
}

LstErrors create_dot_image_dump(list_t *list) {
    char command[MAX_STR_SIZE] = {};
    snprintf(command, MAX_STR_SIZE - 1, "dot -Gdpi=80 -Tpng -o %s%d.png", _LIST_DUMP_GRAPH_DIR_NAME, count_graphs);

    create_parent_directories(_LIST_DUMP_GRAPH_DIR_NAME);
    FILE * fp = popen(command, "w");
    _LST_RETURN_ERR(fp, _LST_ERR_PIPE_FAIL, "не удалось открыть dot. Проверьте его наличие");
    
    create_dot_main_array_dump(list, fp);
    int current = list->tail;
    int count_els = 0;
    if (verify_list(list) != _LST_OK)
        print_error_to_dot_image(list, fp);
    
    while (count_els <= list->size) {
        int next = list->next[current];
        if (next > -1 && next <= list->size + 1 && current != next)
            fprintf(fp, "data_array_info%d->data_array_info%d [constraint=false, weight=1,color=\"#0d00ffff\", minlen=4]\n", current, next);
        current = next;
        count_els++;
    }

    current = list->head;
    count_els = 0;
    while (current != POISON && current <= list->size && count_els <= list->size) {
        int prev = list->prev[current];
        if (prev > -1 && prev < list->capacity && current != prev)
            fprintf(fp, "data_array_info%d->data_array_info%d [constraint=false, weight=1,color=\"#ff0084ff\", minlen=4]\n", current, prev);
        current = prev;
        count_els++;
    }

    fprintf(fp, "}");
    fclose(fp);

    return _LST_OK;
}

LstErrors print_to_html(list_t *list, operations operation, size_t index, list_el_t value) {
    sassert(list, _LST_ERR_PTR_NULL);

    FILE * fp = fopen_with_dirs(_LIST_DUMP_SITE_FILE_PATH, "a");
    _LST_RETURN_ERR(fp, _LST_ERR_PIPE_FAIL, "не удалось открыть файл <%s>. Проверьте его наличие", _LIST_DUMP_SITE_FILE_PATH);

    if (operation == START)
        fprintf(fp, "<p style=\"color: #a30f7eff; font-weight: bold; \">||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||</p>\n");

    add_error_to_html(fp, list);
    if (value != (list_el_t) DATA_POISON)
        fprintf(fp, "<h2>%s at index: %zu, value: %" DATA_SPEC "</h2>\n", operations_descriptions[operation], index, value);
    else 
        fprintf(fp, "<h2>%s at index: %zu, value: NONE</h2>\n", operations_descriptions[operation], index);
    fprintf(fp, "<pre class = \"%s\">\n", operations_classes[operation]);
    fprintf(fp, "\ndata:     ");
    for (size_t i = 0; i < list->capacity; i++) {
        fprintf(fp, "[%-4" DATA_SPEC "]", list->data[i]);
    }
    fprintf(fp, "\nnext:     ");
    for (size_t i = 0; i < list->capacity; i++) {
        fprintf(fp, "[%-4d]", list->next[i]);
    }
    fprintf(fp, "\nprev:     ");
    for (size_t i = 0; i < list->capacity; i++) {
        fprintf(fp, "[%-4d]", list->prev[i]);
    }

    fprintf(fp, "\nfree:     ");
    for (size_t i = 0; i < list->capacity; i++) {
        fprintf(fp, "[%-4d]", list->free[i]);
    }

    fprintf(fp, "\nin order: ");
    print_order_of_data(fp, list);

    fprintf(fp,     "\nhead: %d\ntail: %d\nlast_free: %d\nsize: %d\n</pre>\n", list->head, list->tail, list->last_free, list->size);
    fprintf(fp,     "<div class=\"images\">\n"
                    "<img src=\"%s%d.png\" class=\"img1\">\n</div>\n", _LIST_DUMP_GRAPH_DIR_NAME, count_graphs);
    count_graphs++;

    fclose(fp);

    return _LST_OK;
}

void print_site_headers() {
    FILE * fp = fopen_with_dirs(_LIST_DUMP_SITE_FILE_PATH, "w");
    sassert(fp, _LST_ERR_PTR_NULL);
    fprintf(fp, "<!DOCTYPE html>\n"
                "<html lang=\"ru\">\n"
                "<head>\n"
                "<style>\n"
                ".add_after, .remove, .add_before{\n"
                "color: #000000;\n"
                "}\n"
                ".start {\n"
                "color: #000000;\n"
                "}\n"
                "h2, p{\n"
                "margin: 0;\n"
                "}\n"
                "h2 {\n"
                "color: rgb(30, 0, 255);\n"
                "font-weight: bold;\n"
                "}\n"
                ".error_div {\n"
                "background-color: #d82727a5;\n"
                "border-radius: 5px;\n"
                "}\n"
                ".error_div:hover{\n"
                "background-color: #d82727e5;\n"
                "cursor: pointer;\n"
                "}\n"
                "label {\n"
                "background: #8df4cca4;\n"
                "color: black;\n"
                "padding: 5px 10px;\n"
                "cursor: pointer;\n"
                "display: inline-block;\n"
                "margin: 5px 0;\n"
                "}\n"
                ".images {\n"
                    "position: relative;\n"
                    "display: inline-block;\n"
                    "height: 230px;\n"
                "}\n"
                "</style>\n"
                "<title>my list dump</title>\n"
                "</head>\n"
                "<body width=\"device-width\">\n"
            );
    fclose(fp);
}