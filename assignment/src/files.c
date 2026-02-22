// Ludovico Maria Spitaleri 0001114169

#include "files.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "abort.h"
#include "memory.h"

FILE* safe_fopen(char* path, char* mode) {
    FILE* file = fopen(path, mode);
    safe_assert(
        file != NULL, "Failed to open file %s in %s mode\n", path, mode
    );
    return file;
}

void safe_fclose(FILE* file) {
    int result = fclose(file);
    safe_assert(result == 0, "Failed to close file: %s\n", strerror(errno));
}

size_t read_file_line(FILE* stream, char** out_buffer) {
#define READLINE_START_CAPACITY 1024
#define READLINE_CAPACITY_GROWTH_FACTOR 2
    size_t total_len = 0;
    size_t capacity = READLINE_START_CAPACITY;
    char* row_buff = out_buffer == NULL || *out_buffer == NULL
                         ? safe_malloc(capacity)
                         : *out_buffer;

    /*
     * `fgets` could stop before then newline/EOF if the buffer is not large
     * enough.
     *
     * From `fgets` man pages:
     * fgets() reads in at most one less than size characters from stream
     * and stores them into the buffer  pointed to  by s. Reading stops
     * after an EOF or a newline. If a newline is read, it is stored into
     * the buffer. A terminating null byte ('\0') is stored after the last
     * character in the buffer.
     */
    while (fgets(row_buff + total_len, capacity - total_len, stream) != NULL) {
        size_t n_read = strlen(row_buff + total_len);
        total_len += n_read;

        if (total_len > 0 && row_buff[total_len - 1] == '\n') {
            break;
        }

        capacity *= READLINE_CAPACITY_GROWTH_FACTOR;
        row_buff = safe_realloc(row_buff, capacity);
    }

    if (total_len > 0) {
        *out_buffer = row_buff;
    }

    return total_len;
}
