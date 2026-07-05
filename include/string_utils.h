#ifndef STRING_UTILS_H
#define STRING_UTILS_H

// small freestanding string helpers (no libc available, -nostdlib/-ffreestanding)

int os_strcmp(const char *s1, const char *s2);

// splits a command buffer on the first space: writes '\0' in place of the
// space and returns a pointer to the argument that follows, or NULL if
// there was no argument. Mutates the input buffer.
char *os_split_arg(char *cmd);

#endif // STRING_UTILS_H