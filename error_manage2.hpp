#ifndef ERROR_MANAGE2_H
#define ERROR_MANAGE2_H

#ifndef _LIB_ERROR_MAX_STR_SIZE
    #define _LIB_ERROR_MAX_STR_SIZE 256
#endif

typedef enum {
    STR_TYPE = 0,
    INT_TYPE = 1,
    FLT_TYPE = 2
} ArgType_t;

typedef struct {
    char str[_LIB_ERROR_MAX_STR_SIZE];
} Err_t;

#define ADD_ERR(string, ...) {                                                                  \
    snprintf(_lib_last_error.str, _LIB_ERROR_MAX_STR_SIZE - 1, string, ##__VA_ARGS__);  \
}

#endif // ERROR_MANAGE2_H
