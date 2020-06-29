#define __LIBRARY__
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h> 
#include<sys/wait.h>
#include<linux/sem.h>

_syscall2(int, shmget, int, key, size_t, size);
_syscall1(int, shmat,  int, shmid);
_syscall2(int, sem_open, const char*, name, unsigned int, value);
_syscall1(int, sem_wait, sem_t*, sem);
_syscall1(int, sem_post, sem_t*, sem);
_syscall1(int, sem_unlink, const char*, name);

#define Buffer_N 10
#define Buffer_M 500

sem_t *empty, *full, *mutex;

int main()
{
	char *_empty = "_empty";
        char *_full = "_full";
        char *_mutex = "_mutex";
       
        int *buf;
        int i, item_num, shmid, buf_out = 0;

	empty = sem_open(_empty, Buffer_N);
	full = sem_open(_full, 0);
        mutex = sem_open(_mutex, 1);
        
        shmid = shmget(1314, Buffer_N);
        printf("shmid:%d\n", shmid);
        fflush(stdout);
        if(shmid == -1){
            fprintf(stderr, "Error to open shm_ds!");
            return -1;
        }
        buf = (int*)shmat(shmid);
        printf("Begin to consume.\n");
        fflush(stdout);

        for(i = 0; i < Buffer_M; i++){
            sem_wait(full);
	    sem_wait(mutex);
	    item_num = buf[buf_out];
            buf_out = (buf_out+1)%Buffer_N;
	    sem_post(mutex);
            sem_post(empty);
            printf("pid: %d, consume %d.\n", getpid(), item_num);
            fflush(stdout);		
	}
        printf("Consume finished.\n");
        fflush(stdout);
	
        sem_unlink(_empty);
        sem_unlink(_full);
        sem_unlink(_mutex);

        return 0;
}
