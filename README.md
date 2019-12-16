# sheldon
by Biraj Chowdhury and Caleb Kahan

Features:
	Forks and executes commands!
	Parses multiple commands on one line!
	Redirects using >, <!
  Can use 1 pipe 
  Can go up and down to look at previous commands 
  
Attempted:
  Multiple pipes, could not figure out how to get that working 
  
Bugs:
  
Files and Function Headers
main.c :

/*========== void change_dir(char * newdir);
  changes cwd, takes directory you are attempting to switch into
  =================*/
  
/*========== char ** parse_args(char * line);
  parses through the string to find flags for a shell command;
  inputs 
  char * line - string that holds command;
======================*/

/*========== int runCmd(char * buffer);
  forks and execvps to run a command in the terminal;
  Inputs:
  char * buffer - string that holds command;
  =======================*/
  
/*========== int checkMultipleCmds(char * line);
  checks for a semicolon indicating multiple commands
  Inputs:
  char * line - string that holds command;
  ===================*/
  
/*========== char ** multipleCmds(char * line);
  runs commands seperately
  Inputs: 
  char * line - string that holds commands;
  -===================*/
 
/*========== char * processCharacters(char ** commandList,int commandListLen);
  function to process characters as they are being typed rather than recieving output when a EOL character is entered
  char ** commandList - String array holding the previous commands that were entered;
  int commandListLen - len of commands in commandList to help with adding the current command;
  ==================*/
  
/*========== void revertTermios(struct termios termy);
/*========== void changeTermios(struct termios * termy);
/*========== void getCursorXY(int * x, int * y);
/*========== void moveCursorRight(int * currentX, int * currentY, int totalRow, int totalCol, int  * initial);
/*========== int moveCursorLeft(int * currentX, int * currentY, int initialX, int initialY,int totalCol);
/*========== void deleteIndex(char ** buffer, int * index);
/*========== int * returnPointFromMatrix(int initialX, int initialY, int totalRow, int totalCol, int len);
/*========== void addIndex(char ** buffer, int * index, char value);
/*========== void insertBeginning(char *** multiArray, char * command, int lastIndex);

/*========== int redirect(char * buffer);
  Implements redirection using dup and dup2 and then runs command.
  Inputs:
   char * buffer (command entered);
   =================*/
   
/*========== int existsRedirection(char * buffer);
  checks your command for any redirection symbols;
    Inputs:
   char * buffer (command entered);
   =================*/
   
/*========== char * doubleRedirect(char * moreRedirect,int * isTrue,int * newfd, int  * oldfd, int * openFile);
  2nd level of redirection 
  allows for double redirection in our shell
  
  Inputs
  char * moreRedirect - string that holds the 2nd level of redirection after being strseped to remove first level
  int * isTrue - to store whether double redirection exists;
  int *newfd - to store file number that is going to replace stdin, stdout or stderr
  int *oldfd - to store file number that holds the duped stdin, stdout, or stderr
  char * openFile - to store name of file that is going to replace stdin,stdout, or stderr; 
  =====================*/

   
/*========== char * stripwhitespace(char * arr);
  removes any whitespace in front of or behind a string;
    Inputs:
   char * arr (string);
   =================*/

/*========== int isPipe(char * buffer);
  checks your command for a pipe (|) character.
  Inputs:
   char * buffer (command entered);
   =================*/
   
/*========== int pipeCommands(char * buffer);
  Uses popen to implement piping;
  Inputs:
   char * buffer (command entered);
   =================*/
