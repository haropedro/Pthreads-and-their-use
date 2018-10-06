/****************************************************************
 * Name        : Harold Pedroso                                 *
 * Class       : CSC 415                                        *
 * Date        : 10/06/18                                       *
 * Description :  Writting a simple bash shell program          *
 *                that will execute simple commands. The main   *
 *                goal of the assignment is working with        *
 *                fork, pipes and exec system calls.            *
 ****************************************************************/

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include <stdbool.h> // true, false
#include <sys/wait.h> // waitpid

#define BUFFERSIZE 256
#define PROMPT "myShell >> "
#define PROMPTSIZE sizeof(PROMPT) 

int main(int* argc, char** argv)
{

  char buffer[BUFFERSIZE];
  // Stores the commands entered by user (last value has to be
  // NULL, so the size is 1 larger than the max # of tokens)
  char *args[PROMPTSIZE + 1];

  while (true)
  {
   printf("%s ", PROMPT);
   fgets(buffer, BUFFERSIZE, stdin);

   char *pos;
   if((pos = strchr(buffer, '\n')) != NULL){
   *pos = '\0';
    }

   if(strcmp(buffer, "exit") == 0) {
	  break; // exit shell if user inputs "exit"
   }
        int in, out, saved_in, saved_out; // fds
		char *infile, *outfile;
		bool input, output, output2, output_err, output_err2, background;
		input = output = output2 = output_err = output_err2 = background = false;
		
		// strtok_r breaks the inputted string into tokens, stored in args[]
		char *save_ptr;
		char *next_ptr = strtok_r(buffer, " ", &save_ptr);
		
		for (int i = 0; i <= PROMPTSIZE; i++)
		{
			if (next_ptr != NULL)
			{
				if (strcmp(next_ptr, "<") == 0)
				{
					input = true;
					
					// make all args null after redirect symbol
					for (int j = i; j <= PROMPTSIZE; j++) {
						args[j] = NULL;
					}
					
					// next token must be filename
					next_ptr = strtok_r(NULL, " ", &save_ptr);
					infile = next_ptr;
					
					// get next token
					next_ptr = strtok_r(NULL, " ", &save_ptr);
					
					// break if token is null, since there is nothing after
					if (next_ptr == NULL) {
						break;
					}
				}
				if (strcmp(next_ptr, ">") == 0 || strcmp(next_ptr, ">>") == 0
						|| strcmp(next_ptr, "2>") == 0 || strcmp(next_ptr, "2>>") == 0)
				{
					if (strcmp(next_ptr, ">") == 0) {
						output = true;
					} else if (strcmp(next_ptr, ">>") == 0) {
						output2 = true;
					} else if (strcmp(next_ptr, "2>") == 0) {
						output_err = true;
					} else if (strcmp(next_ptr, "2>>") == 0) {
						output_err2 = true;
					}
					
					// repeat same process as for input
					for (int j = i; j <= PROMPTSIZE; j++) {
						args[j] = NULL;
					}
					next_ptr = strtok_r(NULL, " ", &save_ptr);
					outfile = next_ptr;
					next_ptr = strtok_r(NULL, " ", &save_ptr);
					if (next_ptr == NULL) {
						break;
					}
				}
				if (strcmp(next_ptr, "&") == 0) // '&' is guaranteed to be last token
				{
					background = true;
					for (int j = i; j <= PROMPTSIZE; j++) {
						args[j] = NULL;
					}
					next_ptr = strtok_r(NULL, " ", &save_ptr);
					break;
				}
			}
			args[i] = next_ptr;
			
			if (next_ptr==NULL||output||output2||output_err||output_err2||background) {
				break;
			}
			next_ptr = strtok_r(NULL, " ", &save_ptr);
		}
		args[PROMPTSIZE] = NULL; //PROMPTSIZE is Null
		
		if (input)
		{
			in = open(infile, O_RDONLY);
			saved_in = dup(STDIN_FILENO); // save fd of STDIN
			dup2(in, STDIN_FILENO); // standard input is now to file
			close(in); // in's fd no longer needed
		}
		if (output || output2)
		{
			// O_WRONLY: write; O_TRUNC: truncate; O_APPEND: append; O_CREAT: create file
			// I_IRUSR, S_IWUSR: read & write for owner
			// I_IRGRP, S_IWGRP: read & write for group
			if (output) {
				out = open(outfile,O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
			} else {
				out = open(outfile,O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
			}

			saved_out = dup(STDOUT_FILENO); // save fd of STDOUT
			dup2(out, STDOUT_FILENO); // standard output is now to file
			close(out); // out's fd no longer needed
		}
		else if (output_err || output_err2)
		{
			if (output_err) {
				out = open(outfile,O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
			} else {
				out = open(outfile,O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
			}
			
			saved_out = dup(STDERR_FILENO); // save fd of STDERR
			dup2(out, STDERR_FILENO); // standard error is now to file
			close(out); // out's fd no longer needed
		}
		
		pid_t pid;
		pid = fork();
		
		if (pid == 0) // child
		{
			int exec_result = execvp(args[0], &args[0]); // execute commands
			if (exec_result < 0) {
				printf("An error occurred while executing %s\n", args[0]);
				exit(1);
			}
			exit(0); // child exits
		}
		else // parent
		{
			if (input) {
				dup2(saved_in, STDIN_FILENO); // restore old STDIN
				close(saved_in); // saved_in stored STDIN; no longer needed
			}
			if (output || output2) {
				dup2(saved_out, STDOUT_FILENO); // restore old STDOUT
				close(saved_out);
			} else if (output_err || output_err2) {
				dup2(saved_out, STDERR_FILENO); // restore old STDERR
				close(saved_out);
			}
			
			int exit_status;
			if (!background) { // if background process, omit call to wait
				pid_t wait_result = waitpid(pid, &exit_status, 0);
				if (wait_result < 0) {
					printf("An error occurred while waiting for process %d\n", pid);
					exit(1);
				}
			}
		}
		
	} // end while
    
   
  
return 0;
}








