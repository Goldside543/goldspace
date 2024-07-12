#include <stddef.h>
#include <stdint.h>
short my_strcspn(const char *s1, const char *s2);

char *my_strpbrk(const char *str1, const char *str2) {
    const char *sc1;
    const char *sc2;

    for (sc1 = str1; *sc1 != '\0'; ++sc1) {
        for (sc2 = str2; *sc2 != '\0'; ++sc2) {
            if (*sc1 == *sc2)
                return (char *)sc1;
        }
    }
    return NULL;  /* terminating nulls match */
}

short my_strlen(const char *__s){
    short i = 0;
    while(*__s){
        i++;
        __s++;
    }
    return i;
}

int my_strcmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1==*s2))
        s1++,s2++;
    return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

char *my_strcat(char *dest, const char *src)
{
    char *ret = dest;
    while (*dest)
        dest++;
    while (*dest++ = *src++);
    return ret;
}

char *my_strchr(const char *s, int c)
{
    while (*s != (char)c)
        if (!*s++)
            return 0;
    return (char *)s;
}

char *my_strcpy(char *dest, const char* src)
{
    char *ret = dest;
    while (*dest++ = *src++);
    return ret;
}

short my_strspn(const char *s1, const char *s2)
{
    short ret=0;
    while(*s1 && my_strchr(s2,*s1++))
        ret++;
    return ret;    
}

char *my_strtok(char *str, const char *delim) {
    static char *lasts;
    register int ch;

    if (str == 0)
        str = lasts;
    do {
        if ((ch = *str++) == '\0')
            return 0;
    } while (my_strchr(delim, ch));
    --str;
    lasts = str + my_strcspn(str, delim);
    if (*lasts != 0)
        *lasts++ = 0;
    return str;
}

short my_strcspn(const char *s1, const char *s2)
{
    short ret=0;
    while(*s1)
        if(my_strchr(s2,*s1) != NULL)
            return ret;
        else
            s1++,ret++;
    return ret;
}
