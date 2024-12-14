
#include <unistd.h>
#include <semaphore.h>

#define SEM_NAME1 "/milfhunter1"
#define SEM_NAME2 "/milfhunter2"
#define SEM_NAME3 "/milfhunter3"
#define SHM_NAME "/milfhunter993"
#define MSG_NAME "/milfhunter!!!!"
#define MSG_TYPE 993
#define MSG_FINAL 100
#define PERM 0644 
#define PROT PROT_READ | PROT_WRITE

sem_t* get_sem( const char* sem_name);

void* mmap_shm();

void test_shm( char* file_name);

void test_pipe( char* file_name);

void test_msg( char* file_name);
