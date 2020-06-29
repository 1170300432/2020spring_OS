/* test.c */
#define   __LIBRARY__
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

_syscall3(int, debug_paging, pid_t, pid, void *, address, const char *, logPath);

int i = 0x12345678;

int main(void)
{
    printf("The logical/virtual address of i is 0x%08x\n", &i);
    fflush(stdout);
    debug_paging(getpid(), &i, NULL);
    debug_paging(getpid(), NULL, "/usr/var/paging.log");
    return 0;
}