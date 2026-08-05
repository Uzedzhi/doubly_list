#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>

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

#ifdef _WIN32
    #include <direct.h>
    #define mkdir(path, mode) _mkdir(path)
    #define PATH_SEP '\\'
#else
    #define PATH_SEP '/'
#endif

int create_parent_directories(const char *file_path) {
    char temp_path[512] = {};
    snprintf(temp_path, sizeof(temp_path) - 1, "%s", file_path);

    size_t len = strlen(temp_path);
    if (len == 0) return -1;

    char *p = strrchr(temp_path, PATH_SEP);
    if (p == NULL) return 0;
    *p = '\0'; 

    char *next_sep = strchr(temp_path, PATH_SEP);
    while (next_sep) {
        *next_sep = '\0';
        mkdir(temp_path, 0777);
        *next_sep = PATH_SEP;

        next_sep = strchr(next_sep + 1, PATH_SEP);
    }
    
    mkdir(temp_path, 0777);

    return 0;
}

FILE *fopen_with_dirs(const char *filename, const char *mode) {
    if (strchr(mode, 'w') || strchr(mode, 'a')) {
        if (create_parent_directories(filename) != 0) {
            return NULL; 
        }
    }
    return fopen(filename, mode);
}

int InArray(int *Array, int size, int value) {
    for (int i = 0; i < size; i++) {
        if (Array[i] == value)
            return 1;
    }
    return 0;
}