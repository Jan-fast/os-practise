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

void split(char source[])
{
	char prog[HEIGHT*WIDTH];
	memset(spl_command, 0, sizeof(spl_command));
	strcpy(prog, source);
	char *token;
	int i = 0, j = 0;
	for (token = strtok(prog, " "); token != NULL; token = strtok(NULL, " "))
	{

		for (i = 0; token[i] != '\0'; i++)
		{
			spl_command[j][i] = token[i];
		}
		j++;
	}
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

int main()
{
	while (1)
	{
		char command[LEN];
		char splitArray[HEIGHT][WIDTH] = { {'\0'} };
		printf("%s", "> ");
		gets(command);
		split(command);
		int i = fun();
		if (i == 0)
		{
			mysys(command);
		}
		if (i == -1)
		{
			return 0;
		}
	}
}
