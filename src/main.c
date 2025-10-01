#include <stdio.h>

int main(int argc, char *argv[]){
    char *executable = argv[0];
    if(argc < 2){
        printf("No command specified, please type '%s help' to get a list of all the commands.\n", executable);
        return 0;
    }

    char *command = argv[1];
    printf("Command : %s\n", command);
    return 0;
}