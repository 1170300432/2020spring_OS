#include <linux/shm.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <errno.h>

#define SHM_SIZE 3

shm_ds shm_list[SHM_SIZE] = {
    {0,0,0},
    {0,0,0},
    {0,0,0}
};

int sys_shmget(int key, size_t size){
        
        unsigned long page;
        
        if(key <= 0 || size > PAGE_SIZE || size <= 0){
            printk("Error! Illegal parameter!\n");
            errno = EINVAL;
            return -1;
        }

        int i;
        for(i = 0; i < SHM_SIZE; i++){
            if(shm_list[i].key == key)
                return i;
        }
        
        page = get_free_page();
        if(!page){
            printk("Error! Allocate failed!\n");
            errno = ENOMEM;
            return -1;
        }
        else{
            printk("Allocated a page at 0x%08x\n", page);
        }
 
        for(i = 0; i < SHM_SIZE; i++) {
            if(shm_list[i].key == 0) {
                shm_list[i].key = key;
                shm_list[i].size = size;
                shm_list[i].page = page; 
                return i;
            }
        }
        
        printk("Error! No empty slot!\n");
        errno = ENOMEM;
        return -1;
}

void *sys_shmat(int shmid) {
        unsigned long data_base, brk;
        if(shmid < 0 || shmid >= SHM_SIZE || shm_list[shmid].key == 0)
            return (void *)-EINVAL;
        data_base = get_base(current->ldt[2]);
        printk("Current data-segment's linear address: 0x%08x, page's pysical address: 0x%08x\n", data_base, shm_list[shmid].page);
        brk = current->brk + data_base;
        current->brk += PAGE_SIZE;
        if(put_page(shm_list[shmid].page, brk) == 0){
            errno = ENOMEM;
            return (void *)-1;
        }
        return (void *) (current->brk - PAGE_SIZE);
}
