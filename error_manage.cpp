#include <string.h>
#include <stdio.h>
#include "error_manage.hpp"
#include "better_output.hpp"

void print_error(const char * const error_text[], error_t error) {
    if (error.is_error == true)
        fprintf(stderr, RED "all errors:\n" RESET);
    size_t count = 0;
    while (error.code) {
        if ((error.code & 1) == 1 && error_text[count] && error.error_info[count]) {
            if (strlen(error.error_info[count]) != 1)
                fprintf(stderr, "%s -> %s\n", error_text[count], error.error_info[count]);
            else
                fprintf(stderr, "%s\n", error_text[count]);
        }
        error.code >>= 1;
        count++;
    }
}