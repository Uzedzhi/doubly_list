#ifndef ERROR_MANAGE_H
#define ERROR_MANAGE_H

#define MAX_SIZE 200
#define MAX_ERROR_CODES 32

typedef struct error_t {
    char error_info[MAX_ERROR_CODES][MAX_SIZE];
    int code;
    bool is_error;
} error_t;

#define add_error(error_code, ...) {                    \
    char __err_str[200] = {};                      \
    snprintf(__err_str, 199, ##__VA_ARGS__);   \
    strcpy(error.error_info[error_code], __err_str);    \
    error.code |= (1 << error_code);                    \
    error.is_error = true;                              \
}

void print_error(const char * const error_text[], error_t error);

#undef MAX_SIZE
#undef MAX_ERROR_CODES

#endif // error_manage_h