#include <string.h>
#include <errno.h>
#include <asm/segment.h>

char namestring[24];

int sys_iam(const char * name){
    char c;
    int namelen = 0;
    while((c=get_fs_byte(name+namelen))!='\0'){
        namelen++;
    }
    if(namelen >= 24)
        return -EINVAL;
    for(int i = 0; i <= namelen; i++){
        c = get_fs_byte(name+i);
        namestring[i]=c;
    }
    return namelen;
}

int sys_whoami(char* name, unsigned int size){
    int namelen = strlen(namestring);
    if(namelen <= size){
        for(int i = 0; i <= namelen; i++){
            put_fs_byte(namestring[i], name + i);
        }
        return namelen;
    } else {
        return -EINVAL;
    }
}
