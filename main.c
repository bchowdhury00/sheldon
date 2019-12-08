#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <ctype.h>

void change_dir(char * newdir);
char ** parse_args( char * line);
int runCmd(char * buffer);
int checkMultipleCmds(char * line);
char ** multipleCmds(char * line);
char * processCharacters();
void revertTermios(struct termios termy);
void changeTermios(struct termios * termy);
void getCursorXY(int * x, int * y);

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

    if (checkMultipleCmds(buffer)){
      int n = 1;
      char ** commands = multipleCmds(buffer);
      char * current = *commands;
      while (current != NULL){
      	runCmd(current);
      	current = *(commands + n);
      	n++;
      }
      free(commands);
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

char * processCharacters(){
  char * buffer = malloc(100*sizeof(char));
  int i = 0,sequenceNum =0;
  struct termios goodTermy;
  while(1){
    char ch;
    struct termios goodTermy;
    changeTermios(&goodTermy);
    ch = getchar();
    revertTermios(goodTermy);
    printf("%c",1);
    if(ch == 10){
      printf("%c",ch);
      break;
    }
    if (ch == 0x7f) {
      int x1 = 69;
      int y1 = 69;
      int x2 = 69;
      int y2 = 69;
      getCursorXY(&x1,&y1);
      printf("\b");
      printf(" ");
      printf("\b");
      getCursorXY(&x2,&y2);
      printf("x1: %d, y1: %d, x2: %d, y2: %d\n",x1,y1,x2,y2);
      if(x1 == x2 && y2 == y1){
        //printf("We have a problem");
      }
    }
    else if(ch==27)
        sequenceNum++;
    else if(ch==91 && sequenceNum==1)
        sequenceNum++;
    else if(sequenceNum==2){
        if(ch == 65)
          printf("\nUP KEY\n");
        if(ch == 66)
          printf("\nDOWN KEY\n");
        sequenceNum = 0;
    }
    else {
      sequenceNum = 0;
      buffer[i] = ch;
      i++;
      if(! iscntrl(ch))
        printf("%c",ch);
      else
        printf("%d",ch);
    }
  }
  buffer[i] = 0;
  return buffer;
}
void changeTermios(struct termios *termy){
  struct termios new;
  tcgetattr(0,termy);
  new = *termy;
  new.c_lflag &= ~(ICANON | ECHO);
  new.c_cc[VMIN] = 1;
  new.c_cc[VTIME] = 0;
  new.c_cflag &= ~(CREAD);
  tcsetattr(0, TCSANOW, &new);
}
void revertTermios(struct termios termy){
  tcsetattr(0, TCSANOW, &termy);
}
void getCursorXY(int * x, int * y){
  FILE * file = fopen("cursorPosition","w+");
  fprintf(file,"\033[6n");
  fscanf(file, "\033[%d;%dR",x,y);
  fclose(file);
}
