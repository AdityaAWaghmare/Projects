#include "libraries.h"

int main(){
	char hostname[250];
	gethostname(hostname , 250);
	char *user = getlogin();
	char cwd[500];
	getcwd(cwd , sizeof(cwd));

	char prompt[1000];
	strcpy(prompt , user);
	strcat(prompt , "@");
	strcat(prompt , hostname);
	strcat(prompt , ":");
	strcat(prompt , "~");
	strcat(prompt , cwd);
	strcat(prompt , "$ ");

	printf( "Welcome to myshell\n\n" );
	while(1){
		char *command;
		command = readline(prompt);
		int t = 1, index = 0;
		while(command[index] !='\0'){
			if(command[index] != ' '){
				t=0;
				break;
			}
			index++;
		}
		char copy[500], *token, cmd[500];
		if(t==0){
			strcpy(copy, command);
			token = strtok(copy, " ");
			strcpy(cmd , token);
			token = strtok( NULL, " ");
			while(token != NULL){
				strcat(cmd , " ");
				strcat(cmd , token);
				token = strtok(NULL, " ");
			}
			strcpy(command , cmd);

			add_history(command);
		}

		if( strcmp(command ,"exit") == 0){
                        break;
                }
		else if(strcmp(command , "help") == 0){
			system("bash -c help");
		}
		else{
			system(command);
		}
		free(command);
	}
	printf("\n-myshell terminated-\n");
}
