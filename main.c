#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>

void change_dir(char * newdir);
char ** parse_args( char * line);
char * processCharacters();
void revertTermios(struct termios termy);
void changeTermios(struct termios * termy);

int main(){
  char login_name [100];
  getlogin_r(login_name,100);
  char host_name[100];
  gethostname(host_name,100);
  while(1){
    char currentDir [100];
    getcwd(currentDir, 100);
    printf("%s@%s:%s$ ",login_name, host_name, currentDir);

    char * buffer = processCharacters();
    char ** args = parse_args(buffer);
    if(! strcmp(args[0], "cd")){
      change_dir(args[1]);
      continue;
    }
    if(! strcmp(args[0],"exit")){
      exit(0);
      continue;
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
    free(buffer);
  }
  printf("\n");
  return 0;
}

char ** parse_args( char * line){
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
void change_dir(char * newdir){
  int i = chdir(newdir);
  char currentDir [100];
  getcwd(currentDir, 100);
}
void changeTermios(struct termios *termy){
  struct termios new;
  tcgetattr(0,termy);
  new = *termy;
  new.c_lflag = new.c_lflag | ECHO & !ICANON;
  tcsetattr(0, TCSANOW, &new);
}
void revertTermios(struct termios termy){
  tcsetattr(0, TCSANOW, &termy);
}
char * processCharacters(){
  char * buffer = malloc(100*sizeof(char));
  int i =0;
  while(1){
    char ch;
    struct termios goodTermy;
    changeTermios(&goodTermy);
    ch = getchar();
    revertTermios(goodTermy);
    if(ch==10)
      break;
    buffer[i] = ch;
    i++;

  }
  buffer[i] = 0;
  return buffer;
}