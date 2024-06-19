#include "libraries.h"

void mv(char *token1, char *token2, char *token3){
	if(token1 == NULL){}
	else if(token3 == NULL && token2 != NULL){
		rename(token1, token2);
	}
	/*else if(token3 != NULL && !strcmp(token1, "-i")){
		struct stat stattemp;
                char* ch, *prompt;
		int status = stat(token3, &stattemp);
                if(status == 0){
                        prompt = strcat("cp: overwrite '",token3);
			prompt = strcat(prompt,"'?(y/n) ");
			ch = readline(prompt);
                        if(!strcmp(prompt ,"y") || !strcmp(prompt, "Y")){
                                rename(token2, token3);
                        }
                }
                else{
                        rename(token2, token3);
                }
	}*/
	else if(token3 != NULL && !strcmp(token1, "-f")){
		rename(token2, token3);
	}
	else if(token3 != NULL && !strcmp(token1, "-n")){
	       struct stat stattemp;
               if(stat(token3, &stattemp)!=0){
		       rename(token2, token3);
	       }
	}
	else if( !strcmp(token1, "-u") && token3 != NULL){
                struct stat st_source, st_dest;
                if(stat(token2, &st_source) ==0 && stat(token3, &st_dest)==0){
                        if(st_source.st_mtime >= st_dest.st_mtime){
                                rename(token2, token3);
                        }
                }
        }
	else if( !strcmp(token1, "-v") && token3 != NULL){
                if(rename(token2, token3) == 0){
                        printf("renamed '%s' -> '%s'\n", token2, token3);
                }
                else{
                        printf("error\n");
                }
        }
}
