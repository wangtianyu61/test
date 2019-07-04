#include<stdio.h>
#include<signal.h>
#include<sys/wait.h>
//for Linux processes
#include<unistd.h>
//for Linux processes
#include<stdlib.h>
#include<string.h>
//mainly for handling strings
char split_sign[5] = ",";//default value of separator

void handler(int signal)
/*display when the Ctrl+C signal was caught*/
{
	printf("\nCtrl+C caught and the interpreter was not terminated!\n");
	printf("If you want to exit the shell, please use the bye or Ctrl+Z\n");
	//the codes above are the prompt sentences
}

int special_check(char *cmd)
/*return special DIY commands from others*/
{
	if (strcmp(cmd, "bye") == 0)//detect the "bye" string
	{
		printf("The interpreter has been terminated!\n");//indicate termination
		return 1;
	}
	/* flag = 1 represents the termination signal.
	the codes above handling the bye command.*/
	else
		if (strcmp(cmd, "help") == 0)//detect the help string
		{
			printf("================\nHere are some tips for this shell.\n");
			printf("You can reset separator by inputing the command called 'reset sp'.\nThen you will find a new world.\n");
			printf("Thus you can input multiple commands in one line separated by the separator.\nTo find more information about that, you could input the command 'shell_info'.\n");
			printf("Well, you could quit the shell using 'bye' or Ctrl+Z.\n");
			printf("================\n");
			return 2;
		}
	/*the information above is the 'help' command*/
		else
			if (strcmp(cmd, "shell_info") == 0)//detect the shell_info string
			{
				printf("The current shell uses the separator '%s'.\n", split_sign);
				printf("If you want your command to be executed in the background, you can just append * to the command.\n");
				printf("This program was created by wangtianyu,July 3,2019.\n================\n");
				return 2;
			}
	/*the information above is the 'shell_info' command*/
	return 0;
}

int execute(char *cmd)
/*execute each of the command in one line*/
{
	int i = 1, k = 0, tag;
	char *p;
	//the pointer to split the command by empty space
	int last_length;
	int flag;//flag means the result of execvp function
	int check_flag;//to return information from the special strings
	pid_t pid;

	char *cmdsplit[20] = { 0 };
	//store each part of the command
	if (cmd == NULL)
		return 0;
	//prevent from receving empty commands and continue


	p = strtok(cmd, " ");//split each command into several parts
	cmdsplit[0] = p;
	for (; ;)
	{
		p = strtok(NULL, " ");//split in a loop
		k++;
		if (!p)//split finished
			break;
		cmdsplit[k] = p;
	}

	last_length = strlen(cmdsplit[k - 1]);
	tag = cmdsplit[k - 1][last_length - 1];
	/* to detect whether the last character of the command is '*' */
	if (tag == '*')
		cmdsplit[k - 1][last_length - 1] = '\0';
	/*delete this signal and tag can be regarded as the flag*/
	printf("For the command: ");
	for (i = 0; i < k; i++)
		printf("%s ", cmdsplit[i]);
	printf("\n");
	/*to show the command clearly and divide it from others*/
	check_flag = special_check(cmd);
	if (check_flag != 0)
		return check_flag;
	pid = fork();
	/*create the process*/
	if (pid == 0)
	{
		flag = execvp(cmdsplit[0], cmdsplit);
		if (flag == -1)//to prompt some special commands in case of problems
		{
			/*information about the incorrect commands*/
			printf("Command not found. We recommend you to use the command 'shell_info' or 'help' to find out the rules.\n");
			exit(1);
		}
		printf("\n");
	}
	/*execute the commands in the child process*/
	/*prevent from direct exit*/
	else
	{
		if (pid < 0)
			/*cannot create a child process*/
		{
			printf("Fork error!\n");
			exit(1);
		}
		else
			if (tag != '*')
			{
				wait(NULL);
				wait(NULL);//double check like buffer
				printf("The cycle finished.\n");
			}
		/*to execute the command normally*/
			else
				/*to show it was executed in the background */
				printf("The cycle finished in the background.\n");

	}
	printf("===============\n");//divide separator in the screen
	return 0;
}

int main()
{
	int i, k = 0, flag;
	char cmd_full[1000], *cmd_full_split[20] = { 0 };
	/*split all into the following parts
	cmd_full represents the whole line.
	cmd_full_split stores each command of one line
	*/
	char *p;

	char temp_sign[20] = { 0 };//buffer of the separator input
	//the pointer to split the whole line into several commands by the split signal
	signal(SIGINT, handler);//handle the Ctrl + C
	printf("==========Welcome to wty's shell.================\n");
	printf("You can input multiple commands in one line, separated by the default separator ','.You can change the separator if you input the command called reset sp.\n===========\n");
	/*the commands above are the whole welcome codes*/
	for (;;)
	{
		printf("wty's shell: ~$ ");//display prompt info first
		gets(cmd_full);
		/*receive several commands in one line.
		Therefore, the commands could contain the empty space */
		if (strcmp(cmd_full, "reset sp") == 0)
		{
			printf("please input the separator you want to change to: ");
			scanf("%s", temp_sign);
			getchar();
			if (strlen(temp_sign) > 5)
				printf("the length of the separator is too long!\n");
			else
				strcpy(split_sign, temp_sign);//reset the separator
			memset(cmd_full, 0, sizeof(cmd_full));
			continue;
		}
		p = strtok(cmd_full, split_sign);
		//split into several commands if necessary
		for (;;)
		{
			cmd_full_split[k] = p;
			k++;
			p = strtok(NULL, split_sign);//split in a loop
			if (!p)//split finished
				break;
		}
		/*In this loop, the final value of k means the number of commands*/
		int count = 0;
		for (i = 0; i < k; i++)
		{
			flag = execute(cmd_full_split[i]);
			if (flag == 1)
				break;//indicate the process is over			
		}
		/*this is the execution part and flag to indicate the "bye" command */
		k = 0;
		//reset the k to receive the next command
		memset(cmd_full, 0, sizeof(cmd_full));//empty the line command
		if (flag == 1)
			break;
		//this links flag of bye command to exit the shell
	}
	return 0;
}