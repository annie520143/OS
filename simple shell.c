/*
Student No.: 0616308
Student Name: Chia Shin Ho
Email: annie520143@gmail.com
SE tag: xnxcxtxuxoxs
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/

#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>
#include<stdbool.h>

char input[1001];
char* argv[100];
bool background;

void print_prompt()
{
	printf(">");
}


void read_cmd()
{

	int  i = 0;
	char *tok;
	fgets(input, 1000, stdin);
	
	tok = strtok(input, " ");
	while( tok != NULL ){
		argv[i++] = strdup(tok);
		tok = strtok(NULL, " ");
	}

	i--;
	argv[i][strlen(argv[i])-1] = '\0';

	//execute in background
	if( strcmp(argv[i], "&") == 0){
		background = true;
		argv[i] = NULL;
	}

	else
		background = false;

}


void clear_cmd()
{
	for(int i=0;i<100;i++) argv[i] = NULL;
}

int main()
{		

	
	pid_t pid, cpid;
		
	while(1){

		print_prompt();
		read_cmd();
		pid = fork();
		
		//if(strcmp(argv[0], "exit") == 0) exit(0);

		//fork error
		 else if(pid < 0){
			fprintf(stderr, "fork error");
			exit(-1);
		}

		//child process
		else if(pid == 0){
		//	printf("child");
			
			//command with &
			if(background){
				cpid = fork();
			
				//if(strcmp(argv[0], "exit") == 0) exit(0);
				if(cpid < 0) exit(-1);
				else if(cpid > 0) exit(0);
				else execvp(argv[0], argv);
			}
			
			else
				execvp(argv[0], argv);	
		}

		//parent process
		else{
		//	printf("parent"); 
			wait(NULL);
		}

		clear_cmd();

	}

	return 0;
}

