#include "../inc/libdiogo.h"

char *d_strjoin_var(char *first, ...)
{
    if (!first)
        return NULL;

    va_list args;
    va_start(args, first);
    va_list args_copy;
    va_copy(args_copy, args);
    size_t total_len = d_strlen(first);
    char *curr_str;
    while ((curr_str = va_arg(args_copy, char *)) != NULL) {
        total_len += d_strlen(curr_str);
    }
    va_end(args_copy);
    char *result = malloc(total_len + 1);
    if (!result) {
        va_end(args);
        return NULL;
    }
    size_t pos = 0;
    size_t len = d_strlen(first);
    d_memcpy(result, first, len);
    pos += len;
    while ((curr_str = va_arg(args, char *)) != NULL) {
        len = d_strlen(curr_str);
        d_memcpy(result + pos, curr_str, len);
        pos += len;
    }
    result[pos] = '\0';
    va_end(args);
    return result;
}