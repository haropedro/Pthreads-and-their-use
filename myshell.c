/****************************************************************
 * Name        : Harold Pedroso                                 *
 * Class       : CSC 415                                        *
 * Date        : 10/20/18                                       *
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
#include <inttypes.h>
#include <sys/wait.h> // waitpid



#define BUFFERSIZE 256
#define PROMPT "myShell>> "
#define PROMPTSIZE sizeof(PROMPT) 


char *token, **myargv, str[BUFFERSIZE];
int myargc;


void displayPrompt();
int getInput();
int executeMain();
int redirectFile(int redirectId, int mode);
int fnPipe(int redirectId);
int RunBackground();


int main(int* argc, char** argv){
char *shell, cwd[256];

int rtnCd, redirectId;


while(1){
//while(true){

displayPrompt();
rtnCd = getInput();
if (rtnCd == -1){
continue;
}
shell = myargv[0];
if (strcmp(shell, "exit")== 0){
// printf("** exiting **\n");
return 0;
}

//This will add cd to the shell. 
if (strcmp(shell, "cd")== 0){
chdir(myargv[1]);
continue;
}
//This will the path to shell
if (strcmp(shell, "pwd")== 0){
printf("%s",getcwd(cwd, BUFFERSIZE));
continue;
}

redirectId=0;
for (int i=0;i<myargc;i++){
if (strcmp(myargv[i],">") == 0){
redirectId=i;
redirectFile(redirectId,0);
}
if (strcmp(myargv[i],">>") == 0){
redirectId=i;
redirectFile(redirectId,1);
}
if (strcmp(myargv[i],"<") == 0){
redirectId=i;
redirectFile(redirectId,2);
}
if (strcmp(myargv[i],"|") == 0){
redirectId=i;
fnPipe(redirectId);



}
if (strcmp(myargv[i],"&") == 0){
redirectId=myargc;
RunBackground();
}

}
if (redirectId != 0){
continue;
}

rtnCd = executeMain();
if (rtnCd == -1) {
continue;
}
}
}
//************************************************************
//************************************************************
void displayPrompt(){
char cwd[256], newPrompt[256], s[256];

getcwd(cwd, BUFFERSIZE);
/*
These functions return a null-terminated string containing the absolute path-name that is the current working directory of the callling process. 
*/


printf("\n %s %s %s", "MyShell", cwd,">>" );
if (PROMPTSIZE > 256){
printf("PROMPT exceeds limits specified");
}
}

//***************************************************************
//***************************************************************


//The getInput functions will get all the inputs needed to run the program and it will displays the number of argumemnts and the arg count. 
int getInput(){

char *tokens;
int length;
myargv = (char **)malloc(6* sizeof(char*));

fflush(stdin);
fgets(str, BUFFERSIZE, stdin);

length = strlen(str);
if (length == 1){
printf("Error: input is empty.\n");
return -1;
}
if(length > 256){
printf("Error: input length exceeds limit of 256.\n");
return -1;
}

//tokenizing
myargc=0;
tokens = strtok(str, " \t\n");
myargv[myargc++] = (char *)tokens;

while(1){
//while(true)
tokens = strtok(NULL, " \t\n");
if (!tokens){
break;
}
myargv[myargc++] = (char *)tokens;
}

printf("****** %s\n",str);
for (int i=0;i<myargc;i++){
printf("****** argument no %d = %s\n",i,myargv[i]);
}
printf("*****  argc= %d\n\n",myargc);



if (myargc > 8){
printf("Too many arguments on the input stream .");
return -1;
}

}
//***********************************************************

int executeMain(){

int pid, rtnCd;

pid = fork();
if (pid == 0){
rtnCd = execvp(myargv[0], myargv);
if (rtnCd < 0){
printf("Error: An error occurred executing %s\n", myargv[0]);
return -1;
}
}else{
waitpid(pid, NULL, 0);
}
}
//****************************************************
int redirectFile(int redirectId, const int mode){
//
int fd,cpid,dupRtCd;
//dup_rtn_cd
char **myargv1 = (char **)malloc(6* sizeof(char*));

if (mode == 0){
fd = open(myargv[redirectId+1], O_CREAT | O_WRONLY | O_TRUNC, 00777 );
} else if (mode == 1){
fd = open(myargv[redirectId+1], O_CREAT | O_WRONLY | O_APPEND, 00777 );
} else if (mode == 2){
fd = open(myargv[redirectId+1], O_RDONLY);
}

if (fd < 0){
printf("Error: could not open file, %s",myargv[redirectId+1]);
}

if (mode <= 1){
dupRtCd = dup(1);
dup2(fd,1);
}else{
dupRtCd = dup(0);
dup2(fd,0);
}
close(fd);

for (int i=0;i<redirectId;i++){
myargv1[i] = myargv[i];
}

cpid = fork();
if (cpid == 0){
if (execvp(myargv1[0], myargv1) < 0){
perror("Error in executing redirection of files.");
}
}else{
waitpid(cpid, NULL, 0);
}
fflush(stdout);
close(fd);
if (mode <= 1){
dup2(dupRtCd,1);
}else{
dup2(dupRtCd,0);
}
close(dupRtCd);
}


//********************************************************
int fnPipe(int redirectId){

int pipefd[2];
pid_t pid1,pid2;
char **myargv1 = (char **)malloc(6* sizeof(char*));
char **myargv2 = (char **)malloc(6* sizeof(char*));

for (int i=0;i<redirectId;i++){
myargv1[i] = myargv[i];
}
for (int i=0,j=redirectId+1;j<myargc;i++,j++){
myargv2[i] = myargv[j];
}

if (pipe(pipefd) < 0) {
perror("Pipe couldnt be executed");
return -1;
}
pid1 = fork();
if (pid1 < 0) {
perror("Fork couldnt be executed ");
return -1;
}
printf("forked");
if (pid1 == 0) {
dup2(pipefd[0],0);
close(pipefd[0]);
close(pipefd[1]) ;
printf("piping in child /n"); 
//Close unused write end

if(execvp(myargv2[0], myargv2)< 0){
printf("Pipe could not be executed error.");
return -1;
}
}else if (pid1 > 0){
dup2(pipefd[1], 1);
close(pipefd[0]);
close(pipefd[1]) ;
if (execvp(myargv1[0], myargv1) < 0){
printf("Pipe could not be executed error.");
return -1; // ?? check
}
wait(NULL);
}
return 0;

}


//******************************************************************
int RunBackground(){
int count=1;
pid_t pid;

myargv[myargc-1] = NULL;

pid = fork();
if (pid == 0){
if (execvp(myargv[0], myargv) < 0){
perror("error");
return -1;
}
}else{
printf("[ %d ]", getpid());
//This will returns the process ID of the calling process

if (!count){
waitpid(pid, NULL, 1);
return -1;
}
count = 0;
}
}











































