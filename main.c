#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>

void change_dir(char * newdir);
char ** parse_args( char * line);
int runCmd(char * buffer);
int checkMultipleCmds(char * line);
char ** multipleCmds(char * line);

int main(){
  char login_name [100];
  getlogin_r(login_name,100);
  char host_name[100];
  gethostname(host_name,100);
  while(1){
    char currentDir [100];
    getcwd(currentDir, 100);
    printf("%s@%s:%s$ ",login_name, host_name, currentDir);
    char buffer[100];
    fgets(buffer, 100 , stdin);
    buffer[strlen(buffer)-1] = 0;
    if (checkMultipleCmds(buffer)){
      int n = 1;
      char ** commands = multipleCmds(buffer);
      char * current = *commands;
      while (current != NULL){
	runCmd(current);
	current = *(commands + n);
	n++;
      }
    }
    else
      runCmd(buffer);
  }
  printf("\n");
  return 0;
}

int runCmd(char * buffer){
   char ** args = parse_args(buffer);
    if(! strcmp(args[0], "cd")){
      change_dir(args[1]);
      return 0;
    }
    if(! strcmp(args[0],"exit")){
      exit(0);
      return 0;
    }
    int pid = getpid();
    fork();
    int childInfo;
    if(getpid() == pid)
      wait(&childInfo);
    else{
      execvp(args[0], args);
    }
    free(args);
    return 0;
}
char ** parse_args(char * line){
  char * token;
  char ** returner = malloc(6*sizeof(char *));
  int i = 0;
  while(line){
    token = strsep(&line," ");
    returner[i] = token;
    i++;
  }
  returner[i] = 0;
  return returner;
}

int checkMultipleCmds(char * line){
  if (strchr(line,';'))
    return 1;
  return 0;
}
  
char ** multipleCmds(char * line){
  char * token;
  char ** returner = malloc(6*sizeof(char *));
  int i = 0;
  while(line){
    token = strsep(&line,";");
    if (*token == ' ')
      token = token + 1;
    returner[i] = token;
    i++;
  }
  returner[i] = 0;
  return returner;
}


 
void change_dir(char * newdir){
  int i = chdir(newdir);
  char currentDir [100];
  getcwd(currentDir, 100);
}

void checkForStrokes(){
  struct termios old,new;
  tcsetattr(0,TCSANOW,&old);
  new = old;
  new.c_lflag = new.c_lflag & !ICANON;
  tcsetattr(0, TCSANOW, &current);
}

