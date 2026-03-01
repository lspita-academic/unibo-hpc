// Ludovico Maria Spitaleri 0001114169

#ifndef ENV_H
#define ENV_H

#include <stdbool.h>

/**
 * Get string value from environment variable.
 */
char* get_env_string(const char* key, char* default_val);

/**
 * Get boolean value from environment variable.
 */
bool get_env_bool(const char* key, bool default_val);

#endif  // ENV_H
