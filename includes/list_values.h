#ifndef LIST_VALUES_H
#define LIST_VALUES_H

#include <stdlib.h>

#ifdef __STDC_NO_THREADS__
    #define THREAD_LOCAL __thread
#else
    #define THREAD_LOCAL _Thread_local
#endif

#define _LST_ERROR_BUF_MAX_SIZE 256
#ifndef NDEBUG
    #define _LST_ADD_ERR(string, ...) \
        snprintf(lst_last_error, _LST_ERROR_BUF_MAX_SIZE - 1, string, ##__VA_ARGS__);
#else
    #define _LST_ADD_ERR(string, ...)
#endif


#define _LST_RETURN_ERR(condition, code, string, ...) \
    if (!(condition)) {\
        _LST_ADD_ERR(string, ##__VA_ARGS__)\
        return code;\
    }

const  size_t START_LIST_SIZE   = 10;
const  size_t MAX_STR_SIZE      = 200;
const  size_t INITIAL_HEAD_VAL  = 0;
const  size_t INITIAL_TAIL_VAL  = 0;
const  size_t INITIAL_FREE_VAL  = 0;
const  size_t MAX_TESTING_SIZE  = 100;
typedef const char * const string;

#ifdef STRING_TYPE
    string DATA_POISON = NULL;

    #define DATA_SPEC "s"
#else
    const int DATA_POISON = -1;

    #define DATA_SPEC "d"
#endif // STRING_TYPE

const int POISON = -1;

string operations_descriptions[]    = {"added after index to a list ", "before operation happened ", "after removal from list ", "added before index to the list "};
string operations_classes[]         = {"add_after", "start", "remove", "add_before"};

#define initialize_with_poison(array, poison) {\
    for (size_t _ = 0; _ < START_LIST_SIZE; _++) {\
        array[_] = poison;\
    }\
}

enum operations {
    ADD_AFTER       = 0,
    START           = 1,
    REMOVE          = 2,
    ADD_BEFORE      = 3,
    UNDEF_OPERATION = 4
};

#endif // LIST_VALUES_H