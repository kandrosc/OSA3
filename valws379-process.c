#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int get_reference_size(char * line) {
    int i;
    char num_str[3];
    for(i=0;i<strlen(line);i++) {
        if(line[i] == 44) {
            break;
        }
    }
    num_str[0] = line[i+1];
    if(line[i+2] >= 48 && line[i+2] <= 57) {
        num_str[1] = line[i+2];
        num_str[2] = '\0';
    }
    else {
        num_str[1] = '\0';
    }
    return atoi(num_str);
}

// https://www.cs.rutgers.edu/~pxk/416/notes/c-tutorials/pipe.html#:~:text=A%20pipe%20is%20a%20system,(stuff%20to%20be%20read)
// https://stackoverflow.com/questions/646241/c-run-a-system-command-and-get-output
int main(int argc, char const *argv[]) {
    char *endPtr;
    unsigned long long windowsize = strtoull(argv[1],&endPtr,10);
    long page_size = strtol(argv[2],&endPtr,10);
    long skipsize = strtol(argv[3],&endPtr,10);
    char cmd[256];
    char line[1024];
    char * line_ptr;
    int reference_size;
    int err;
    FILE *fp;

    unsigned long long w_counter = 0;
    int p_counter = 0;

    int num_windows = 0;
    int num_pages = 0;

    int skipline = 0; // Boolean flag that tells us we are skipping the line

    snprintf(cmd,sizeof(cmd),"valgrind --tool=lackey --trace-mem=yes %s 2>&1",argv[4]);

    
    fp = popen(cmd,"r");
    if (fp == NULL) {
        printf("Failed to run valgrind\n");
        return 1;
    }
    line_ptr = &line[0];
    printf("0,0\n");
    while(fgets(line,sizeof(line),fp) != NULL) {
        for(int i=0;i<strlen(line);i++) {
            if(line[i] == 61) {
                skipline = 1;
                break;
            }
        }
        if(skipline == 1) {
            skipline = 0;
            continue;
        }
        reference_size = get_reference_size(line_ptr);
        p_counter = p_counter + reference_size;
        if(p_counter > page_size) {
            p_counter = p_counter - page_size;
            num_pages++;
        }

        w_counter = (w_counter + 1) % windowsize;
        if(w_counter == 0) {
            num_windows ++;
            printf("%d,%d\n",num_windows,num_pages);
            num_pages = 0;
        }
    }

    err = pclose(fp);
    if(err < 0) {
        printf("Failed to close the file pointer\n");
        return 1;
    }
    return 0;
}