#include <stddef.h>
#include <stdint.h>

char *strtok(char *str, const char *delim) {
    static char *last;
    if (str == NULL) str = last;
    if (str == NULL) return NULL;

    char *end = str + strspn(str, delim);
    if (*end == '\0') return NULL;

    char *start = end;
    end = strpbrk(start, delim);
    if (end) {
        *end = '\0';
        last = end + 1;
    } else {
        last = NULL;
    }
    return start;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

size_t strspn(const char *s, const char *accept) {
    const char *p;
    const char *a;
    size_t count = 0;

    for (p = s; *p != '\0'; ++p) {
        for (a = accept; *a != '\0'; ++a) {
            if (*p == *a) {
                ++count;
                break;
            }
        }
        if (*a == '\0') {
            return count;
        }
    }
    return count;
}

char *strpbrk(const char *s, const char *accept) {
    const char *a;
    for (; *s != '\0'; ++s) {
        for (a = accept; *a != '\0'; ++a) {
            if (*s == *a) {
                return (char *)s;
            }
        }
    }
    return NULL;
}
