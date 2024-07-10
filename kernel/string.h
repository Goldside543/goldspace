#ifndef STRING_H
#define STRING_H

#include <stddef.h>

char *strtok(char *str, const char *delim);
int strcmp(const char *s1, const char *s2);
size_t strspn(const char *s, const char *accept);
char *strpbrk(const char *s, const char *accept);

#endif // STRING_H
