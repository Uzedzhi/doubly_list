#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#ifdef _ERROR_H
    #include <errno.h>

    #define __HELPER_FUNCS_ERR_ALLOC_FAIL       1
    #define __HELPER_FUNCS_ERR_REALLOC_FAIL     2
    #define __HELPER_FUNCS_ERR_STREAM_FAIL      3
    #define __HELPER_FUNCS_ERR_BUFFER_OVERFLOW  4
#endif

size_t GetFileSize(FILE *fp) {
    if (fseek(fp, 0, SEEK_END) == -1)
        return -1;

    size_t FileSize = ftell(fp);

    if (fseek(fp, 0, SEEK_SET) == -1)
        return -1;

    return FileSize;
}

char * GetBufferFromFile(FILE *fp, size_t FileSize) {
    char *FileBuffer = (char *) calloc(FileSize + 32, sizeof(char));
    if (!FileBuffer) {
        #ifdef _ERROR_H
            errno = __HELPER_FUNCS_ERR_ALLOC_FAIL;
        #endif
        return NULL;
    }

    int CharsRead = fread(FileBuffer, sizeof(char), FileSize, fp);
    if (ferror(fp)) {
        #ifdef _ERROR_H
            errno = __HELPER_FUNCS_ERR_STREAM_FAIL;
        #endif
        return NULL;
    }
    FileBuffer[CharsRead] = '\0';
    return FileBuffer;
}

void *reallocate_array(void **array, size_t capacity, size_t new_bytes) {
    if ((double) SIZE_MAX / (double) capacity <= (double) new_bytes / (double) capacity) {
        #ifdef _ERROR_H
            errno == __HELPER_FUNCS_ERR_BUFFER_OVERFLOW;
        #endif
        return NULL;
    }

    void *new_array = realloc(*array, new_bytes);
    if (new_array == NULL) {
        #ifdef _ERROR_H
            errno == __HELPER_FUNCS_ERR_REALLOC_FAIL;
        #endif
        return NULL;
    }

    *array = new_array;
    return *array;
}