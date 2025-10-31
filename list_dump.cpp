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
    print_order_of_data(stderr, list);
    fprintf(stderr, "\nhead: %d\ntail: %d\nfree: %d\nsize: %d\n", list->head, list->tail, list->free, list->size);
}

void print_site_toes() {
    FILE * fp = fopen(dump_site_file_name, "a");\
    sassert(fp, ERR_PTR_NULL);\

    fprintf(fp, "<p style=\"color: #a30f7eff\">||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||</p>\n");
    fprintf(fp, "</body>\n"
                "</html");
    fclose(fp);
    
}

error_t create_dot_main_array_dump(list_t *list, FILE * fp) {
    fprintf(fp, "\ndigraph {\nrankdir=LR\nbgcolor=\"#ffffff\"\n"
                "ranksep=0.0\nsplines=ortho\n"
                "node[shape=record,color=\"#000000ff\", fontcolor=\"#000000ff\", style=\"filled\", fillcolor=\"#87ef8782\"]\n");
    char * str_if_head = "| <FONT COLOR=\"#ff0000ff\"> HEAD</FONT> ";
    char * str_if_tail = "| <FONT COLOR=\"#7300ffff\">TAIL</FONT> ";
    char * str_if_free = "| <FONT COLOR=\"MAGENTA\">  FREE</FONT> ";

    int head = list->head;
    int tail = list->tail;
    int free = (int) get_top(list->free);
    for (size_t i = 0; i < list->capacity; i++) {
        if (list->data[i] == POISON)
            fprintf(fp, "data_array_info%zu[label=<index in array: %zu | value: <FONT COLOR=\"magenta\">PSN</FONT> | {prev: %d | next: %d}", i, i, -1, -1);
        else
            fprintf(fp, "data_array_info%zu[label=<index in array: %zu | value: <FONT COLOR=\"red\">%d</FONT>   | {prev: %d | next: %d}", i, i, list->data[i], list->prev[i], list->next[i]);
        if (i != 0)
            fprintf(fp, " %s %s %s>]\n", (i == head) ? str_if_head : " ", (i == tail) ? str_if_tail : " ", (i == free) ? str_if_free : " ");
        else
            fprintf(fp, " %s %s %s>, fillcolor=\"#8059596e\"]\n", (i == head) ? str_if_head : " ", (i == tail) ? str_if_tail : " ", (i == free) ? str_if_free : " ");
        }
    
    for (size_t i = 0; i < list->capacity - 1; i++) {
        fprintf(fp, "data_array_info%zu->", i);
    }
    fprintf(fp, "data_array_info%zu [style=\"invis\", weight=500, minlen=4]", list->capacity - 1);
    return error;
}

void add_error_to_html(list_t *list, int index, int value) {
    sassert(list, ERR_PTR_NULL);

    if (error.is_error == true) {
        FILE * fp = fopen(dump_site_file_name, "a");
        sassert(fp, ERR_PTR_NULL);
        fprintf(fp, "<div class=\"error_div\">");

        int count = 0;
        int errors = error.code;
        int err_count = 0;
        while (errors > 0) {
            if ((errors & 1) == 1) {
                fprintf(fp, "<p> %s: at %s index</p>", error_text[count], error.error_info[count]);
            }
            count++;
            errors >>= 1;
        }
        fprintf(fp, "</div>");
        fclose(fp);
    }
}

int verify_list(list_t *list) {
    sassert(list, ERR_PTR_NULL);

    int free = (int) get_top(list->free);
    int tail = list->tail;
    int head = list->head;
    int current = tail;
    int size = list->size;
    int previous = 0;
    int count_els = 0;
    if (tail < 1 || head < 1 || free < 1 || size < 0) {
        add_error(ERR_INVALID_ARGUMENTS, "tail: %d, head: %d, free: %d, size: %d", tail, head, free, size);
        return 0;
    }

    while (current != POISON) {
        if (count_els++ > size + 1) {
            add_error(ERR_INVALID_SIZE, "size: %d, count_els: %d", size, count_els);
            break;
        }
        if (current < -1 || current > size + 1 || list->next[current] > size + 1 || list->next[current] < -1) {
            add_error(ERR_INVALID_NEXT, "%d", current);
            break;
        }
        if (list->prev[current] > size + 1 || list->prev[current] < -1) {
            add_error(ERR_INVALID_PREV, "%d", current);
        }
        if (current != head && (list->next[current] == -1 || current != list->prev[list->next[current]])) {
            add_error(ERR_INVALID_RELATION_WITH_NEXT, "%d", current);
        }
        previous = current;
        current = list->next[current];
    }
    if (count_els > size + 1) {
        add_error(ERR_INVALID_SIZE, "size: %d, count_els: %d", size, count_els);
    }
    return current;
}

void print_error_to_dot_image(list_t *list, FILE *fp) {
    sassert(fp,     ERR_PTR_NULL);
    sassert(list,   ERR_PTR_NULL);

    int count = 0;
    int errors = error.code;
    int err_count = 0;

    while (errors > 0) {
        if ((errors & 1) == 1) {
            int index = atoi(error.error_info[(int) count]);
            int showed_index = index;
            const char * name = "";
            switch((lstErrors) count) {
                case ERR_INVALID_PREV:
                case ERR_INVALID_NEXT:
                    if (index < 0)
                        break;
                    if ((lstErrors) count == ERR_INVALID_PREV)
                        name = "prev";
                    else 
                        name = "next";
                    
                    if ((lstErrors) count == ERR_INVALID_PREV && list->prev[index] > 0 && list->prev[index] <= list->size) 
                        showed_index = list->prev[index];
                    else if ((lstErrors) count == ERR_INVALID_NEXT && list->next[index] > 0 && list->next[index] <= list->size)
                        showed_index = list->next[index];
                    
                    fprintf(fp, "data_array_info%zu[fillcolor=\"#e93131b4\"]\n", showed_index);
                    fprintf(fp, "error%zu[weight=1,color=\"#ff0000ff\", minlen=4, style=\"filled\", fillcolor=\"#e93131b4\", label=\"invalid %s\"]\n"
                                "data_array_info%zu->error%zu[weight=1,color=\"#ff0000ff\", minlen=4]\n", err_count, name, showed_index, err_count);
                    err_count++;
                    break;
                case ERR_INVALID_RELATION_WITH_NEXT:
                    if (index < 0)
                        break;
                    
                    fprintf(fp, "data_array_info%zu[fillcolor=\"#e93131b4\"]\n", index);
                    fprintf(fp, "error%zu[weight=1,color=\"#ff0000ff\", style=\"filled\", fillcolor=\"#e93131b4\", minlen=4, label=\"invalid relation\"]\n"
                                "data_array_info%zu->error%zu[weight=1,color=\"#ff0000ff\", minlen=4]\n", err_count, index, err_count);
                    if (list->next[index] > 0 && list->next[index] < list->size) {
                        fprintf(fp, "data_array_info%zu[fillcolor=\"#e93131b4\"]\n", list->next[index]);
                        fprintf(fp, "data_array_info%zu->error%zu[weight=1,color=\"#ff0000ff\", minlen=4]\n", list->next[index], err_count);
                    }
                    
                    err_count++;
                    break;
            }
        }
        errors >>= 1;
        count++;
    }
}

error_t create_dot_image_dump(list_t *list) {
    FILE * fp = fopen(dump_graph_file_name, "w");
    sassert(fp, ERR_PTR_NULL);

    create_dot_main_array_dump(list, fp);
    int current = list->tail;
    int count_els = 0;
    int error_element = verify_list(list);
    if (error.is_error == true)
        print_error_to_dot_image(list, fp);
    
    while (current > -1 && current <= list->size + 1 && count_els++ <= list->size + 2) {
        int next = list->next[current];
        if (next > -1 && next <= list->size + 1)
            fprintf(fp, "data_array_info%zu->data_array_info%zu [constraint=false, weight=1,color=\"#0d00ffff\", minlen=4]\n", current, next);
        current = next;
        count_els++;
    }

    current = list->head;
    count_els = 0;
    while (current > -1 && current <= list->size + 1 && count_els++ <= list->size + 2) {
        int prev = list->prev[current];
        if (prev > -1 && prev <= list->size + 1)
            fprintf(fp, "data_array_info%zu->data_array_info%zu [constraint=false, weight=1,color=\"#ff0084ff\", minlen=4]\n", current, prev);
        current = prev;
        count_els++;
    }

    fprintf(fp, "}");
    fclose(fp);
    char command[MAX_STR_SIZE] = {};
    snprintf(command, MAX_STR_SIZE - 1, "dot graph.txt -Gdpi=80 -Tpng -o graph/graph%zu.png", count);
    if (system(command) != 0) {
        add_error(ERR_CMD_INVALID, "%s", command);
    }
    return error;
}

error_t print_to_html(list_t *list, operations operation, size_t index, int value) {
    sassert(list, ERR_PTR_NULL);

    verify_list(list);
    if (error.is_error == true)
        return error;

    FILE * fp = fopen(dump_site_file_name, "a");
    sassert(fp, ERR_PTR_NULL);


    if (operation == START)
        fprintf(fp, "<p style=\"color: #a30f7eff; font-weight: bold; \">||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||</p>\n");

    if (value != POISON)
        fprintf(fp, "<h2>%s at index: %zu, value: %d</h2>\n", operations_descriptions[operation], index, value);
    else 
        fprintf(fp, "<h2>%s at index: %zu, value: NONE</h2>\n", operations_descriptions[operation], index, value);
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
    if (!error.is_error) {
        print_order_of_data(fp, list);
    }

    fprintf(fp,     "\nhead: %d\ntail: %d\nfree: %d\nsize: %d\n</pre>\n", list->head, list->tail, list->free, list->size);
    fprintf(fp,     "<div class=\"images\">\n"
                    "<img src=\"graph/graph%zu.png\" class=\"img1\">\n</div>\n",  count);
    count++;
    fclose(fp);
    return error;
}

void print_site_headers() {
    FILE * fp = fopen(dump_site_file_name, "w");
    sassert(fp, ERR_PTR_NULL);
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
                "padding: 10px;\n"
                "background-color: #d82727a5;\n"
                "position:fixed;\n"
                "top: 20px;\n"
                "right: 20px;\n"
                "border-radius: 5px;\n"
                "z-index: 9999;\n"
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

#ifndef NDEBUG
error_t open_live_server(const char * site_name) {
    sassert(site_name, ERR_PTR_NULL);

    char command[MAX_STR_SIZE] = {};
    snprintf(command, MAX_STR_SIZE - 1, "open %s", site_name);
    if (system(command) != 0) {
        add_error(ERR_CMD_INVALID, "%s", command);
    }
    return error;
}
#else
error_t open_live_server(const char * site_name) {return error;}
#endif 
