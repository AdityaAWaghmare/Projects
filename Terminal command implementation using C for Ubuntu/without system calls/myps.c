#include "libraries.h"


void ps(char* option){
	
	if(option == NULL|| !strcmp(option,"-e")|| !strcmp(option,"-d")|| !strcmp(option,"-a")|| !strcmp(option,"-f") ){
		//opening proc directory to check process
		DIR *dir = opendir("/proc");
		struct dirent *entries;
		
		//to check own terminal
		int self_fd_0;
		char own_tty[25];
		self_fd_0 = open("/proc/self/fd/0", O_RDONLY);
		sprintf(own_tty, "%s", ttyname(self_fd_0));
		close(self_fd_0);
		
		//ps start
		if(option != NULL && !strcmp(option, "-f")){
			printf("%-12s %5s %5s %5s %5s %-9s\t%8s %s \n", "UID", "PID", "PPID", "C", "STIME", "TTY", "TIME", "CMD");
		}
		else{
			printf("%5s %-9s %8s %s \n", "PID", "TTY", "TIME", "CMD");
		}
		//traverse dir entries
		while ((entries = readdir(dir)) != NULL){
			
			//check if process dir
			int is_num = 1;
			for (int i = 0; entries->d_name[i]; i++){
				if (!isdigit(entries->d_name[i])){
					is_num = 0;
					break;
				}
			}
			//if process dir
			if(is_num){
				//check this process terminal
				char path[1000], *tty;
				int fd_0;
				sprintf(path, "/proc/%s/fd/0", entries->d_name);
				fd_0 = open(path, O_RDONLY);
				tty = ttyname(fd_0);
				
				//open stat file for data for ps
				FILE *file;
				sprintf(path, "/proc/%s/stat", entries->d_name);
				file = fopen(path, "r");
				
				unsigned long utime, stime, pid_not, ppid, c ;
				char cmd[250];
				int sid;//get ps data
				fscanf(file, "%lu %s %*s %lu %*s %d %*s %*s %*s %*s %*s %*s %*s %lu %lu %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %lu ",&pid_not, cmd , &ppid, &sid, &utime , &stime, &c);
				//get pid
				int pid = atoi(entries->d_name);
				
				cmd[strlen(cmd) - 1] = '\0';

				//get ps TIME	
				unsigned long time;
				char TIME[250], STIME[250];				
				time = (int)((utime + stime) / sysconf(_SC_CLK_TCK));
				sprintf(TIME, "%02lu:%02lu:%02lu", (time / 3600) % 24, (time / 60) % 60, time % 60);
				//get ps STIME	
				stime = (int)(stime/ sysconf(_SC_CLK_TCK) );	
				sprintf(STIME, "%02lu:%02lu", (stime / 3600) % 24, (stime / 60) % 60);
				//print for options
				if(option == NULL){
					if (tty && strcmp(tty, own_tty) == 0){
						printf("%5s %-9s %8s %s \n", entries->d_name, tty + 5, TIME, cmd + 1);
					}
				}
				else if(!strcmp(option, "-e")){	
					if(tty == NULL)
					{
						printf("%5s %-9c %8s %s \n", entries->d_name, '?', TIME, cmd + 1);
					}
					else{
						printf("%5s %-9s %8s %s \n", entries->d_name, tty + 5, TIME, cmd + 1);
					}	
				}
				else if(!strcmp(option, "-d")){
					if(pid != sid){		
						if(tty == NULL){
							printf("%5s %-9c %8s %s \n", entries->d_name, '?', TIME, cmd + 1);
						}
						else
						{
							printf("%5s %-9s %8s %s \n", entries->d_name, tty + 5, TIME, cmd + 1);
						}
					}	
				}
				else if(!strcmp(option, "-a")){
					if(pid != sid){		
						if(tty != NULL)
						{
							printf("%5s %-9s %8s %s \n", entries->d_name, tty + 5, TIME, cmd + 1);
						}
					}	
				}
				else if(!strcmp(option, "-f")){
					if (tty && strcmp(tty, own_tty) == 0){
						printf("%-12s %5s %5lu %5lu %5s %-9s\t%8s %s \n", getlogin(), entries->d_name, ppid, c, STIME, tty + 5, TIME, cmd + 1);
					}
				}
				
				fclose(file);
				close(fd_0);				
			}
		}
		closedir(dir);	
	}
}
