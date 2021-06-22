#include<stdio.h>
#include<unistd.h>

int main(){
    	pid_t pid;
    	pid = fork();
    	printf("pid = %d\n", pid);
    	if (pid == 0)
		printf("In child process\n");
    	else
		printf("In parent process\n");
	return 0;
}
