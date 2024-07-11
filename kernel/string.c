#include <stddef.h>
#include <stdint.h>

size_t my_strspn(const char *s, const char *accept) {
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

char *my_strpbrk(const char *s, const char *accept) {
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

char *my_strtok(char *str, const char *delim) {
    static char *last = NULL;
    if (str != NULL) {
        last = str;
    }
    if (last == NULL) {
        return NULL;
    }

    char *start = last + my_strspn(last, delim);
    if (*start == '\0') {
        last = NULL;
        return NULL;
    }

    char *end = my_strpbrk(start, delim);
    if (end) {
        *end = '\0';
        last = end + 1;
    } else {
        last = start + strlen(start);
    }
    return start;
}

int my_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}
