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
    char *agrv2[num];
    for(int i=0;i<num;i++)
    agrv2[i]=agrv[i];
    agrv2[num]=NULL;

    pid_t pid;
    pid = fork();
    if (pid == 0) {
        int error = execvp(agrv2[0], agrv2); 
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
