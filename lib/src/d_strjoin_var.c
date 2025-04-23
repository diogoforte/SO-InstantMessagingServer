#include "../inc/libdiogo.h"

char *d_strjoin_var(char *first, ...) {
    if (!first)
        return NULL;
    va_list args;
    va_start(args, first);
    size_t total_len = d_strlen(first);
    char *curr_str;
    while ((curr_str = va_arg(args, char *)) != NULL) {
        total_len += d_strlen(curr_str);
    }
    va_end(args);
    char *result = malloc(total_len + 1);
    if (!result) {
        return NULL;
    }
    size_t pos = 0;
    size_t len = d_strlen(first);
    d_memcpy(result, first, len);
    pos += len;
    va_start(args, first);
    while ((curr_str = va_arg(args, char *)) != NULL) {
        len = d_strlen(curr_str);
        d_memcpy(result + pos, curr_str, len);
        pos += len;
    }
    result[pos] = '\0';
    va_end(args);

    return result;
}
