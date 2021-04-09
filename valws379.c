#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <setjmp.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>

#define EVENT_STRUCT_SIZE sizeof(struct inotify_event)
#define EVENT_BUFFER_SIZE (10 * (EVENT_STRUCT_SIZE + NAME_MAX + 1))
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

    (void) signal(SIGINT, catch);
    if(sigsetjmp(state,1)) {
        return 0;
    }
    int inotify_fd = inotify_init();
    if(inotify_fd < 0) {
        perror ("inotify_init");
        return 1; // can't create the inotify fd, return 1 to os and exit
    }
    int watch_des = inotify_add_watch(inotify_fd, "/proc",IN_CREATE);
    if(watch_des == -1) {
        perror ("inotify_add_watch");
        return 1; // can't create the watch descriptor, return 1 to os and exit
    }

    char buffer[EVENT_BUFFER_SIZE];
    while(1) {
        int bytesRead = read(inotify_fd, buffer, EVENT_BUFFER_SIZE), bytesProcessed = 0;
        if(bytesRead < 0) { // read error
            perror("read");
            return 1;
        }
        while(bytesProcessed < bytesRead) {
            struct inotify_event* event = (struct inotify_event*)(buffer + bytesProcessed);
            if (event->mask & IN_CREATE)
                if (event->mask & IN_ISDIR)
                    printf("%s IN_CREATE IN_ISDIR\n", event->name);
            bytesProcessed += EVENT_STRUCT_SIZE + event->len;
            }

        /*
        procfile = 0;
        procfolder = opendir("/proc/");
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
        closedir(procfolder);
        */
    }
    return 0;
}