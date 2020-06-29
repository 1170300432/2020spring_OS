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
        char *_mutex  = "_mutex";
       
        int *buf;
        int i, item_num, shmid, buf_in = 0;

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
        printf("Begin to produce.\n");
        fflush(stdout);

        for(item_num = 0; item_num < Buffer_M; item_num++){
	    sem_wait(empty);
	    sem_wait(mutex);
            buf[buf_in] = item_num;
            printf("produce %d.\n", item_num);
            fflush(stdout);
            buf_in = (buf_in+1)%Buffer_N;
            sem_post(mutex);
	    sem_post(full);
	}
        printf("Produce finished.\n");
        fflush(stdout);
	
        return 0;
}
