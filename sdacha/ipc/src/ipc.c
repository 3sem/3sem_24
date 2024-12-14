#include "ipc.h"
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <openssl/md5.h>


static int
open_file( const char* file_name)
{
    int fd = open( file_name, O_RDONLY, PERM);

    if ( fd == -1 )
    {
        printf( "%s", file_name);
        perror( "file open error");

        return -1;
    }

    return fd;
}

sem_t* 
get_sem( const char* sem_name)
{
    sem_t* sem = NULL;
    sem = sem_open( sem_name, O_CREAT | O_RDWR, PERM, 0);

    if ( sem == SEM_FAILED )
    {
        // Probably the semaphore is exist
        sem = sem_open( sem_name, O_RDWR, PERM, 0);

        if ( sem == SEM_FAILED )
        {
            printf( "sem_name = %s\n", sem_name);
            perror( "fatal error while opening semaphore");
        } 
            
    }

    return sem;
    
}

void* 
mmap_shm()
{
    int fd = shm_open( SHM_NAME, O_CREAT | O_RDWR, PERM);

    if ( fd == -1 )
    {
        perror( "fatal error while opening shared memory");
        return NULL;
    }

    if ( ftruncate( fd, getpagesize()) == -1 )
    {
        perror( "fatal error while ftruncate");
        return NULL;
    }

    void* shar_mem = mmap( NULL,       getpagesize(), PROT,
                           MAP_SHARED, fd,            0);

    if ( shar_mem == MAP_FAILED )
    {
        perror( "fatal error while mmap");
        return NULL;
    }
        

    return shar_mem; 
    
}

static void 
close_sem()
{
    sem_unlink( SEM_NAME1);
    sem_unlink( SEM_NAME2);
    sem_unlink( SEM_NAME3);

}




#define MD5_HASH_SIZE 16

static void 
write_hash( unsigned char hash[MD5_HASH_SIZE])
{
    for ( int i = 0 ; i < MD5_HASH_SIZE; i++ )
    {
        printf( "%02x", (unsigned int)hash[i]);
    }

    printf( "\n");
}

void 
test_shm( char* file_name)
{
    sem_t* sem_p = get_sem( SEM_NAME1);
    sem_t* sem_c = get_sem( SEM_NAME2);
    sem_t* sem_f = get_sem( SEM_NAME3);

    if ( sem_p == SEM_FAILED || 
         sem_c == SEM_FAILED ||
         sem_f == SEM_FAILED )
    {
        return;
    }

    char* shm_buf = (char*) mmap_shm();

    if ( shm_buf == NULL )
    {
        return;
    }

    pid_t pid = fork();

    if ( pid == -1 )
    {
        perror( "fork error");
        return;
    }

    if ( pid == 0 )
    {
        // use md5 to validation
        MD5_CTX md5_hash = { 0 };
        MD5_Init( &md5_hash); 

        while ( 1 )
        {
            sem_wait( sem_c); // wait for write

            int is_finish = 0;
            sem_getvalue( sem_f, &is_finish);

            if ( is_finish )
            {
                break; // the child process stoped reading from file
            }

            MD5_Update( &md5_hash, shm_buf, getpagesize());

            sem_post( sem_p); // signal that read is completed
        }

        unsigned char results[MD5_HASH_SIZE] = { 0 };
        MD5_Final( results, &md5_hash);
        write_hash( results);

        sem_post( sem_p);

        return;
    }
    
    else
    {
        int fd = open_file( file_name);

        if ( fd == -1 )
        {
            exit( -1); // kill child as well
        }

        while ( 1 )
        {
            ssize_t n_read = read( fd, shm_buf, getpagesize());

            if ( n_read == -1 )
            {
                perror( "read() error");
                exit( -1); // exit because we in parent proccess
            }

            sem_post( sem_c); // wait for reading
            sem_wait( sem_p);

            if ( n_read != getpagesize() )
            {
                munmap( shm_buf, getpagesize());
                sem_post( sem_f); // signal of finishing
                sem_post( sem_c);
                sem_wait( sem_p);

                close_sem();
                shm_unlink( SHM_NAME);

                break;
            }

        }
        

    }
    
}


void 
test_pipe( char* file_name)
{   
    int fds[2] = { 0 };

    if ( pipe( fds) < 0 )
    {
        perror("pipe error");
        return;
    }


    pid_t pid = fork();

    if ( pid == 0 )
    {
        close( fds[1]); // close write pipe
        char* buffer = calloc( getpagesize(), sizeof(char));

        if ( buffer == NULL )
        {
            printf( "Memory allocation error\n");
            return;
        }

        MD5_CTX hash = { 0 };
        MD5_Init( &hash);

        while ( 1 )
        {
            ssize_t n_read = read( fds[0], buffer, getpagesize());

            if ( n_read == -1 )
            {
                perror( "child: read from pipe error");
                exit( -1); // exit to kill child
            }

            MD5_Update( &hash, buffer, getpagesize());

            if ( n_read != getpagesize() )
            {
                break;
            }
        }
    
        unsigned char results[16] = { 0 };

        MD5_Final( results, &hash);
        write_hash( results);
    }


    else 
    {
        close( fds[0]);

        int fd = open_file( file_name);

        if ( fd == -1 )
        {
            return;
        }

        char* buffer = calloc( getpagesize(), sizeof(char));

        if ( buffer == NULL )
        {
            printf( "Memory allocation error\n");
            return;
        }

        while ( 1 )
        {
            ssize_t n_read = read( fd, buffer, getpagesize());

            if ( n_read == -1 )
            {
                perror( "parent: read from file error");
                return;
            }

            write( fds[1], buffer, n_read);

            if ( n_read != getpagesize() )
            {
                break;
            }
        }

        close( fd);

    }

}

static int 
get_msg()
{
    key_t id = ftok( MSG_NAME, 0);

    int msgid = msgget( id, IPC_CREAT | PERM);

    if ( msgid == -1 ) // probably exist
    {
        msgid = msgget( id, PERM);

        if ( msgid == -1 )
        {
            perror( "Fatal error, msget returns -1");
        }
    }

    return msgid;
}

void
test_msg( char* file_name)
{
    int msgid = get_msg();
    sem_t* sem_fin = get_sem( SEM_NAME1);
    sem_t* sem_c = get_sem( SEM_NAME2);
    sem_t* sem_p = get_sem( SEM_NAME3);

    pid_t pid = fork();

    if ( pid == -1 )
    {
        perror( "fork error");
        return;
    }

    if ( pid == 0 )
    {
        char* buffer = calloc( getpagesize() + sizeof(long), sizeof(char));

        if ( buffer == NULL )
        {
            printf( "Memory allocation error\n");
            return;
        }

        MD5_CTX hash = { 0 };
        MD5_Init( &hash);

        while ( 1 )
        {
            sem_wait( sem_c);

            if ( msgrcv( msgid, buffer, getpagesize(), 0, 0) == -1 )
            {
                perror( "msgrcv error");
                free( buffer);
                return;
            }

            int is_finish = 0;
            sem_getvalue( sem_fin, &is_finish);

            if ( is_finish )
            {
                break;
            }

            MD5_Update( &hash, buffer + sizeof(long), getpagesize());
            sem_post( sem_p);

        }

        free( buffer);
        unsigned char results[16] = { 0 };

        MD5_Final( results, &hash);
        write_hash( results);
        sem_post( sem_p); // signal to finish
    }

    else 
    {
        int fd = open_file( file_name);

        if ( fd == -1 )
        {
            return;
        }

        char* buffer = calloc( getpagesize() + sizeof(long), sizeof(char));
        *(long*)buffer = MSG_TYPE;

        if ( buffer == NULL )
        {
            printf( "Memory allocation error\n");
            return;
        }

        while ( 1 )
        {
            ssize_t n_read = read( fd, buffer + sizeof(long), getpagesize());

            if ( n_read == -1 )
            {
                perror( "read error");
                break;
            }

            if ( msgsnd( msgid, buffer, n_read, 0) == -1 )
            {
                perror( "msgsnd error");
                break;
            }
            
            sem_post( sem_c);
            sem_wait( sem_p);

            if ( n_read != getpagesize() )
            {
                sem_post( sem_fin); // set sem_fin to 1
                msgsnd( msgid, buffer, 1, 0); // avoid infinity receiving loop
                sem_post( sem_c);
                sem_wait( sem_p);
                
                break;
            }
        }
 
        msgctl( msgid, IPC_RMID, NULL);
        close_sem();
        free( buffer);
        close( fd);
    }
}

