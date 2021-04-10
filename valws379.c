#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <setjmp.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>

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

// Function to sort list in descending order, numbers first
int numorder (const struct dirent **a, const struct dirent **b) {
    int n1 = isnumber((*a)->d_name);
    int n2 = isnumber((*b)->d_name);
    if(n1==1 && n2==1) { // both are numbers
        int na = atoi((*a)->d_name);
        int nb = atoi((*b)->d_name);
        if(na > nb) {return 1;}
        else if(na<nb){return -1;}
        else{return 0;}
    }
    // a is a number but b is a string
    else if (n1==1 && n2==0) {return 1;}

    // a is a string but b is a number
    else if (n1==0 && n2 == 1) { return -1;}

    else { // both are strings
        return strcoll ((*a)->d_name, (*b)->d_name);
    }
}


int read_procs() {
    DIR *procfolder;
    struct dirent *proc;
    int procfile;
    FILE *status;
    char fname[269];
    char pname[256];
    /*
    (void) signal(SIGINT, catch);
    if(sigsetjmp(state,1)) {
        return 0;
    }
    */
    (void) signal(SIGSEGV,catch);
    if(sigsetjmp(state,1)) {
        return 1;
    }

    while(1){
        /*
        procfile = 0;
        procfolder = opendir("/proc/");
        while(proc = readdir(procfolder)) {
            procfile++;
            snprintf(fname,sizeof(fname),"/proc/%s/status",proc->d_name);
            if(isnumber(proc->d_name)) {
                status = fopen(fname,"r");
                fgets(pname,sizeof(pname),status);
                if(strstr(pname, "valgrind") != NULL) {
                    printf("%s: %s\n",proc->d_name,pname);
                }
                fclose(status);
            }
        }
        closedir(procfolder);
        */
        struct dirent **namelist;
        int n;
        int p;

        n = scandir("/proc", &namelist,NULL,numorder);
        if (n < 0)
            perror("scandir");
        else {
            p = n-30;
            while (n>p) {
                n--;
                if(isnumber(namelist[n]->d_name)) {
                    snprintf(fname,sizeof(fname),"/proc/%s/status",namelist[n]->d_name);
                    status = fopen(fname,"r");
                    fgets(pname,sizeof(pname),status);
                    if(strstr(pname, "valgrind") != NULL) {
                        printf("%s\n",pname);
                    }
                    fclose(status);
                }
                free(namelist[n]);
            }
            free(namelist);
        }
    }
}


// https://c-for-dummies.com/blog/?p=3246
// https://stackoverflow.com/questions/26552503/print-directories-in-ascending-order-using-scandir-in-c
// https://stackoverflow.com/questions/66047191/how-to-sort-file-names-in-alphanumeric-order-in-c
int main() {
    int err = 1;
    while(err == 1) {
        err = read_procs();
    }

    return err;
}