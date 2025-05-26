#pragma once


#if defined(__cplusplus)
    extern "C" {
#endif


typedef struct Functions
{
    void    (*PostCommand)(int command, const char* strParam1, float fltParam1, int intParam1);
} Functions;


typedef int     (*InitProc)(Functions* functions);
typedef void    (*StartProc)(const char* startMap);
typedef void    (*DestoryProc)(void);


#if defined(__cplusplus)
    }
#endif
