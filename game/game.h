#pragma once


#if defined(__cplusplus)
    extern "C" {
#endif


typedef struct Functions
{
    void    (*PostCommand)(int command, const char* strParam1, float fltParam1, int intParam1);
} Functions;


#define INIT_OK 0


typedef int     (*InitProc)(Functions* functions);
typedef void    (*StartProc)(const char* startMap);
typedef void    (*CollisionProc)(int entityIdx);
typedef void    (*DestroyProc)(void);


#if defined(__cplusplus)
    }
#endif
