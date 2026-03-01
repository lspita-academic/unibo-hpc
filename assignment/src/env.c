// Ludovico Maria Spitaleri 0001114169

#include "env.h"

#include <stdlib.h>
#include <string.h>

char* get_env_string(const char* key, char* default_val) {
    char* value = getenv(key);
    return value == NULL ? default_val : value;
}

bool get_env_bool(const char* key, bool default_val) {
    char* value = get_env_string(key, NULL);
    if (value == NULL) {
        return default_val;
    } else {
        return strcmp(value, "false") != 0 && strcmp(value, "0") != 0;
    }
}
