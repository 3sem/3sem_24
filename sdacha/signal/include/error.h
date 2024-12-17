#ifndef ERROR_H
#define ERROR_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


#define location()                                              \
        do                                                      \
        {                                                       \
                printf( "ERROR: %s:%d\n", __FILE__, __LINE__);  \
        }                                                       \
        while ( 0 )

#define handle( err, ...)                                       \
        do                                                      \
        {                                                       \
                int res = (err);                                \
                                                                \
                if ( res == -1 )                                \
                {                                               \
                        if ( errno != 0 )                       \
                        {                                       \
                                perror( #err);                  \
                                errno = 0;                      \
                        }                                       \
                                                                \
                        location();                             \
                        __VA_ARGS__;                            \
                        return -1;                              \
                }                                               \
        }                                                       \
        while ( 0 )

#define handlep( err, ...)                                      \
        do                                                      \
        {                                                       \
                void *res = (err);                              \
                                                                \
                if ( res == (void *)-1 )                        \
                {                                               \
                        if ( errno != 0 )                       \
                        {                                       \
                                perror( #err);                  \
                                errno = 0;                      \
                        }                                       \
                                                                \
                        location();                             \
                        __VA_ARGS__;                            \
                        return -1;                              \
                }                                               \
        }                                                       \
        while ( 0 )

#define handlen( err, ...)                                      \
        do                                                      \
        {                                                       \
                void *res = (err);                              \
                                                                \
                if ( res == NULL )                              \
                {                                               \
                        if ( errno != 0 )                       \
                        {                                       \
                                perror( #err);                  \
                                errno = 0;                      \
                        }                                       \
                                                                \
                        location();                             \
                        __VA_ARGS__;                            \
                        return -1;                              \
                }                                               \
        }                                                       \
        while ( 0 )


#endif // ERROR_H
