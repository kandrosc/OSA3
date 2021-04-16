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

// I = read, S = store, L = load, M = modify
// https://www.cs.rutgers.edu/~pxk/416/notes/c-tutorials/pipe.html#:~:text=A%20pipe%20is%20a%20system,(stuff%20to%20be%20read)
// https://stackoverflow.com/questions/646241/c-run-a-system-command-and-get-output
int main(int argc, char const *argv[]) {
    unsigned long long windowsize;
    long page_size;
    long skipsize;
    char *endPtr;
    
    if(argc == 5) {
        if((strcmp(argv[1],"-s") == 0 || strcmp(argv[3],"-s") == 0 ) && !(strcmp(argv[1],"-s") == 0 && strcmp(argv[3],"-s") == 0)  ) {
            if(strcmp(argv[1],"-s") == 0) {
                windowsize = strtoull(argv[3],&endPtr,10);
                if(*endPtr) {
                    printf("Insufficient argument for <windowsize>: please enter a valid unsigned long long integer\n");
                    return 1;
                }
                page_size = strtol(argv[4],&endPtr,10);
                if(*endPtr) {
                    printf("Insufficient argument for <pagesize>: please enter an intger that is a power of 2 from 16 to 65536\n");
                    return 1;
                }
                if(page_size < 16 || page_size > 65536) {
                    printf("Insufficient argument for <pagesize>: please enter an intger that is between 16 to 65536\n");
                }
                if(page_size & (page_size -1) != 0) {
                    printf("Insufficient argument for <pagesize>: please enter an intger that is a power of 2\n");
                    return 1;
                }
                skipsize = strtol(argv[2],&endPtr,10);
                if(*endPtr) {
                    printf("Insufficient argument for <skipsize>: please enter a valid intger\n");
                    return 1;
                }
            }
            else if(strcmp(argv[3],"-s") == 0) {
                windowsize = strtoull(argv[1],&endPtr,10);
                if(*endPtr) {
                    printf("Insufficient argument for <windowsize>: please enter a valid unsigned long long integer\n");
                    return 1;
                }
                page_size = strtol(argv[2],&endPtr,10);
                if(*endPtr) {
                    printf("Insufficient argument for <pagesize>: please enter an intger that is a power of 2 from 16 to 65536\n");
                    return 1;
                }
                if(page_size < 16 || page_size > 65536) {
                    printf("Insufficient argument for <pagesize>: please enter an intger that is between 16 to 65536\n");
                }
                if(page_size & (page_size -1) != 0) {
                    printf("Insufficient argument for <pagesize>: please enter an intger that is a power of 2\n");
                    return 1;
                }
                skipsize = strtol(argv[4],&endPtr,10);
                if(*endPtr) {
                    printf("Insufficient argument for <skipsize>: please enter a valid intger\n");
                    return 1;
                }
            }
        }
        else {
            printf("Insufficient arguments supplied: valws379 [ -s skipsize ] pgsize windowsize\n");
            return 1;
        }
    }
    else if(argc == 3) {
        windowsize = strtoull(argv[1],&endPtr,10);
        if(*endPtr) {
            printf("Insufficient argument for <windowsize>: please enter a valid unsigned long long integer\n");
            return 1;
        }
        page_size = strtol(argv[2],&endPtr,10);
        if(*endPtr) {
            printf("Insufficient argument for <pagesize>: please enter an intger that is a power of 2 from 16 to 65536\n");
            return 1;
        }
        if(page_size < 16 || page_size > 65536) {
            printf("Insufficient argument for <pagesize>: please enter an intger that is between 16 to 65536\n");
        }
        if(page_size & (page_size -1) != 0) {
            printf("Insufficient argument for <pagesize>: please enter an intger that is a power of 2\n");
            return 1;
        }
    }
    else {
        printf("Insufficient arguments supplied: valws379 [ -s skipsize ] pgsize windowsize\n");
        return 1; 
    }

    char prog[128];
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

    printf("Enter the name of the program that valgrind will run:\n");
    printf("valgrind --tool=lackey --trace-mem=yes ");
    scanf("%s",prog);
    snprintf(cmd,sizeof(cmd),"valgrind --tool=lackey --trace-mem=yes %s 2>&1",prog);

    
    fp = popen(cmd,"r");
    if (fp == NULL) {
        printf("Failed to run valgrind\n");
        return 1;
    }
    line_ptr = &line[0];

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
    printf("Number of windows: %d\n",num_windows);
    return 0;
}