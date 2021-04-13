#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <setjmp.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// https://www.cs.rutgers.edu/~pxk/416/notes/c-tutorials/pipe.html#:~:text=A%20pipe%20is%20a%20system,(stuff%20to%20be%20read)
// https://stackoverflow.com/questions/646241/c-run-a-system-command-and-get-output
int main() {
    char prog[128];
    char cmd[256];
    char line[256];
    int err;
    FILE *fp;
    printf("Enter the name of the program that valgrind will run:\n");
    printf("valgrind --tool=lackey --trace-mem=yes ");
    scanf("%s",prog);
    snprintf(cmd,sizeof(cmd),"valgrind --tool=lackey --trace-mem=yes %s",prog);
    //printf("Your command is:\n");
    //printf("%s\n",cmd);
    
    fp = popen(cmd,"r");
    if (fp == NULL) {
        printf("Failed to run valgrind\n");
        return 1;
    }
    while(fgets(line,sizeof(line),fp) != NULL) {
        printf("%s",line);
    }
    err = pclose(fp);
    if(err < 0) {
        printf("Failed to close the file pointer\n");
        return 1;
    }
    
    return 0;
}