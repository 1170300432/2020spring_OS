#include <linux/sem.h>
#include <asm/segment.h>
#include <unistd.h>
#include <asm/system.h>
#include <linux/kernel.h>

#define SEMS_SIZE 5
#define MAX_NAME 20

sem_t sems[SEMS_SIZE] = {
    {"", 0, NULL},
    {"", 0, NULL},
    {"", 0, NULL},
    {"", 0, NULL},
    {"", 0, NULL},
};

sem_t * sys_sem_open(const char *name, unsigned int value){
    int i, index = -1;
    char temp_name[MAX_NAME];
    for(i = 0; i < MAX_NAME; i++){
        temp_name[i] = get_fs_byte(name+i);
        if(temp_name[i] == '\0')
            break;
    }

    if(i == 0 || i == MAX_NAME){
        printk("The length of name is unsuitable!\n");
        return NULL;
    }

    for(i = 0; i < SEMS_SIZE; i++){
        if(strcmp(sems[i].name, "") == 0){
            index = i;
            break;
        }
        else if(strcmp(sems[i].name, temp_name) == 0)
            return &sems[i];
    }

    sem_t *res = NULL;
    if(index != -1){
        res = &sems[index];
        strcpy(res->name, temp_name);
        res->value = value;
    }
    else
        printk("No empty slots!\n");
    return res;
}

int sys_sem_wait(sem_t *sem){
    cli();
    if(sem == NULL || sem < sems || sem >= sems + SEMS_SIZE){
        sti();
        return -1;
    }
    while(sem->value == 0)
        sleep_on(&sem->queue);
    sem->value--;
    sti();
    return 0;
}

int sys_sem_post(sem_t *sem){
    if(sem == NULL || sem < sems || sem >= sems + SEMS_SIZE)
        return -1;
    cli();
    wake_up(&sem->queue);
    sem->value++;
    sti();
    return 0;
}

int sys_sem_unlink(const char *name){
    int i; 
    char temp_name[MAX_NAME];
    for(i = 0; i < MAX_NAME; i++){
        temp_name[i] = get_fs_byte(name+i);
        if(temp_name[i] == '\0')
            break;
    }
    if(i == 0 || i == MAX_NAME)
        return -1;
    for(i = 0; i < SEMS_SIZE; i++){
        if(strcmp(sems[i].name, temp_name) == 0){
            sems[i].name[0] = '\0';
            sems[i].value = 0;
            sems[i].queue = NULL;
            return 0;
        }
    }
    return -1;
}
