# CSC 415 - Project 5 - Producer Consumer - Part 2

## Student Name : Harold Pedroso

## Student ID : 915736450

## Build Instructions

The build instructions are to download the repo and clone it. Later read the instructions of how a producer and consumer program works.



## Run Instructions
 Later open a terminal or kernel and type of gcc -I -Wall pandc.c -o pandc -lpthread. Also, read the pandcpseudo.txt file bfore starting to run the program. After this the code will compile. Later type ./pandc to run the program. If you type this the kernel will complaiun that you did not eneter the right amounts of arguments because its waiting for the number of buffers, producers, consumers, items to be produce, ptime and ctime, or something lke this to test the utility of the program: ./pandc 5 5 5 16 1 1 


Testing Strategy:

The testing strategy is to have a single atomic counter (i.e. a counter with
mutex synchronization so it is guaranteed to produce different numbers) to generate
numbers for Producer threads, then have the main routine combine the output from
all of the Consumer threads, sort it and verify that all of the input numbers appeared
as output.




