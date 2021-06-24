#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define LEN 256
#define WIDTH 256
#define HEIGHT 10

char spl_command[HEIGHT][WIDTH];
int command_num = 0;

void mysys(const char *command)
{
	char prog[HEIGHT*WIDTH];
	if (command == NULL)
	{
		printf("ERROR!\n");
		exit(0);
	}
	memset(prog, 0, sizeof(prog));
	strcpy(prog, command);
	char *argv;
	strtok(prog, " ");
	argv = strtok(NULL, "");
	pid_t pid = fork();
	if (pid == 0)
		execlp(prog, prog, argv, NULL);
	wait(NULL);
}

int fun()
{
	if (strcmp(spl_command[0], "echo") == 0)
	{

		int pid = fork();
		if (pid == 0)
		{
			int i = 0, flag = 0;
			for (i = 1; spl_command[i][0] != '\0'; i++)
			{
				if (spl_command[i][0] == '>')
				{
					flag = 1;
					break;
				}
			}
			if (flag)
			{
				printf("\n");
				char *token, space[2] = { ' ','\0' }, enter[2] = { '\n','\0' };
				token = spl_command[i] + 1;
				FILE *fp = fopen(token, "w+");
				int j = 0, len;
				for (j = 1; j < i - 1; j++)
				{
					fseek(fp, 0, SEEK_END);
					fwrite(spl_command[j], strlen(spl_command[j]), 1, fp);
					fseek(fp, 0, SEEK_END);
					fwrite(space, 2, 1, fp);
				}
				fseek(fp, 0, SEEK_END);
				fwrite(spl_command[j], strlen(spl_command[j]), 1, fp);
				fseek(fp, 0, SEEK_END);
				fwrite(enter, 2, 1, fp);
				fclose(fp);
			}
			else
			{
				int j = 0;
				for (j = 1; spl_command[j][0] != '\0'; j++)
				{
					printf("%s", spl_command[j]);
					printf(" ");
				}
				printf("\n");
			}

		}
		else {
			int status;
			wait(&status);
		}
	}
	else if (strcmp(spl_command[0], "cd") == 0)
	{
		int pid = fork();
		if (pid == 0)
		{
			chdir(spl_command[1]);
		}
		else
		{
			int status;
			wait(&status);
		}
	}
	else if (strcmp(spl_command[0], "pwd") == 0)
	{
		int pid = fork();
		if (pid == 0)
		{
			mysys("pwd");
		}
		else
		{
			int status;
			wait(&status);
		}
	}
	else if (strcmp(spl_command[0], "exit") == 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
	return 1;
}

void split(char string[], char word[16][64])
{
	char *c;
	c = strsep(&string, "|");

	int i=0, j=1;
	for (i=0; c[i] != '\0'; i++)
	{
		if (c[i] != '\n') word[0][i] = c[i];
	}
	word[0][i] = '\0';

	while (c != NULL)
	{
		c = strsep(&string, "|");//用’|’分割字符串，放入字符数组word

		if (c != NULL)
		{
			for (i=0; c[i] != '\0'; i++)
			{
				if (c[i] != '\n') word[j][i] = c[i];
			}
		}
		word[j][i] = '\0';
		j++;
		command_num++;
	}
}

void cmd(char word[16][64]) {
    int p[8][2];//存放文件描述符
    for (int j=0; word[j][0] != '\0'; j++)
    {
        pipe(p[j]);//   int pipe(int pipefd[2]); 成功：0；失败：-1，设置errno
        pid_t pid = fork();
        
        if (pid == 0)		//子进程
        {
            if (j > 0)
            {
                dup2(p[j-1][0], 0); //p[j][0]管道读端
                close(p[j-1][0]);

                close(p[j-1][1]); //p[j][1]管道写端

            }

            if (word[j+1][0] != '\0')
            {
                dup2(p[j][1], 1); //p[j][1]管道写端
                close(p[j][1]); //p[j][1]管道写端

                close(p[j][0]); //p[j][0]管道读端
            }

            execl("/bin/sh", "sh", "-c", word[j], NULL);
            exit(0);
        }
        else if (pid < 0)
        {
            printf("fork failed\n");
        }
        else				//父进程
        {
            wait(NULL);
            if (word[j+1][0] != '\0') close(p[j][1]); //p[j][1]管道写端
        }
    }
}

int main()
{
	while (1)
	{
		char command[LEN];
        char word[16][64];
		char splitArray[HEIGHT][WIDTH] = { {'\0'} };
		printf("%s", "> ");
		gets(command);
		split(command, word);
		printf("%d", command_num);
		cmd(word);
	}
}