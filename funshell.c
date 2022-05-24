#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* progr1part2b Drew Martin */
/* funny shell interface to accept user command and run in separate process */
/* commands should be entered in reverse order */

/* some supporting functions for the main method */

/* get the funny reverse command input */
/* parse the users command input */
/* using the string tokenizer */
int getRevCommand(char *buffer, char **cmdList) {
	strtok(buffer, "\n");
	char *input[100];
	int listsize = 0;
	char *tok = strtok(buffer, " ");
	
	while (tok != NULL) {
		input[listsize] = tok;
		listsize++;
		tok = strtok(NULL, " ");
	}

	cmdList[listsize] = NULL;
	int cmdCount = 0;

	for (int x = listsize-1; x >= 0; x--) {
		cmdList[cmdCount] = input[x];
		cmdCount++;
	}
	return listsize;
}

/* check for redirecting output if necessary */
/* look for and find <* if user wants to redirect and reports where it is in the list */
int redirectStdOutput(char** cmdList, int listsize) {
	for (int x = 0; x < listsize; x++) {
		if (cmdList[x][0] == '<') {
			/* found redirect symbol <* which starts where the < is found */
			return x;
		}
	}
	return -1; /* if no symbol was found */
}

/* main method */
/* prompt user then determine what needs to be done with their input */
/* nested if statements determine if background, foreground, or redirection needs to be done */

int main () {
	/* set up buffer */
	const int SIZE = 500; /* just setting it to 500 for now */
	char buffer[SIZE];
	/* introduce user to my shell */
	printf("This is the funny shell! Input commands in reverse order, or input exit to quit!\n");
	printf("Your input should be reversed and the shell will execute for you!\n");
	printf("You can use &, fg, or <* commands for different operations!\n");
	/* set up a do while loop for continous running */
	pid_t background = 0;
	do {
		pid_t currpid;
		/* print any running processes */
		while((currpid = waitpid(-1, NULL, 0)) > 0) {
			printf("Background process run (pid = %d)\n", currpid);
			background = 0;
		}
		printf("dbmart2> ");
		bzero(buffer, SIZE); /* reset buffer */
		fgets(buffer, 200, stdin); /* get input after the prompt */

		/* run until exit */
		if ((strcmp(buffer, "exit\n")) != 0 && buffer[0] != '\n') {
			int operators = 0; /* for specified operators */
			/* const char *cmdList[100]; */
			char* cmdList[100]; /* command list */
			int listsize = getRevCommand(buffer, cmdList);

			/* if statements here will support functionality for redirection and background */
			if ((operators = redirectStdOutput(cmdList, listsize)) >= 0) {
				printf("Redirection symbol was detected!\n");
				/* if no redirection is found */
				if (operators == (listsize -1)) {
					printf("Command will not be redirected.\n");
				}
				else {
					int pid = fork(); /* fork to create child process */
					/* check if there was problem creating */
					if (pid == -1) {
						printf("Child not created, error occurred!\n");
					}
					else if (pid == 0) {
						/* printf("Redirecting, child pid is %d\n", pid); */
						
						
						/* child process */
						/* create file desc */
						int fd;
						char *fname = cmdList[operators + 1]; 
						printf("Redirecting std output to %s\n", fname);
						/* got the redirection file */
						char *cmndList[operators + 1];
						for (int x = 0; x < operators; x++) {
							cmndList[x] = cmdList[x];
						}
						/* updated command list after getting destination  */
						cmndList[operators] = NULL;

						if (( fd = open(fname, O_CREAT | O_WRONLY, 0666)) < 0) {
							printf("Unable to open redirected destination!\n");
						}
						if (dup2(fd, 1) < 0) {
							printf("Unable to redirect the standard output!\n");
						}
						if (execvp(cmndList[0], cmndList) == -1) {
							printf("Output was not redirected, error!\n");
							close(fd);
							exit(0);
						}
						/* close file descriptor */
						close(fd);
					}
					else {
						wait(NULL); /* if nothing else, wait */
					}
				}
			}
			else if (cmdList[listsize - 1][0] == '&') {
				printf("Background symbol detected!\n");
				/* this sections is for background processes */
				cmdList[listsize - 1] == NULL;
				if (background == 0) {
					pid_t pid = fork();
					if (pid == -1) {
						printf("Unable to create child, error!\n");
					}
					else if (pid == 0) {
						printf("Background process!\n");
						/* child process */
						if (execvp(cmdList[0], cmdList) == -1) {
							printf("Unable to execute, error!\n");
							exit(0);
						}
						else {
							background = pid;
							printf("Running (pid = %d) ", pid);
							for (int x = listsize - 2; x >= 0; x--) {
								printf("%s", cmdList[x]);
							}
							printf("\n");
						}
					}
					else {
						printf("Background process is currently running!\n");
					}
				}
			}
			else if (strcmp(cmdList[0], "fg") == 0) {
				printf("Foreground symbol detected!\n");
				if(background != 0) {
					/* bring process to foreground functionality */
					waitpid(background, NULL, 0);
					background = 0;
				}
				else {
					printf("No background process is currently running!\n");
					/* nothing running in background */
				}
			}
			else {
				printf("No redirection or background specified, running command now!\n");
				int pid = fork();
				/* printf("Child pid is %d\n", pid);
				for (int x = listsize - 2; x >= 0; x--) {
					printf(" %s", cmdList[x]);
				}
				printf("\n"); */
				if (pid == -1) {
						printf("Unable to create child, error!\n");
					}
					else if (pid == 0) {
						printf("Command is running...\n");
						/* child process */
						if (execvp(cmdList[0], cmdList) == -1) {
							printf("Unable to execute, error!\n");
							exit(0);
						}
					}
					else {
						wait(NULL);
					}
			}
		}
	}while(strcmp(buffer, "exit\n") != 0);
	printf("Funny shell is closed!\n");

}
