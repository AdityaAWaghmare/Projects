#include "libraries.h"
void ps(char* );
void grep(char*, char*, char*, char*);

int main(){
	//for prompt
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
		//to check if all spaces 
		int t = 1, index = 0;
		while(command[index] !='\0'){
			if(command[index] != ' '){
				t=0;
				break;
			}
			index++;
		}
		char copy1[500], *token[6], copy[500], cmd[500], *token0;
		strcpy(cmd, "");
		for(int i=0; i<6; i++){
			token[i] = NULL;
		}
		//if all are not spaces or empty
		if(t==0){
			strcpy(copy1, command);
			token[0] = strtok(copy1, " ");
			token[1] = strtok( NULL, " ");
			token[2] = strtok( NULL, " ");
			token[3] = strtok( NULL, " ");
			token[4] = strtok( NULL, " ");
			token[5] = strtok( NULL, " ");

			strcpy(copy, command);
                        token0 = strtok(copy, " ");
                        strcpy(cmd , token0);
                        token0 = strtok( NULL, " ");
                        while(token0 != NULL){
                                strcat(cmd , " ");
                                strcat(cmd , token0);
                                token0 = strtok(NULL, " ");
                        }
			add_history(command);
		}
		if(token[0] != NULL){
			if(strcmp(token[0] ,"exit") == 0 && token[1] == NULL){
                        	break;
                	}
			else if(token[0] != NULL && !strcmp(token[0], "ls") && token[2] == NULL) ls(token[1]);
			else if(token[2] != NULL && !strcmp(token[0], "cp") && token[4] == NULL) cp(token[1], token[2], token[3]);
			else if(token[2] != NULL && !strcmp(token[0], "mv") && token[4] == NULL) mv(token[1], token[2], token[3]);
			else if(token[0] != NULL && !strcmp(token[0], "ps") && token[2] == NULL) ps(token[1]);
			else if(token[2] != NULL && !strcmp(token[0], "grep") && token[5] == NULL) grep(token[1], token[2], token[3], token[4]);
			else{
				if(!strcmp(cmd, "help")){
					strcpy(cmd, "bash -c help");
				}
				system(cmd);
			}
		}
	
	}
	printf("\n-myshell terminated-\n");
}
