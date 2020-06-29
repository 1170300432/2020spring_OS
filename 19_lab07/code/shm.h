#ifndef _SHM_H
#define _SHM_H

typedef struct shm_ds
{
    unsigned int key;
    unsigned int size;
    unsigned long page;
}shm_ds;

#endif
