#ifndef SASSERT_H
#define SASSERT_H

#include "better_output.hpp"

#define sassert(condition, error_code, ...)                                         \
    if (!(condition)) {                                                             \
        fprintf(stderr, RED "ERROR: " WHITE "\n" __VA_ARGS__);                      \
        fprintf(stderr, "\n\ton line %d in func %s\n" RESET, __LINE__, __func__);   \
        exit(error_code);                                                           \
    }

#define RET_ASSERT(condition, error_code, ...)              \
    if (!(condition)) {                                     \
        fprintf(stderr, RED "ERROR:" WHITE, ##__VA_ARGS__); \
        fprintf(stderr, "\n" RESET);                        \
        return error_code;                                  \
    }

#endif // SASSERT_H