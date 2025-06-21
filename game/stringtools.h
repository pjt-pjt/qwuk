#pragma once

#if defined(STRINGTOOLS_IMPL)
    #include <string.h>
#endif

int    Equals(const char* left, const char* right);
int    StartsWith(const char* str, const char* subStr);


#if defined(STRINGTOOLS_IMPL)

    int    Equals(const char* left, const char* right)
    {
        return (strcmp(left, right) == 0);
    }

    int    StartsWith(const char* str, const char* subStr)
    {
        int len1 = strlen(str);
        int len2 = strlen(subStr);
        return (len2 <= len1 && strncmp(str, subStr, len2) == 0);
    }

#endif
