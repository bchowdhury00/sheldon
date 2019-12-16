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
#include "header.h"

int main(){
  char login_name [100];
  getlogin_r(login_name,100);
  char host_name[100];
  gethostname(host_name,100);

  FILE *commandsStream = fopen("commands","r");
  char ** commandList = malloc(100*sizeof(char *));
  int j;
  for(j=0;j<100;j++){
    commandList[j] = malloc(100*sizeof(char));
  }
  int i = 0;
  int limit = 99;
  //Checking if actual file
  if(commandsStream){
    while (fgets(commandList[i], sizeof(commandList[i]), commandsStream)){
      commandList[i][strlen(commandList[i])-1] = 0;
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
    char ** newCommandList = malloc(100*sizeof(char *));
    for(j=0;j<limit+1;j++){
      newCommandList[j] = malloc(100*sizeof(char));
    }
    int k;
    for(k=0;k<i;k++){
      newCommandList[k] = commandList[i-k-1];
    }
    free(commandList);
    commandList = newCommandList;
  }
  //Creating file if not there
  int commandsFile = open("commands",O_WRONLY | O_CREAT | O_APPEND,0755);
  while(1){
    char * currentDir = malloc(100*sizeof(char));
    getcwd(currentDir, 100);
    replaceExtraStringWithTilde(&currentDir,getenv("HOME"),100);
    printf("%s@%s:%s$ ",login_name, host_name, currentDir);
    free(currentDir);
    char * buffer = processCharacters(commandList, i);

    buffer = stripwhitespace(buffer);
    // caleb- ADD INSERT TO CHECK
    if (checkMultipleCmds(buffer)){
      int n = 1;
      char ** commands = multipleCmds(buffer);
      char * current = *commands;
      while (current != NULL){
      	runCmd(current);
        //write(commandsFile, buffer, strlen(buffer));
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
      int len = strlen(buffer);
      buffer[len] ='\n';
      buffer[len+1] = 0;
      write(commandsFile, buffer, strlen(buffer));
      buffer[len] = 0;
      char * bufPtr = buffer;
      while(strchr(bufPtr,'~')){
        bufPtr = strchr(bufPtr,'~');
        insertString(&buffer, bufPtr-buffer,getenv("HOME"), 200);
        bufPtr+=1;
      }
      if (existsRedirection(buffer)){
	       redirect(buffer);
	        continue;
      }
      if (isPipe(buffer)){
	       pipeCommands(buffer);
	        continue;
      }

      runCmd(buffer);
      free(buffer);
    }
  }
  close(commandsFile);

  printf("\n");
  return 0;
}
