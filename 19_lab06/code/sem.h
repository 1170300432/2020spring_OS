# ifndef _SEM_H_
# define _SEM_H_

#include <linux/sched.h> 

typedef struct sem_t{
    char name[20];
    int value;
    struct task_struct *queue;
}sem_t;

#endif