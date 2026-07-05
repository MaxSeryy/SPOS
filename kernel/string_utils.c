#include "string_utils.h"

int os_strcmp(const char *s1, const char *s2) {
    // logic: while chars are equal and not end of string, move forward
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

char *os_split_arg(char *cmd) {
    while (*cmd) {
        if (*cmd == ' ') {
            *cmd = '\0'; // cut
            return cmd + 1;
        }
        cmd++;
    }
    return 0;
}