# CSC 415 - Project 3 - My Shell

## Student Name: Harold Pedroso

## Student ID: 915736450

## Build Instructions:
Download all the files from the repo, after that extract the files and open a kernel or terminal.
The folder is going to be named csc415-p3-haropedro-master, after this open a terminal and cd to the folder. Later type gcc -I -Wall myshell.c -o myshell on the terminal and ./myshell to execute the program. After you are inside the shell, runs all the commands like in the actual linux shell. The shell will show you the current directory you are working on the left. The shell can take a lot of arguments but this cannot be greater than 8. Once you execute a command like ls -l, the shell will show you the command you specified, the numbers of arguments and argc and it will perform the task. 

## Run Instructions:
The shell works in the following way, you type a command in the linux kernel and it does the function that you say it supposed to do. It activates a fork process in which a parent and child carries out the functions inside the shell. 

To run the shell type gcc -I -Wall myshell.c -o myshell and later type ./myshell

## List Extra Credits completed (if non attempted leave blank)

The extracredit I implemented is the one that shows the current working directory in the prompt. 
