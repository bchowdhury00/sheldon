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
	  changes back the terminal to cannonical mode. (process characters line at a time)
	  Inputs:
	  	struct termios termy(struct of old/good terminal to revert to)
	/*========== void changeTermios(struct termios * termy);
	  changes the terminal to non-cannonical mode (processes characters one character at a time)
	  Inputs:
	  	struct termios termy(stores old original termios struct for later reversion)
	  ==================*/
	/*========== void getCursorXY(int * x, int * y);
	  gets the coordinates of the cursor
	  Inputs:
	  	int * x (stores the x-coordinate here)
		int * y (stores the y-coordinate here)
	  ==================*/
	/*========== void moveCursorRight(int * currentX, int * currentY, int totalRow, int totalCol, int  * initialX);
	  moves the cursor one unit to the right, sometimes creates new lines
	  Inputs:
	  	int * currentX - takes pointer to currentX coordinate of cursor
		int * currentY- takes pointer to currentY coordinate of cursor
		int totalRow - takes total num of rows of screen
		int totalCol - takes total num of columns of screen
		int * initialX - takes initial X coordinate of cursor. 
	  Basically the system is to find the initial coordinate, set the current coordinate to that, and then manually edit the current 	   coordinate.
	  ==================*/
	/*========== int moveCursorLeft(int * currentX, int * currentY, int initialX, int initialY,int totalCol);
	 moves the cursor one unit to the right, sometimes creates new lines
	 Inputs:
	  	int * currentX - takes pointer to currentX coordinate of cursor
		int * currentY- takes pointer to currentY coordinate of cursor
		int initialX - takes initial X coordinate of cursor. 
		int initialY- takes initial Y coordinate of cursor. 
		int totalCol - takes total num of columns of screen
	  Return value has 3 pos values: 0,1,2. 2 means the cursor can't move left and sends an alert. 1 means it has to traverse a new           line backwards. 0 is just regularly moving backward. 
	 ==================*/
	/*========== void deleteIndex(char ** buffer, int * index);
	  removes element from buffer at index. 
	  combined with backspace.
	  Inputs: 
	  	char ** buffer (command line)
	  	int * index (where to remove character)
	  ==================*/	
	/*========== int * returnPointFromMatrix(int initialX, int initialY, int totalRow, int totalCol, int len);
	  returns array of length 2 that when an initial coordinate and how many characters have been put down, calculates the final 	           coordinates of the cursor
	  Inputs:
	  	int initialX (initial x-cor of cursor)
		int initialY (initial y-cor of cursor)
		int totalRow (total rows of screen)
		int totalCol (total Col of screen)
		int len(number of characters to move forward)
	  Output:
	  	array of length 2 with x-cor and y-cor of final cursor position
	  ==================*/
	/*========== void addIndex(char ** buffer, int * index, char value);
	   inserts character into buffer at index with a value specified. EX: place 'c' at index 5 of buffer.
	   Inputs:
	   	char ** buffer(command line)
		int * index (where to insert new character)
		char value (what value is the new character)
	   ==================*/
	/*========== void insertBeginning(char *** multiArray, char * command, int lastIndex);
	  Prepends command (a string) to beginning of array of strings
	  Inputs:
	  	char *** multiArray (pointer to array of strings or commands in this case)
		char * command (string of command to append)
		int lastIndex (last Index filled up in multiArray)
	  ==================*/
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

