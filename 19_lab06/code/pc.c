#define __LIBRARY__
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<linux/sem.h>
#include<signal.h>
#include<sys/types.h>
#include<fcntl.h> 
 
#define producer_N 1
#define consumer_N 5
#define Buffer_N 10
#define Buffer_M 500

_syscall2(int, sem_open, const char*, name, unsigned int, value);
_syscall1(int, sem_wait, sem_t*, sem);
_syscall1(int, sem_post, sem_t*, sem);
_syscall1(int, sem_unlink, const char*, name);

int buf_in, buf_out, fd;
sem_t *empty, *full, *mutex;

void producer()
{
	int item_num = 0;
        while(item_num < Buffer_M)
	{
		sem_wait(empty);
		sem_wait(mutex);
		if (lseek(fd, buf_in * sizeof(int), SEEK_SET) < 0)
                	fprintf(stderr, "Error in producer lseek\n");
                write(fd, (char*)&item_num, sizeof(int));
                buf_in = (buf_in + 1) % Buffer_N;
                sem_post(mutex);
		sem_post(full);
                item_num++;
	}
        close(fd); 
}

void consumer()
{
	int item_num, limit, i;
        limit = Buffer_M / consumer_N;
        for(i = 0; i < limit; i++)
        {
		sem_wait(full);
		sem_wait(mutex);
		lseek(fd, Buffer_N * sizeof(int), SEEK_SET);
                read(fd, (char*)&buf_out, sizeof(int));
                lseek(fd, buf_out * sizeof(int), SEEK_SET);
                read(fd, (char*)&item_num, sizeof(int));
                buf_out = (buf_out + 1) % Buffer_N;
                lseek(fd, Buffer_N * sizeof(int), SEEK_SET);
                write(fd, (char*)&buf_out, sizeof(int));
                printf("%d: %d\n", getpid(), item_num);
                fflush(stdout);
		sem_post(mutex);
                sem_post(empty);		
	}
        close(fd);
}

int main()
{
	char *_empty = "_empty";
        char *_full = "_full";
        char *_mutex  = "_mutex";
       
        pid_t ret1[producer_N], ret2[consumer_N];
        int i, status, item_num;
        buf_in = 0;
        buf_out = 0;

        fd = open("share_buffer", O_CREAT | O_RDWR | O_TRUNC, 0666);
        lseek(fd, Buffer_N * sizeof(int), SEEK_SET);
        write(fd, (char*)&buf_out, sizeof(int));
	empty = sem_open(_empty, Buffer_N);
	full = sem_open(_full, 0);
        mutex = sem_open(_mutex, 1);
        
	for(i = 0; i < producer_N; i++) {
		if((ret1[i]= fork()) == 0) {
                        producer();
                        return 0;
                }
                else if(ret1[i] < 0) {
		        printf("producer %d failed to create! \n", i);
			return -1;
                }
	}
	for(i = 0; i < consumer_N; i++) {
		if((ret2[i] = fork())== 0) {
                        consumer();
                        return 0;
                }
                else if(ret2[i] < 0) {
		        printf("consumer %d failed to create! \n", i);
			return -1;
                }    
	}
	for(i = 0; i < producer_N; i++)
		waitpid(ret1[i],&status,0);
	for(i = 0; i < consumer_N; i++)
		waitpid(ret2[i],&status,0);
        sem_unlink(_empty);
        sem_unlink(_full);
        sem_unlink(_mutex);
        close(fd);
        return 0;
}
