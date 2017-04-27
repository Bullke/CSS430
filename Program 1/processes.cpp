////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// Nenad Bulicic                                                                           \\
// CSS 430 - Program 1: System Calls and Shell                                             \\
////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// This assignment intends (part 1) to familiarize you with Linux programming using        \\
// several system calls such as fork, execlp, wait, pipe, dup2, and close, and (part 2) to \\
// help you understand that, from the kernel's view point, the shell is simply viewed as   \\
// an application program that uses system calls to spawn and to terminate other user	   \\
// programs. Also to become familiar with the ThreadOS operating system simulator in       \\
// part 2 of this assignment.                     										   \\
////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// 					  			  PART 1: SYSTEM CALLS                                     \\
//																						   \\
// This implementation is a C++ program, named processes.cpp that receives one argument,   \\
// (i.e., argv[1]) upon its invocation and searches how many processes whose name is       \\
// given in argv[1] are running on the system where your program has been invoked.         \\
// To be specific, this program should demonstrate the same behavior as: 				   \\
//                           $ ps -A | grep argv[1] | wc -l						   		   \\
////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define READ 0
#define WRITE 1

int main(int argc, const char * argv[])
{
    int status, pipeDescriptor1[2], pipeDescriptor2[2];
    pid_t pid;
    if(argc < 2)
    {
        fprintf(stderr, "Error! Not enough arguments\n");
        exit(EXIT_FAILURE);
    }
    // Creates pipe 1
    if(pipe(pipeDescriptor1) < 0)
    {
        fprintf(stderr, "Error! Pipe 1 failed\n");
    }
    // Creates pipe 2
    if(pipe(pipeDescriptor2) < 0)
    {
        fprintf(stderr, "Error! Pipe 2 failed\n");
    }
    // Forks the process
    pid = fork();
    if(pid < 0)
    {
        fprintf(stderr, "Error! Fork failed\n");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0)  //  I am the child process
    {
    	// Forks the process
        pid = fork();
        if(pid < 0)
        {
            fprintf(stderr, "Error! Fork failed\n");
            exit(EXIT_FAILURE);
        }
        else if(pid == 0)  //  I am the grandchild process
        {
        	// Forks the process
            pid = fork();
            if(pid < 0)
            {
                fprintf(stderr, "Error! Fork failed\n");
                exit(EXIT_FAILURE);
            }
            else if(pid == 0)  //  I am the grandgrandchild process
            {
            	// Execute "ps -A" and pipe to parent
                dup2(pipeDescriptor2[WRITE], 1);
                close(pipeDescriptor2[READ]);
                close(pipeDescriptor1[WRITE]);
                close(pipeDescriptor1[READ]);
                execlp("ps", "ps", "-A", NULL);
            }
            else  //  I am the parent (of grandgrandchild) process
            {
            	// Read from child, execute "grep argv[1]" and pipe to parent
                close(pipeDescriptor2[WRITE]);
                dup2(pipeDescriptor2[READ], 0);
                close(pipeDescriptor1[READ]);
                wait(&status);
                dup2(pipeDescriptor1[WRITE], 1);
                execlp("grep", "grep", argv[1], NULL);
            }
        }
        else  //  I am the parent (of grandchild) process
        {
        	// Read from child, execute "wc -l" and pipe to parent
            close(pipeDescriptor2[WRITE]);
            close(pipeDescriptor2[READ]);
            close(pipeDescriptor1[WRITE]);
            wait(&status);
            dup2(pipeDescriptor1[READ], 0);
            execlp("wc", "wc", "-l" , NULL);
            
        }
    }
    else  //  I am the parent (of child) process
    {
        close(pipeDescriptor2[WRITE]);
        close(pipeDescriptor2[READ]);
        close(pipeDescriptor1[WRITE]);
        close(pipeDescriptor1[READ]);
        wait(&status);
    }
    return 0;
}