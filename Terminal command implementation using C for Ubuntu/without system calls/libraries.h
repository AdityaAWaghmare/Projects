#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<readline/history.h>
#include<readline/readline.h>
#include<unistd.h>
#include<dirent.h>
#include<limits.h>
#include<math.h>
#include<sys/dir.h>
#include<sys/stat.h>
#include<sys/time.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<errno.h>
#include<time.h>
#include<pwd.h>
#include<grp.h>
#include<sys/types.h>
#include<ctype.h>

void ls(char* );
void cp(char*, char*, char*);
void mv(char*, char*, char*);
void ps(char* );
void grep(char*, char*, char*, char*);
