#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <stdlib.h>
#include <stdio.h>

#define CALLOC_WITH_TYPE(num_of_elements, type) \
    (type *) calloc(num_of_elements, sizeof(type))

#define SMART_FREE(el) \
    if (el) free (el)  \

size_t GetFileSize(FILE *fp);
char *GetBufferFromFile(FILE *fp, size_t FileSize);
void *reallocate_array(void **array, size_t capacity, size_t new_bytes);
FILE *fopen_with_dirs(const char *filename, const char *mode);
int   create_parent_directories(const char *file_path);
int InArray(int *Array, int size, int value);

#endif // FILEHELPER_H