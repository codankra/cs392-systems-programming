#define _PROGRAM_NAME "msh"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> 
#include <sys/signal.h>
#include <stdio.h>
#include <pwd.h>
#include "msh.h"
#include <limits.h>
#include <sys/wait.h>
#include <ctype.h>

void sigChildHandler(int sig){
	pid_t toReap;
	int status;
	while ((toReap = waitpid((pid_t)(-1), &status, WNOHANG)) > 0) {
		printf("pid %i done\n", toReap);
	}
	

}
void sigIntHandlerP(int sig){
	write(1, "\n", 1);

}
int allSpaces(char input[1000]){
	int i = 0;
	while(input[i] != '\n' && input[i] != '\0'){
		if (!isspace(input[i])){
			return 0;
		}
		i++; 
	}
	return 1; //String is only spaces
}

int main(int argc){
	if (argc > 1){
		fprintf(stderr, "Unrecognized command-line arguments\n");
		printf("%s usage: just run the program without any extra arguments\n", _PROGRAM_NAME);
		return -1;
	}
	if(signal(SIGCHLD, sigChildHandler) == SIG_ERR)
			perror("SIGCHLD failure");
	while(1){ // Start LÖÖP here
		if(signal(SIGINT, sigIntHandlerP) == SIG_ERR)
			perror("parent SIGINT failure");
		uid_t uid = geteuid();
		struct passwd *pw = getpwuid(uid);
		char* username;
		if(pw)
			username = pw -> pw_name;
		else 
			fprintf(stderr,"%s: cannot find username for UID %u\n", 
				_PROGRAM_NAME, (unsigned) uid);
		char directory[PATH_MAX]; //defined in limits.h
		getcwd(directory, sizeof(directory)); 
		printf("SWS:%s:%s>", username, directory); //Print Prompt
		char input[1000];
		fgets(input, 1000, stdin);
		//Start input handling block
		if (strlen(input) > 1 && input[strlen(input) - 1] == '\n')                                                           
            input[strlen(input) - 1] = '\0'; //get rid of enter for comparison
        char fullcmd[1000];
        strcpy(fullcmd, input); //To preserve full command in case of future printing
        char* cmd = strtok(input, " ");
        if (allSpaces(input)){//Test whitespace and do nothing
        }
        //Decide what command
        else if (strcmp(cmd, "exit") == 0){ //exit *
        	killpg(getpid(), SIGTERM);
        	exit(0);
		}
        else if (strcmp(cmd, "cd") == 0){ //cd path *
        	char* path = strtok(NULL, " ");
        	chdir(path);
        }
        else{ 
        	//Parse arguments
        	pid_t pid;
        	char* argv[100];
        	int cur = 0;
        	while(cmd != NULL){
        		argv[cur] = cmd;
        		cur++;
        		cmd = strtok(NULL, " ");
        	}
        	if (strcmp(argv[cur-1], "&") == 0){ //run in background
        		argv[cur-1] = NULL; //remove &
        		if ((pid = fork()) < 0){
			        perror("Fork error");
			        exit(1);
			    }
        		if (pid == 0){ //child
        			signal(SIGINT, SIG_DFL); //Reinstate default handler
        			char* cmd2 = strtok(fullcmd, "&");
        			printf("pid: %i cmd: %s\n", getpid(), cmd2);
        			if(execvp(argv[0], argv) < 0){
        				perror("Execvp error");
        				exit(0);
        			}
        		}
        		else{ //parent
        		}
        	}
        	//----------------------------------------------------
        	else{ //Run in foreground!
        		argv[cur] = NULL;
        		if ((pid = fork()) < 0){
			        perror("Fork error");
                    exit(1);
			    }
        		if (pid == 0){ //child
        			signal(SIGINT, SIG_DFL); //Reinstate default handler
        			if(execvp(argv[0], argv) < 0){
        				perror("Execvp error");
        				exit(0);
        			}
        		}
        		else{ //parent
        			//printf("Started %i\n", pid);
        			waitpid(pid, NULL, 0);
        			//printf("Finished %i\n", pid);
        		}
        	}
        }
	}
}