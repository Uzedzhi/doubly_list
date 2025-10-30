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

error_t create_dot_image_next_array_dump(list_t *list) {
    sassert(list, ERR_PTR_NULL);

    FILE * fp = fopen(dump_graph_file_name, "w");
    sassert(fp, ERR_PTR_NULL);

    create_dot_main_array_dump(list, fp);
    int current     = list->tail;
    int count_tails = 0;
    int count_els   = 0;
    while (current != POISON) {
        int end = list->next[current];
        if (count_els > list->size) {
            add_error(ERR_CYCLING_LIST, " ");
            fprintf(fp, "error%zu[weight=1,color=\"#ff0000ff\", minlen=4, label=\"invalid cycle\"]\n"
                        "data_array_info%zu->error%zu[weight=1,color=\"#ff0000ff\", minlen=4]\n", count, current, count);
            break;
        }
        if (end > (int) list->capacity || end < -2 || (end == -1 && current != list->head)) {
            fprintf(fp, "error%zu[weight=1,color=\"#ff0000ff\", minlen=4, label=\"invalid next\"]\n"
                        "data_array_info%zu->error%zu[weight=1,color=\"#ff0000ff\", minlen=4]\n", count, current, count);
            add_error(ERR_INCORRECT_LIST, " ");
            break;
        }
        if (end != POISON)
            fprintf(fp, "data_array_info%zu->data_array_info%zu [weight=1,color=\"#00c414ff\", minlen=4]\n", current, end);
        current = end;
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

error_t create_dot_main_array_dump(list_t *list, FILE * fp) {
    fprintf(fp, "\ndigraph {\nrankdir=LR\nbgcolor=\"#ffffff\"\n"
                "ranksep=0.0\nsplines=ortho\n"
                "node[shape=record,color=\"#000000ff\", fontcolor=\"#000000ff\", style=\"filled\", fillcolor=\"#22c85682\"]\n");
    char * str_if_head = "| <FONT COLOR=\"#ff0000ff\"> HEAD</FONT> ";
    char * str_if_tail = "| <FONT COLOR=\"ORANGE\">TAIL</FONT> ";
    char * str_if_free = "| <FONT COLOR=\"BLUE\">  FREE</FONT> ";
    int head = list->head;
    int tail = list->tail;
    int free = (int) get_top(list->free);
    for (size_t i = 0; i < list->capacity; i++) {
        if (list->data[i] == POISON)
            fprintf(fp, "data_array_info%zu[label=<index in array: %zu | value: <FONT COLOR=\"magenta\">PSN</FONT> | {prev: %d | next: %d}", i, i, -1, -1);
        else
            fprintf(fp, "data_array_info%zu[label=<index in array: %zu | value: <FONT COLOR=\"red\">%d</FONT>   | {prev: %d | next: %d}", i, i, list->data[i], list->prev[i], list->next[i]);
        fprintf(fp, " %s %s %s>]\n", (i == head) ? str_if_head : " ", (i == tail) ? str_if_tail : " ", (i == free) ? str_if_free : " ");
        }
    for (size_t i = 0; i < list->capacity - 1; i++) {
        fprintf(fp, "data_array_info%zu->", i);
    }
    fprintf(fp, "data_array_info%zu [style=\"invis\", weight=500, minlen=4]", list->capacity - 1);
    return error;
}

void add_error_to_html(list_t *list, int index, int value) {
    sassert(list, ERR_PTR_NULL);

    FILE * fp = fopen(dump_site_file_name, "a");
    sassert(fp, ERR_PTR_NULL);
    fprintf(fp, "<div style=\"width:180px; background-color: #ff0000c5; height:90px; position:fixed; top:20px; right:20px;border-radius: 5px;\">");
    if (error.error_info[ERR_INCORRECT_LIST][0] != '\0')
        fprintf(fp, "error with connections at index %d, value %d", index, value);
    if (error.error_info[ERR_CYCLING_LIST][0] != '\0')
        fprintf(fp, " and your list is cycling at index %d, value %d", index, value);
    fprintf(fp, "</div>");
    fclose(fp);
}

error_t create_dot_image_prev_array_dump(list_t *list) {
    FILE * fp = fopen(dump_graph_file_name, "w");
    sassert(fp, ERR_PTR_NULL);

    create_dot_main_array_dump(list, fp);
    int current = list->head;
    int count_els = 0;
    while (current != POISON) {
        int end = list->prev[current];
        if (count_els > list->size) {
            add_error(ERR_CYCLING_LIST, " ");
            fprintf(fp, "error%zu[weight=1,color=\"#ff0000ff\", minlen=4, label=\"invalid cycle\"]\n"
                        "data_array_info%zu->error%zu[weight=1,color=\"#ff0000ff\", minlen=4]\n", count, current, count);
            break;
        }
        
        if (end > (int) list->capacity || end < -2 || (end == -1 && current != list->tail)) {
            fprintf(fp, "error%zu[weight=1,color=\"#ff0000ff\", minlen=4, label=\"invalid prev\"]\n"
                        "data_array_info%zu->error%zu[weight=1,color=\"#ff0000ff\", minlen=4]", count, current, count);
            add_error(ERR_INCORRECT_LIST, " ");
            break;
        }
        if (end != POISON)
            fprintf(fp, "data_array_info%zu->data_array_info%zu [weight=1,color=\"#0d00ffff\", minlen=4]\n", current, end);
        
        current = end;
        count_els++;
    }
    fprintf(fp, "}");
    fclose(fp);
    char command[MAX_STR_SIZE] = {};
    snprintf(command, MAX_STR_SIZE - 1, "dot graph.txt -Gdpi=80 -Tpng -o graph/graph%zu_prev.png", count++);
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
                    "<img src=\"graph/graph%zu.png\" class=\"img1\">\n"
                    "<img src=\"graph/graph%zu_prev.png\" class=\"img2\">\n</div>\n",  count, count);
    fclose(fp);
}

void print_site_headers() {
    FILE * fp = fopen(dump_site_file_name, "w");
    sassert(fp, ERR_PTR_NULL);

    // fprintf(fp, "<!DOCTYPE html>\n"
    //             "<html lang=\"ru\">\n"
    //             "<head>\n"
    //             "<style>\n"
    //             ".add_after, .remove, .add_before{\n"
    //             "color: #00ff9dc5;\n"
    //             "}\n"
    //             ".start {\n"
    //             "color: #b2ec06ff;\n"
    //             "}\n"
    //             "h2, p{\n"
    //             "margin: 0;\n"
    //             "}\n"
    //             "h2 {\n"
    //             "color: #a30f7eff;\n"
    //             "font-weight: bold;\n"
    //             "}\n"
    //             "label.check {\n"
    //             "position: fixed;\n"
    //             "top: 20px;\n"
    //             "left: 30px;\n"
    //             "background: #00ff9dc5;\n"
    //             "color: black;\n"
    //             "padding: 10px 15px;\n"
    //             "cursor: pointer;\n"
    //             "display: inline-block;\n"
    //             "z-index: 9999;\n"
    //             "border-radius: 5px;\n"
    //             "}\n"
    //             "label.check:hover {\n"
    //             "background: #00ff9dff;\n"
    //             "}\n"
    //             "body {\n"
    //             "background-color: black;\n"
    //             "}\n"
    //             "input[type=\"checkbox\"] {\n"
    //             "display: none;\n"
    //             "}\n"
    //             "label {\n"
    //                 "background: #8df4cca4;\n"
    //                 "color: black;\n"
    //                 "padding: 5px 10px;\n"
    //                 "cursor: pointer;\n"
    //                 "display: inline-block;\n"
    //                 "margin: 5px 0;\n"
    //             "}\n"
    //             ".images {\n"
    //                 "position: relative;\n"
    //                 "display: inline-block;\n"
    //                 "height: 230px;\n"
    //             "}\n"
    //             ".images .img1, .images .img2 {\n"
    //                 "position: absolute;\n"
    //                 "top: 0;\n"
    //                 "left: 0;\n"
    //             "}\n"
    //             ".images .img1 { opacity: 1; }\n"
    //             ".images .img2 { opacity: 0; }\n"
    //             "input:checked ~ .images .img1 { opacity: 0; }\n"
    //             "input:checked ~ .images .img2 { opacity: 1; }\n"
    //             "</style>\n"
    //             "<title>my list dump</title>\n"
    //             "</head>\n"
    //             "<body width=\"device-width\">\n"
    //         );
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
                "label.check {\n"
                "position: fixed;\n"
                "top: 20px;\n"
                "left: 30px;\n"
                "background: #ff9f0fd5;\n"
                "color: rgb(0, 0, 0);\n"
                "padding: 10px 15px;\n"
                "cursor: pointer;\n"
                "display: inline-block;\n"
                "z-index: 9999;\n"
                "border-radius: 5px;\n"
                "}\n"
                "label.check:hover {\n"
                "background: #00ff9dff;\n"
                "}\n"
                "input[type=\"checkbox\"] {\n"
                "display: none;\n"
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
                ".images .img1, .images .img2 {\n"
                    "position: absolute;\n"
                    "top: 0;\n"
                    "left: 0;\n"
                "}\n"
                ".images .img1 { opacity: 1; }\n"
                ".images .img2 { opacity: 0; }\n"
                "input:checked ~ .images .img1 { opacity: 0; }\n"
                "input:checked ~ .images .img2 { opacity: 1; }\n"
                "</style>\n"
                "<title>my list dump</title>\n"
                "</head>\n"
                "<body width=\"device-width\">\n"
            );
    fprintf(fp, "<input type=\"checkbox\", id =\"t1\">\n"
                "<label for=\"t1\", class=\"check\">change</label>\n");
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
