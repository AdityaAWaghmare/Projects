#include "libraries.h"

int copy_file(char* , char*);

void cp(char* token1, char* token2, char* token3){
	if(token1 == NULL){}
	else if(token3 == NULL && token2 != NULL){
		copy_file(token1, token2);
	}
	/*else if( !strcmp(token1, "-i") && token3 != NULL){
		struct stat stattemp;
		char ch;
		if(stat(token3, &stattemp) == 0){
			printf("cp: overwrite '%s'?(y/n) ", token3);
			scanf(" %c ", &ch);
			if(ch == 'y' || ch =='Y'){
				copy_file(token2, token3);
			}
		}
		else{
			copy_file(token2, token3);
		}
	}*/
	else if( !strcmp(token1, "-l") && token3 != NULL){
		int returns ;
		returns = link(token2, token3);
		if(returns == -1){
			printf("error hardlinking\n");
		}
	}
	else if( !strcmp(token1, "-n") && token3 != NULL){
		struct stat stattemp;
		if(stat(token3, &stattemp)!=0){
			copy_file(token2, token3);
		}
        }
	else if( !strcmp(token1, "-u") && token3 != NULL){
		struct stat st_source, st_dest;
		if(stat(token2, &st_source) ==0 && stat(token3, &st_dest)==0){
			if(st_source.st_mtime >= st_dest.st_mtime){
				copy_file(token2, token3);
			}
		}
        }
	else if( !strcmp(token1, "-v") && token3 != NULL){
		if(copy_file(token2, token3) == 1){
			printf("'%s' -> '%s'\n", token2, token3);
		}
		else{
			printf("error\n");
		}
        }
}

int copy_file(char *source_name, char *destination_name){
		FILE *source, *destination;
		int readcnt;
                char buffer[1024];
		int execution_success = 0;

                source = fopen(source_name, "rb");
                destination = fopen(destination_name, "wb");

                if(destination == NULL){
                        strcat(destination_name, "/");
                        strcat(destination_name, source_name);
                        destination = fopen(destination_name, "wb");
                }
                if(source != NULL && destination != NULL){
                        while( (readcnt = fread(buffer, 1, 1024, source)) >0){
                                fwrite(buffer ,1 ,readcnt ,destination);
                        }
			execution_success = 1;
                }
		struct stat source_meta;
                if(stat(source_name,&source_meta)==0) chmod(destination_name,source_meta.st_mode);

                if(source != NULL){
                        fclose(source);
                }
                if(destination != NULL){
                        fclose(destination);
                }
		return execution_success;
}
