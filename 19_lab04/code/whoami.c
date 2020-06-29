#define __LIBRARY__
#include <unistd.h>
#include <stdio.h>
_syscall2(int, whoami, char*, name, unsigned int, size);

int main(){
    char name[24];
    whoami(name, 24);
    printf("%s\n",name);
    return 0;
}
