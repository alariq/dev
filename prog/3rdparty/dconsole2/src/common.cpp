#include "dconsole/common.h"

#if !defined(__WIN32__)

#include <stdio.h>
#include <ctype.h>
#include <string.h>

char *  myitoa( int value, char * str, int base )
{
    switch(base)
    {
    case 10:
        sprintf(str, "%d", value);
        break;
    case 8:
        sprintf(str, "%o", value);
        break;
    case 16:	
        sprintf(str, "%x", value);
        break;
    default:
        return str;	
    }

    return str;
}

int mystricmp(const char* str1, const char* str2)
{
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char* buf1 = new char[len1+1]; strcpy(buf1, str1); buf1[len1] = '\0';
    char* buf2 = new char[len2+1]; strcpy(buf2, str2); buf2[len2] = '\0';
    for(size_t i=0;i<len1;++i) buf1[i] = tolower(buf1[i]);
    for(size_t i=0;i<len2;++i) buf2[i] = tolower(buf2[i]);
    int rv = strcmp(buf1, buf2);
    delete[] buf1; 
    delete[] buf2;
    return rv;		
}

#endif