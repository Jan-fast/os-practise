#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>

#define MAX_LEN 100
#define MAX_ARGC 16
int command_count = 0;
struct command{
	int argc;
	char *argv[MAX_ARGC];
	char input[MAX_LEN];
	char output[MAX_LEN];
}Cmd[4];

void mysys_simple(struct command *c) 	//不包含管道和重定向的shell命令
{
	if(strcmp(c->argv[0], "exit") == 0)      //exit
		exit(0);
	else if(strcmp(c->argv[0], "pwd") == 0)      //pwd
	{
		char path_name[100];
		if(getcwd(path_name,sizeof(path_name)))
			printf("%s\n", path_name);
	}		
	else if(strcmp(c->argv[0], "cd") == 0)       //cd
	{
		getcwd(c->argv[1], sizeof(c->argv[1]));
		chdir(c->argv[1]);	
	}	
	else
	{	
		int pid;
		pid = fork();
		if(pid == 0)
		{

			if(execvp(c->argv[0],c->argv) < 0)
				perror("execvp");
      		exit(0);
		}
		wait(NULL);
	}
	return ;
}

void mysys_pipe()   					//实现管道命令
{
	pid_t pid;    
    int fd[2];

    pipe(fd); 
    pid = fork();
    if (pid == 0) { 
        dup2(fd[0], 0); 
        close(fd[0]);
        close(fd[1]);

        mysys_simple(&Cmd[1]);
        exit(0);
    }
    //wait(NULL);
    dup2(fd[1], 1); 
    close(fd[0]);
    close(fd[1]);
 
    mysys_simple(&Cmd[0]);
}

// void redirect(struct command *c)  		//重定向
// {
// 	int fd0, fd1;
// 	if(c->output != NULL)          //重定向输出
// 	{
// 		fd1 = open(c->output, O_CREAT|O_RDWR, 0666);
// 		dup2(fd1, 1);
// 		close(fd1);
// 	}
// 	if(c->input != NULL)           //重定向输入
// 	{
// 		fd0 = open(c->input, O_CREAT|O_RDWR, 0666);
// 		dup2(fd0, 0);
// 		close(fd0);
// 	}
// }

void dump_command(void)    				//打印结构体信息
{
	for(int i=0;i<command_count;i++)
	{
		printf("argc=%d\n",Cmd[i].argc);
		for(int j=0;j<Cmd[i].argc;j++)
			printf("[%s]",Cmd[i].argv[j]);
		printf("\n");
		printf("output=[%s]\n",Cmd[i].output);
		printf("input=[%s]\n",Cmd[i].input);

	}
}

void parse_command_s(char *line,int c_count)  //把不包含管道符的line字符串切割存到command【】中     
{
	char buffer[MAX_LEN]={'\0'};
	memset(buffer,0,sizeof(buffer));
	strcpy(buffer,line);
	int count=0;
	char *token;
	char *buf;
	token=strtok_r(buffer," ",&buf);
	while( token )
	{
		if(strchr(token,'<') != NULL ){
			if(strlen(token)>1){
				strcpy(Cmd[c_count].input,token+1);
				token = strtok_r(NULL," ",&buf);
				continue;
			}else{
				token = strtok_r(NULL," ",&buf);
				strcpy(Cmd[c_count].input,token);
				token = strtok_r(NULL," ",&buf);
				continue;
			}
		}
	    if(strchr(token,'>') != NULL){
			if(strlen(token)>1){
				strcpy(Cmd[c_count].output,token+1);
				token = strtok_r(NULL," ",&buf);
				continue;
			}else{
				token = strtok_r(NULL," ",&buf);
				strcpy(Cmd[c_count].output,token);
				token = strtok_r(NULL," ",&buf);
				continue;
			}
		}
		if(token != NULL){
			Cmd[c_count].argv[count]=(char *)malloc(sizeof(MAX_LEN));
			strcpy(Cmd[c_count].argv[count],token);
		}
		count++;
		token=strtok_r(NULL," ",&buf);
	}
	Cmd[c_count].argv[count]=NULL;
	Cmd[c_count].argc=count;
}

void parse_command(char *line)         // 根据管道符对line进行切割  
{
	char *token_s;
	char *buf1 = NULL;
	token_s = strtok_r(line,"|",&buf1);
	while( token_s ){
		parse_command_s(token_s,command_count);
		command_count++;
		token_s=strtok_r(NULL,"|",&buf1);
    }
}

int main() 
{
    while(1)
    {
        char input[256];
		printf(">");
		gets(input);
		if(strcmp(input,"")==0)
			continue;
		parse_command(input);
		// dump_command();
        if(command_count == 1)
        {
        	if(Cmd[0].input[0] == '\0'  && Cmd[0].output[0] == '\0')
				mysys_simple(&Cmd[0]);
			// else
			// {
			// 	int pid= fork();
        	// 	if(pid == 0)
			// 	{
         	// 		redirect(&Cmd[0]);
		   	//   		mysys_simple(&Cmd[0]);
          	// 		exit(0);
        	// 	}
        	// wait(NULL);
			// }
		}			
		else
		{
			int pid1 = fork();
			if (pid1 == 0)
			{
				mysys_pipe();
				exit(0);
			}	
			wait(NULL);
			sleep(1);
		}
		command_count = 0;
		memset(Cmd,0,sizeof(Cmd));
	}
	return 0;
}