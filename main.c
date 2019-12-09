#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <ctype.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/stat.h>

void change_dir(char * newdir);
char ** parse_args( char * line);
int runCmd(char * buffer);
int checkMultipleCmds(char * line);
char ** multipleCmds(char * line);
char * processCharacters();
void revertTermios(struct termios termy);
void changeTermios(struct termios * termy);
void getCursorXY(int * x, int * y);
void moveCursorRight(int * currentX, int * currentY, int totalRow, int totalCol, int  * initialX);
int moveCursorLeft(int * currentX, int * currentY, int initialX, int initialY,int totalCol);

int main(){
  char login_name [100];
  getlogin_r(login_name,100);
  char host_name[100];
  gethostname(host_name,100);

  FILE *commandsStream = fopen("commands","r");
  char commandList[100][100];
  int i = 0;
  while (fgets(commandList[i], sizeof(commandList[i]), commandsStream))
    i++;
  fclose(commandsStream);

  int commandsFile = open("commands",O_RDWR | O_CREAT | O_APPEND,0755);
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
        write(commandsFile, buffer, strlen(buffer));
      	current = *(commands + n);
      	n++;
      }
      free(commands);
    }
    else{
      runCmd(buffer);
      int len = strlen(buffer);
      buffer[len] = '\n';
      buffer[len+1] = 0;
      write(commandsFile, buffer, strlen(buffer));
    }
    //commandsFile = freopen("commands","r",commandsFile);
    //char reading [100];
    //fread(reading, sizeof(char), 100, commandsFile);
    //printf("%s",reading);
  }
  close(commandsFile);

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
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  int totalCol = w.ws_col;
  int totalRow = w.ws_row;
  int initialX;
  int initialY;
  struct termios goodTermy;

  changeTermios(&goodTermy);
  getCursorXY(&initialX,&initialY);

  int currentX = initialX;
  int currentY = initialY;


  char * buffer = calloc(100,sizeof(char));
  int i = 0,sequenceNum =0;
  while(1){
    printf("\033[0;0H Current X: %d, Current Y %d    ", currentX,currentY);
    printf("\033[%d;%dH", currentX,currentY);
    char ch;
    ch = getchar();
    if(ch == 10){
      printf("%c",ch);
      //currentX+=1;
      //currentY=initialY;
      break;
    }
    if (ch == 0x7f) {
      int val = moveCursorLeft(&currentX,&currentY,initialX,initialY,totalCol);
      if(val){
        if(val == 1){
          buffer[i] = ' ';
          i--;
          printf("\033[%d;%dH ", currentX,currentY+1);
        }
        continue;
      }
      buffer[i] = ' ';
      i--;
      printf("\b");
      printf(" ");
      printf("\b");
      fflush(stdout);

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
        if(ch == 67){
          moveCursorRight(&currentX,&currentY,totalRow,totalCol,&initialX);
          i++;
        }
        if(ch == 68){
          moveCursorLeft(&currentX,&currentY,initialX,initialY,totalCol);
          i--;
        }
        sequenceNum = 0;
    }
    else {

      if(! iscntrl(ch))
        printf("%c",ch);
      else
        printf("%d",ch);
      moveCursorRight(&currentX,&currentY,totalRow,totalCol,&initialX);

      sequenceNum = 0;
      buffer[i] = ch;
      i++;
    }
  }
  revertTermios(goodTermy);
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
  printf("\033[6n");
  fflush(stdout);
  //fseek(file, 0, SEEK_SET);
  //fgets(buff, 20, file);
  //printf("%s\n",buff);
  scanf("\033[%d;%dR",x,y);
  printf("\033[%d;%dH", (*x), (*y));
  fflush(stdout);
}
void moveCursorRight(int * currentX, int * currentY, int totalRow, int totalCol, int  * initialX){
  *currentY= *currentY+1;
  if(*currentY == totalCol + 1){
    //printf("EWTD");
    printf("\n");
    if(*currentX <totalRow){
      *currentX = *currentX+1;
    }
    else{
      *initialX = *initialX-1;
    }
    *currentY = 1;
  }
}
int moveCursorLeft(int * currentX, int * currentY, int initialX, int initialY,int totalCol){
  if(*currentX == initialX && *currentY == initialY){
    printf("\a");
    return 2;
  }
  *currentY= *currentY-1;
  if(*currentY == 0){
    *currentX = *currentX-1;
    *currentY = totalCol;
    return 1;
  }
  return 0;
}
