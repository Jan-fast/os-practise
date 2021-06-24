#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include<sys/types.h>

void mysys(char *command) {
    char temp[100];
    int num=0;
    char *p;
    char *buff;
    char *agrv[100];
    strcpy(temp,command);
    buff=temp;
    p = strsep(&buff, " ");
    
    while(p!=NULL) {
        agrv[num]=p;
        num=num+1;
        p = strsep(&buff, " ");
    }
    agrv[num]=NULL;
    
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        int error = execvp(agrv[0], agrv); 
        if (error < 0)
            perror("execv");
        exit(123);
    }
    wait(NULL);
}

int main()
{
    printf("--------------------------------------------------\n");
    mysys("echo HELLO WORLD");
    printf("--------------------------------------------------\n");
    mysys("ls /");
    printf("--------------------------------------------------\n");
    return 0;
}
