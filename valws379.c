#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <setjmp.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>

sigjmp_buf state;

void catch() {
    siglongjmp(state,1);
}


// Return true if the proc folder name i
int isnumber(char * fname) {
    for(int i=0;i<strlen(fname);i++) {
        if(!isdigit(fname[i])) {return 0;}
    }
    return 1;
}

// https://c-for-dummies.com/blog/?p=3246
int main() {
    DIR *procfolder;
    struct dirent *proc;
    int procfile;
    FILE *status;
    char fname[269];
    char pname[256];

    (void) signal(SIGSEGV, catch);
    if(sigsetjmp(state,1)) {
        closedir(procfolder);
    }

    procfolder = opendir("/proc/");
    printf("A\n");
    while(1) {
        procfile = 0;
        
        while(proc = readdir(procfolder)) {
            procfile++;
            snprintf(fname,sizeof(fname),"/proc/%s/status",proc->d_name);
            if(isnumber(proc->d_name)) {
                //printf("%s\n",proc->d_name);
                status = fopen(fname,"r");
                fgets(pname,sizeof(pname),status);
                if(strstr(pname, "valgrind") != NULL) {
                    printf("%s: %s\n",proc->d_name,pname);
                }
                fclose(status);
            }
        }
        //closedir(procfolder);
    }
    return 0;
}