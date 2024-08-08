/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef STRING_H
#define STRING_H

short my_strlen(const char *__s);
int my_strcmp(const char *s1, const char *s2);
char *my_strcat(char *dest, const char *src);
char *my_strchr(const char *str, int c);
char *my_strcpy(char *dest, const char* src);
short my_strspn(const char *s1, const char *s2);
char *my_strtok(char *str, const char *delim);
short my_strcspn(const char *s1, const char *s2);
char *my_strpbrk(const char *str1, const char *str2);
char* my_strncpy(char* dest, const char* src, size_t n);

#endif /* STRING_H */
