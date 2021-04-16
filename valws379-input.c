#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// I = read, S = store, L = load, M = modify
// https://www.cs.rutgers.edu/~pxk/416/notes/c-tutorials/pipe.html#:~:text=A%20pipe%20is%20a%20system,(stuff%20to%20be%20read)
// https://stackoverflow.com/questions/646241/c-run-a-system-command-and-get-output
int main(int argc, char const *argv[]) {
    unsigned long long windowsize;
    long page_size;
    long skipsize = 0;
    char *endPtr;
    
    if(argc == 5) {
        if((strcmp(argv[1],"-s") == 0 || strcmp(argv[3],"-s") == 0 ) && !(strcmp(argv[1],"-s") == 0 && strcmp(argv[3],"-s") == 0)  ) {
            if(strcmp(argv[1],"-s") == 0) {
                windowsize = strtoull(argv[3],&endPtr,10);
                if(*endPtr) {
                    printf("Insufficient argument for <windowsize>: please enter a valid unsigned long long integer\n");
                    return 1;
                }
                if(windowsize < 1) {
                    printf("Insufficient argument for <windowsize>: please enter a value greater than 0\n");
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
                if(skipsize < 1) {
                    printf("Insufficient argument for <skipsize>: please enter a value greater than 0\n");
                    return 1;
                }
            }
            else if(strcmp(argv[3],"-s") == 0) {
                windowsize = strtoull(argv[1],&endPtr,10);
                if(*endPtr) {
                    printf("Insufficient argument for <windowsize>: please enter a valid unsigned long long integer\n");
                    return 1;
                }
                if(windowsize < 1) {
                    printf("Insufficient argument for <windowsize>: please enter a value greater than 0\n");
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
                if(skipsize < 1) {
                    printf("Insufficient argument for <skipsize>: please enter a value greater than 0\n");
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
        if(windowsize < 1) {
            printf("Insufficient argument for <windowsize>: please enter a value greater than 0\n");
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

    printf("Enter the name of the program that valgrind will run:\n");
    printf("valgrind --tool=lackey --trace-mem=yes ");
    scanf("%s",prog);
    snprintf(cmd,sizeof(cmd),"./process %lld %ld %ld %s > data.txt",windowsize,page_size,skipsize,prog);


    system(cmd);
    system("python3 plot.py");
    system("rm data.txt");
    return 0;
}