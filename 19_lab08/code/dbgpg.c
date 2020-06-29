#include <linux/mm.h>
#include <linux/sched.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <errno.h>
#include <fcntl.h>

int sys_debug_paging(pid_t pid, void *address, const char *logPath){
    int i, j = -1;
    for(i = 0; i < NR_TASKS; i++){
        if(task[i]->pid == pid){
            j = i;
            break;
        }
    }
    int fd = 1;
    if(logPath != NULL){
        fd = open(logPath, O_CREAT|O_APPEND, 0666);
    }
    if(fd < 0){
        printk("\nFailed to open a file!\n");
        printk("\n---------------------------------\n");
        errno = EINVAL;
        return -1;
    }
    if(j == -1){
        fprintk(fd, "\nPid is not exist!\n");
        fprintk(fd, "\n---------------------------------\n");
        errno = EINVAL;
        return -1;
    }
    int linearBaseAddress = task[j]->start_code;
    fprintk(fd, "\nPid: %d Task: %d linearBaseAddress: 0x%08X\n", pid, j, linearBaseAddress);
    if(address != NULL){
        int linearAddress = linearBaseAddress + address;
        fprintk(fd, "\nlinearAddress: 0x%08X\n", linearAddress);
        int dirIndex = (linearAddress >> 22) & 0x3FF;
        int tableIndex = (linearAddress >> 12) & 0x3FF;
        int offset = linearAddress & 0xFFF;
        fprintk(fd, "\ndirIndex\ttableIndex\toffset\n");
        fprintk(fd, "0x%06X\t0x%06X\t0x%06X\n", dirIndex, tableIndex, offset);
        int pageDirectoryTable = (task[j]->tss).cr3;
        int tableAddress = (*(int *)(pageDirectoryTable + dirIndex * 4)) & 0xFFFFF000;
        int pageAddress = (*(int *)(tableAddress + tableIndex * 4)) & 0xFFFFF000;
        int physicalAddress = pageAddress + offset;
        fprintk(fd, "\ntableAddress\tpageAddress\tphysicalAddress\n");
        fprintk(fd, "0x%08X\t0x%08X\t0x%08X\n", tableAddress, pageAddress, physicalAddress);
    } else {
        //int size = task[j]->brk;
        int dirIndex = (linearBaseAddress >> 22) & 0x3FF;
        int pageDirAddress = (task[j]->tss).cr3;
        int tableAddress;
        int dirIndexArray[20];
        int tableAddressArray[20];
        int P;
        i = 0;
        for(j = 0; j < 16; j++){
            P = (*(int *)(pageDirAddress + dirIndex * 4)) & 0x1;
            if(P) {
                tableAddress = (*(int *)(pageDirAddress + dirIndex * 4)) & 0xFFFFF000;
                dirIndexArray[i] = dirIndex;
                tableAddressArray[i] = tableAddress;
                i++;
            }
            dirIndex += 1;
        }
        fprintk(fd,"\nIndex\ttableAddress\n");
        for(j = 0; j < i; j++) {
            fprintk(fd, "0x%X\t0x%08X\n", dirIndexArray[j], tableAddressArray[j]);
        }
        int pageAddress;
        int tableIndex;
        for(j = 0; j < i; j++) {
            fprintk(fd, "\nTable %08X:\n", tableAddressArray[j]);
            fprintk(fd, "\tIndex\tPageAddress\n");
            for(tableIndex = 0; tableIndex < 1024; tableIndex++) {
            	pageAddress = (*(int *)(tableAddressArray[j] + tableIndex * 4)) & 0xFFFFF000;
                P = (*(int *)(tableAddressArray[j] + tableIndex * 4)) & 0x1;
                if(P){
                    fprintk(fd,"\t0x%X\t0x%08X\n", tableIndex, pageAddress);
                }
            }
        }
    }
    fprintk(fd, "\n---------------------------------\n");
    if(fd != 1){
        close(fd);
    }
    return 0;
}
