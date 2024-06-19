#include "libraries.h"

void grep(char* arg1,char* arg2,char* arg3,char* arg4){
	if(arg3 ==NULL && arg2 != NULL){
		FILE *file = fopen(arg2, "r");
		if(file == NULL){
			printf("error checking file\n");
			exit(0);
		}
		else{
			char line[1000];
			while(fgets(line, 1000, file) != NULL){
				if(strstr(line, arg1) != NULL) printf("%s\n", line);
			}
			fclose(file);
		}
	}
	else if(arg4 == NULL && arg3 != NULL && !strcmp(arg1, "-v")){
		FILE *file = fopen(arg3, "r");
		if(file == NULL){
			printf("error checking file\n"); exit(0);
		}
		else{
			char line[1000];
			while(fgets(line, 1000, file) != NULL){
				if(strstr(line, arg2) == NULL) printf("%s\n", line);
			}
			fclose(file);
		}
	}
	else if(arg4 == NULL && arg3 != NULL && !strcmp(arg1, "-n")){
		FILE *file = fopen(arg3, "r");
		if(file == NULL){
			printf("error checking file\n"); exit(0);
		}
		else{
			char line[1000];
			int line_no = 0;
			while(fgets(line, 1000, file) != NULL){
				line_no++;
				if(strstr(line, arg2) != NULL) printf("%d : %s\n",line_no, line);
			}
			fclose(file);
		}
	}
	else if(arg4 == NULL && arg3 != NULL && !strcmp(arg1, "-o")){
                FILE *file = fopen(arg3, "r");
                if(file == NULL){
                        printf("error checking file\n"); exit(0);
                }
                else{
                        char line[1000];
                        while(fgets(line, 1000, file) != NULL){
                                if(strstr(line, arg2) != NULL) printf("%s\n", arg2);
                        }
                        fclose(file);
                }
        }
	else if(arg4 != NULL && !strcmp(arg1, "-m")){
		int bound = atoi(arg2);
		FILE *file = fopen(arg4, "r");

		if(file == NULL){
			printf("error checking file\n"); exit(0);
		}
		else{
			char line[1000];
			int count = 0;
			while(fgets(line, 1000, file) != NULL){
				if(strstr(line, arg3) != NULL){
					count++;
					printf("%s", line);
				}
				if(count == bound) break;
			}
			fclose(file);
		}
	}
}
