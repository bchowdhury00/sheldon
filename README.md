# sheldon
by Biraj Chowdhury and Caleb Kahan

Features:

	-Forks and executes commands!
	
	-Parses multiple commands on one line! (some features such as history are not implemented on it though)
	
	-Redirects using >, <!
	
	-Can use 1 pipe 
	
	-Can go up and down to look at previous commands- history preserves across sessions with a file called commands 
	
	-Moving cursor left and right with arraow keys
	
	-alerts- when trying to move your cursor where uou are not allowed to do so (turn sound on)
	
	-Implemented backspace, and isnertion of characters in non-canonical mode. 
	
	-Replace ~/ with home directory
	
	-you can typle in ~ and the shell will recognize what it means
	
	-white space doesn't matter
	
	-exit and cd are implemented- can move over directories
	
	-prompt is exactly the same as prompt in regular shell
	
	-
  
Attempted:
	
	Multiple pipes, could not figure out how to get that working 
	
	Tabs- autocomplete- not enough time
	
Bugs: We changed the terminal to non-canonical mode, and handled all keypresses ourselves. As a result, pressing certain keys may not have the desired consequences. However, all major keys were handled.

  
Files and Function Headers:
main.c 



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
	  
	/*========== void insertString(char ** buffer, int indexTilde, char * value, int newSize);
	  Generic function to help insert substring inside string
	  Makes overall string lenghh bigger
	  Used to replaced tildes inside buffer with actual path before running execvp
	  Inputs:
	  	char ** buffer (command entered)
		int indextilde (location of ~)
		char * value (home directory)
		int newSize (how big copy of buffer should be)
	   =================*/
	   
	/*========== void replaceExtraStringWithTilde(char ** buffer, char * valueToSearch, int newSize);
	  Helps shortens lengthy directory in prompt with substituting tilde for home directory. 
	  Inputs:
		char ** buffer(current working directory)
		char * valueToSearch - (current home directory)
		int newSize (how big copy of buffer should be)

	  =================*/

