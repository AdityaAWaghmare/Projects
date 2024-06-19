#include "libraries.h"
#define GREEN "\x1b[32m\e[1m"
#define BLUE "\x1b[34m\e[1m"
#define RESET "\e[0m\x1b[0m"
void permissions(mode_t );

void ls(char *option){
	if(option == NULL){
		struct dirent *dirdata;
		struct stat stattemp;
		DIR *dir = opendir(".");
		dirdata = readdir(dir);
		while(dirdata != NULL){
			stat(dirdata->d_name, &stattemp);
			char *copy = dirdata->d_name;
			if(S_ISDIR(stattemp.st_mode) && copy[0] != '.'){
				printf(BLUE"%s  "RESET, dirdata->d_name);
			}
			else if(copy[0] != '.' &&(S_IXUSR & stattemp.st_mode)){
					printf(GREEN"%s  "RESET, dirdata->d_name);
			}
			else if(copy[0] != '.'){
				printf("%s  ", dirdata->d_name);
			}
				dirdata = readdir(dir);
		}
		closedir(dir);
		printf("\n");
	}
	else if(!strcmp(option , "-a")){
		struct dirent *dirdata;
		struct stat stattemp;
		DIR *dir = opendir(".");
                dirdata = readdir(dir);
                while(dirdata != NULL){
                        stat(dirdata->d_name, &stattemp);
                        if(S_ISDIR(stattemp.st_mode)){
                                printf(BLUE"%s  "RESET, dirdata->d_name);
                        }
                        else if(S_IXUSR & stattemp.st_mode){
                                printf(GREEN"%s  "RESET, dirdata->d_name);
                        }
			else{
				printf("%s  ", dirdata->d_name);
			}
                                dirdata = readdir(dir);
                }
                closedir(dir);
		printf("\n");
        }
	else if(!strcmp(option, "-i")){
		struct dirent *dirdata;
                struct stat stattemp;
                DIR *dir = opendir(".");
                dirdata = readdir(dir);
                while(dirdata != NULL){
                        stat(dirdata->d_name, &stattemp);
                        char *copy = dirdata->d_name;
			ino_t index = stattemp.st_ino;
                        if(S_ISDIR(stattemp.st_mode) && copy[0] != '.'){
                                printf("%lu "BLUE"%s\n"RESET, (unsigned long)index, dirdata->d_name);
                        }
                        else if(copy[0] != '.' &&(S_IXUSR & stattemp.st_mode)){
                                        printf("%lu "GREEN"%s\n"RESET, (unsigned long)index, dirdata->d_name);
                        }
                        else if(copy[0] != '.'){
                                printf("%lu %s\n", (unsigned long)index, dirdata->d_name);
                        }
                                dirdata = readdir(dir);
                }
                closedir(dir);
        }
	else if(!strcmp(option, "-l")){
		struct dirent *dirdata;
		struct stat stattemp;
		DIR *dir = opendir(".");
		struct passwd *pw;
		struct group *grp;
		char time[80];
		dirdata = readdir(dir);

		long int total_blocks = 0;

		while(dirdata != NULL){
			stat(dirdata->d_name, &stattemp);
			char *copy = dirdata->d_name;
			if(copy[0] != '.'){
				permissions(stattemp.st_mode);

				printf(" %ld ",(long)stattemp.st_nlink);

				pw = getpwuid(stattemp.st_uid);
				if(pw == NULL){
					printf("%5ld ",(long)stattemp.st_uid);
				}
				else{
					printf("%5s ",pw->pw_name);
				}

				grp = getgrgid(stattemp.st_gid);
                                if(pw == NULL){
                                        printf("%5ld ",(long)stattemp.st_gid);
                                }
                                else{
                                        printf("%5s ",grp->gr_name);
                                }

				printf("%10ld ", (long)stattemp.st_size);

				strftime(time , sizeof(time), "%b %e %H:%M", localtime(&(stattemp.st_mtime)));
				printf("%s ", time);

				printf("%s\n", dirdata->d_name);

				/*total_blocks += stattemp.st_size/stattemp.st_blksize;
				if(stattemp.st_size%stattemp.st_blksize >0){
					total_blocks++;
				}
				*/
				total_blocks += stattemp.st_blocks;
			}
			
			dirdata = readdir(dir);
		}
		closedir(dir);
		printf("total %ld\n", total_blocks/2);
	}
	else if(!strcmp(option , "-s")){
		struct dirent *dirdata;
                struct stat stattemp;
                DIR *dir = opendir(".");
                dirdata = readdir(dir);

		long int total = 0;

                while(dirdata != NULL){
                        stat(dirdata->d_name, &stattemp);
                        char *copy = dirdata->d_name;
                        if(S_ISDIR(stattemp.st_mode) && copy[0] != '.'){
                                printf("%ld "BLUE"%s\n"RESET, stattemp.st_blocks/2, dirdata->d_name);
				total += stattemp.st_blocks/2;
                        }
                        else if(copy[0] != '.' &&(S_IXUSR & stattemp.st_mode)){
                                        printf("%ld "GREEN"%s\n"RESET, stattemp.st_blocks/2, dirdata->d_name);
					total += stattemp.st_blocks/2;
                        }
                        else if(copy[0] != '.'){
                                printf("%lu %s\n", stattemp.st_blocks/2, dirdata->d_name);
				total += stattemp.st_blocks/2;
                        }
                                dirdata = readdir(dir);
                }
                closedir(dir);
		printf("total %ld\n", total);
        }
	else if(!strcmp(option, "-m")){
		struct dirent *dirdata;
                struct stat stattemp;
                DIR *dir = opendir(".");
                dirdata = readdir(dir);
		int t=0;
                while(dirdata != NULL){
                        stat(dirdata->d_name, &stattemp);
                        char *copy = dirdata->d_name;
                        if(S_ISDIR(stattemp.st_mode) && copy[0] != '.'){
				if(t==1){
                                	printf(BLUE ", %s" RESET, dirdata->d_name);
				}
				else{
					printf(BLUE "%s" RESET, dirdata->d_name);
					t=1;
				}

                        }
                        else if(copy[0] != '.' &&(S_IXUSR & stattemp.st_mode)){
				if(t==1){
                                        printf(GREEN ", %s" RESET, dirdata->d_name);
				}
				else{
					printf(GREEN "%s" RESET, dirdata->d_name);
					t=1;
				}
                        }
                        else if(copy[0] != '.'){
				if(t==1){
                                	printf(", %s", dirdata->d_name);
				}
				else{
					printf("%s", dirdata->d_name);
					t=1;
				}

                        }
                                dirdata = readdir(dir);
                }
                closedir(dir);
                printf("\n");
        }

}
void permissions(mode_t mode){
		printf( (S_ISDIR(mode)) ?"d":"-");
                printf( (mode & S_IRUSR)?"r":"-");
                printf( (mode & S_IWUSR)?"w":"-");
                printf( (mode & S_IXUSR)?"x":"-");
                printf( (mode & S_IRGRP)?"r":"-");
                printf( (mode & S_IWGRP)?"w":"-");
                printf( (mode & S_IXGRP)?"x":"-");
                printf( (mode & S_IROTH)?"r":"-");
                printf( (mode & S_IWOTH)?"w":"-");
                printf( (mode & S_IXOTH)?"x":"-");
}
