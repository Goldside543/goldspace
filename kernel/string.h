#ifndef STRING_H
#define STRING_H

#include <stddef.h>

char *my_strtok(char *str, const char *delim);
int my_strcmp(const char *s1, const char *s2);
size_t my_strspn(const char *s, const char *accept);
char *my_strpbrk(const char *s, const char *accept);

#endif // STRING_H
