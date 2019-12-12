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
char * processCharacters(char ** commandList);
void revertTermios(struct termios termy);
void changeTermios(struct termios * termy);
void getCursorXY(int * x, int * y);
void moveCursorRight(int * currentX, int * currentY, int totalRow, int totalCol, int  * initial);
int moveCursorLeft(int * currentX, int * currentY, int initialX, int initialY,int totalCol);
void deleteIndex(char ** buffer, int * index);
int * returnPointFromMatrix(int initialX, int initialY, int totalRow, int totalCol, int len);
void addIndex(char ** buffer, int * index, char value);
void insertBeginning(char *** multiArray, char * command, int lastIndex);
int redirect(char * buffer);
int existsRedirection(char * buffer);
char * stripwhitespace(char * arr);
int doubleRedirect(char * buffer, char * extra);

int main(){
  char login_name [100];
  getlogin_r(login_name,100);
  char host_name[100];
  gethostname(host_name,100);

  FILE *commandsStream = fopen("commands","wb");
  char ** commandList = malloc(100*sizeof(char *));
  int j;
  for(j=0;j<100;j++){
    commandList[j] = malloc(100*sizeof(char));
  }
  int i = 0;
  int limit = 99;
  while (fgets(commandList[i], sizeof(commandList[i]), commandsStream)){
    i++;
    if(i==limit){
      commandList = realloc(commandList,sizeof(char *) * 2 * (limit+1));
      for(j=limit+1;j<2*limit+2;j++){
        commandList[j] = malloc(100*sizeof(char));
      }
      limit = 2*limit+1;
    }
  }
  fclose(commandsStream);

  int commandsFile = open("commands",O_RDWR | O_CREAT | O_APPEND,0755);
  while(1){
    char currentDir [100];
    getcwd(currentDir, 100);
    printf("%s@%s:%s$ ",login_name, host_name, currentDir);

    char * buffer = processCharacters(commandList);
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
      insertBeginning(&commandList, buffer, i);
      i++;
      if(i==limit){
        commandList = realloc(commandList,sizeof(char *) * 2 * (limit+1));
        for(j=limit+1;j<2*limit+2;j++){
          commandList[j] = malloc(100*sizeof(char));
        }
        limit = 2*limit+1;
      }
      if (existsRedirection(buffer))
	redirect(buffer);
      else
	runCmd(buffer);
      buffer[strlen(buffer)-1] ='\n';
      buffer[strlen(buffer)-1] = 0;
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
    ///printf("\n%c",args[0][4]);
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

int existsRedirection(char * buffer){
  if (strchr(buffer,'<'))
    return 1;
  if (strchr(buffer,'>'))
    return 1;
  return 0;
}

int redirect(char * buffer){
  FILE * output;
  if (strstr(buffer,"2>>")){
    char * file = buffer;
    strsep(&file,"2");
    strsep(&file,">");
    strsep(&file,">");
    file = stripwhitespace(file);
    buffer = stripwhitespace(buffer);
    output = fopen(file,"a");
    int  old = dup(STDERR_FILENO);
    int f = fileno(output);
    dup2(STDERR_FILENO,fileno(output));
    runCmd(buffer);
    dup2(STDERR_FILENO,old);
    close(old);
    fclose(output);
    return 0;
  }
  else if (strstr(buffer,"2>")){
    char * file = buffer;
    strsep(&file,"2");
    strsep(&file,">");
    file = stripwhitespace(file);
    buffer = stripwhitespace(buffer);
    output = fopen(file,"w");
    int  old = dup(STDERR_FILENO);
    int f = fileno(output);
    dup2(f,STDERR_FILENO);
    runCmd(buffer);
    dup2(old,STDERR_FILENO);
    close(old);
    fclose(output);
    return 0;
  }
  else if (strstr(buffer,">>")){
    char * file = buffer;
    strsep(&file,">");
    strsep(&file,">");
    file = stripwhitespace(file);
    buffer = stripwhitespace(buffer);
    output = fopen(file,"a");
    int f = fileno(output);
    int  old = dup(STDOUT_FILENO);
    dup2(f,STDOUT_FILENO);
    runCmd(buffer);
    dup2(old,STDOUT_FILENO);
    close(old);
    fclose(output);
    return 0;
  }
  else if (strstr(buffer,">")){
    char * file = buffer;
    strsep(&file,">");
    file = stripwhitespace(file);
    buffer = stripwhitespace(buffer);
    output = fopen(file,"w");
    int f = fileno(output);
    int  old = dup(STDOUT_FILENO);
    dup2(f,STDOUT_FILENO);
    runCmd(buffer);
    dup2(old,STDOUT_FILENO);
    close(old);
    fclose(output);
    return 0;
  }
  else if (strstr(buffer,"<")){
    char * file  =  buffer;
    strsep(&file,"<");
    file = stripwhitespace(file);
    buffer = stripwhitespace(buffer);
    output = fopen(file,"r");
    int f = fileno(output);
    int  old = dup(STDIN_FILENO);
    dup2(f,STDIN_FILENO);
    runCmd(buffer);
    dup2(old,STDIN_FILENO);
    close(old);
    fclose(output);
    return 0;
  }
  return 0;
}
/*
int doubleRedirect(char * buffer, char * extra){
  FILE * output;
  if (strstr(extra,"2>>")){
    char * file = extra;
    strsep(&file,"2");
    strsep(&file,">");
    strsep(&file,">");
    file = stripwhitespace(file);
    output = fopen(file,"a");
    int  old = dup(STDERR_FILENO);
    int f = fileno(output);
    dup2(STDERR_FILENO,fileno(output));
    runCmd(buffer);
    dup2(STDERR_FILENO,old);
    close(old);
    fclose(output);
    return 0;
  }
  else if (strstr(extra,"2>")){
    char * file = extra;
    strsep(&file,"2");
    strsep(&file,">");
    file = stripwhitespace(file);
    output = fopen(file,"w");
    int  old = dup(STDERR_FILENO);
    int f = fileno(output);
    dup2(f,STDERR_FILENO);
    runCmd(buffer);
    dup2(old,STDERR_FILENO);
    close(old);
    fclose(output);
    return 0;
  }
  else if (strstr(extra,">>")){
    char * file = extra;
    strsep(&file,">");
    strsep(&file,">");
    file = stripwhitespace(file);
    output = fopen(file,"a");
    int f = fileno(output);
    int  old = dup(STDOUT_FILENO);
    dup2(f,STDOUT_FILENO);
    runCmd(buffer);
    dup2(old,STDOUT_FILENO);
    close(old);
    fclose(output);
    return 0;
  }
  else if (strstr(extra,">")){
    char * file = extra;
    strsep(&file,">");
    file = stripwhitespace(file);
    output = fopen(file,"w");
    int f = fileno(output);
    int  old = dup(STDOUT_FILENO);
    dup2(f,STDOUT_FILENO);
    runCmd(buffer);
    dup2(old,STDOUT_FILENO);
    close(old);
    fclose(output);
    return 0;
  }
  else if (strstr(extra,"<")){
    char * file  = extra;
    strsep(&file,"<");
    file = stripwhitespace(file);
    output = fopen(file,"r");
    int f = fileno(output);
    int  old = dup(STDIN_FILENO);
    dup2(f,STDIN_FILENO);
    runCmd(buffer);
    dup2(old,STDIN_FILENO);
    close(old);
    fclose(output);
    return 0;
  }
  return 0;
}
*/
char * stripwhitespace(char * arr){
  while (arr[0] == ' '){
    arr = arr + 1;
  }
  int end = strlen(arr);
  while ((arr)[end - 1] == ' '){
    (arr)[end - 1] = '\0';
    end = strlen(arr);
  }
  return arr;
}

char * pipe(char * buffer){
  
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
    token = stripwhitespace(token);
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

char * processCharacters(char ** commandList){
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


  char * buffer = calloc(200,sizeof(char));
  int i = -1,sequenceNum =0,commandListIndex=0;
  while(1){
    int len = strlen(buffer);
    printf("\033[0;0H Current X: %ld, Current Y %d", strlen(buffer), i);
    printf("\033[%d;%dH", initialX,initialY);
    printf("%s",buffer);
    printf("\033[%d;%dH", currentX,currentY);
    char ch;
    ch = getchar();
    if(ch == 10){
      moveCursorRight(&currentX,&currentY,totalRow,totalCol,&initialX);
      printf("\n");
      break;
    }
    if (ch == 0x7f) {
      int val = moveCursorLeft(&currentX,&currentY,initialX,initialY,totalCol);
      if(val){
        if(val == 1){
          deleteIndex(&buffer,&i);
          int * point = returnPointFromMatrix(initialX, initialY, totalRow, totalCol, strlen(buffer));
          printf("\033[%d;%dH ", point[0],point[1]);
          printf("  CurrentX: %d, current y: %d", point[0],point[1]);
        }
        continue;
      }

      deleteIndex(&buffer,&i);
      int * point = returnPointFromMatrix(initialX, initialY, totalRow, totalCol, strlen(buffer));
      printf("\033[%d;%dH ", point[0],point[1]);
    }
    else if(ch==27)
        sequenceNum++;
    else if(ch==91 && sequenceNum==1)
        sequenceNum++;
    else if(sequenceNum==2){
        if(ch == 65){
          free(buffer);
          buffer = calloc(200,sizeof(char));
          int lenCommand = strlen(commandList[commandListIndex]);
          int k;
          i=-1;
          printf("\033[%d;%dH", initialX,initialY);
          for(k=0;k<lenCommand;k++){
            addIndex(&buffer,&i,commandList[commandListIndex][k]);
            moveCursorRight(&currentX,&currentY,totalRow,totalCol,&initialX);
          }
          //int * point = returnPointFromMatrix(initialX, initialY, totalRow, totalCol, strlen(buffer));
          //currenX = point[0];
        }
          commandListIndex++;
        if(ch == 66)
          printf("\nDOWN KEY\n");
        if(ch == 67){
          int len = strlen(buffer);
          if(i<len-1){
            moveCursorRight(&currentX,&currentY,totalRow,totalCol,&initialX);
            i++;
          }

        }
        if(ch == 68){
          if(moveCursorLeft(&currentX,&currentY,initialX,initialY,totalCol)!=2){
            i--;
          }

        }
        sequenceNum = 0;
    }
    else {

      if(! iscntrl(ch)){
        //printf("%c",ch);
      }
      else{
        //printf("%d",ch);
      }
      moveCursorRight(&currentX,&currentY,totalRow,totalCol,&initialX);

      sequenceNum = 0;
      addIndex(&buffer,&i,ch);
    }
  }
  revertTermios(goodTermy);
  i++;
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
void deleteIndex(char ** buffer, int * index){
  if(*index == strlen(*buffer)-1){
    //printf("%s","HI");
    (*buffer)[*index] = 0;
  }
  else{
    (*buffer)[*index] = 0;
    strcat(*buffer,*buffer+*index+1);
  }
  *index = *index - 1;
}
void addIndex(char ** buffer, int * index, char value){
  *index = *index+1;
  if(strlen(*buffer)==0){
    (*buffer)[*index] = value;
  }
  else if(*index == strlen(*buffer)){
    //printf("%s","HI");
    (*buffer)[*index] = value;
  }
  else{
    char storage = (*buffer)[*index];
    (*buffer)[*index] = 0;
    char * newString = malloc(3*sizeof(char));
    newString[0] = value;
    newString[1] = storage;
    newString[2] = 0;

    char *secondString = malloc(strlen(*buffer+*index+1)+1);
    strcpy(secondString,*buffer+*index+1);
    strcat(*buffer,newString);
    strcat(*buffer,secondString);
    //printf("\n");
    //printf("X%sX",*buffer);
    //printf("Y%sY",newString);
    //printf("Z%sZ",secondString);
    free(newString);
    free(secondString);
    //printf("buffer: %s", *buffer);
  }
}
int * returnPointFromMatrix(int initialX, int initialY, int totalRow, int totalCol, int len){
  int * point = malloc(2*sizeof(int));
  point[0] = (initialY+len)/totalCol + initialX;
  point[1] = (initialY+len)%totalCol;
  if(point[1]==0){
    point[0]--;
    point[1]+=totalCol;
  }
  return point;
}
void insertBeginning(char *** multiArray, char * command, int lastIndex){
  char ** twoArray = multiArray[0];
  int i =0;
  for(i=lastIndex;i>0;i--){
    if(i==0){
      int zz =5;
    }
    strcpy(twoArray[i],twoArray[i-1]);
  }
  strcpy(twoArray[0],command);
}
