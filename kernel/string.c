#include <stddef.h>
#include <stdint.h>

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
    while(*s1 && strchr(s2,*s1++))
        ret++;
    return ret;    
}

char *my_strtok(char * str, const char * delim)
{
    static char* p=0;
    if(str)
        p=str;
    else if(!p)
        return 0;
    str=p+strspn(p,delim);
    p=str+strcspn(str,delim);
    if(p==str)
        return p=0;
    p = *p ? *p=0,p+1 : 0;
    return str;
}

short my_strcspn(const char *s1, const char *s2)
{
    short ret=0;
    while(*s1)
        if(strchr(s2,*s1))
            return ret;
        else
            s1++,ret++;
    return ret;
}
