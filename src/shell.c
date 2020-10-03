// For a basic shell, we are going to accept commands as user input
// Call fork to create a child process.
// Execute the command in the child process while the parent waits for the command to complete.
// Return back to Step-1

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>


// Shell input parser
char **get_input(char *input) {
	// Limited command length allowed due to small memory allocation.
	char **command = malloc(8 * sizeof(char*));
	// If OS runs out of memory, malloc will fail. We will exit in such case.
	if (command == NULL) {
		perror("malloc failed");
		exit(1);
	}

	char *separator = " ";
	char *parsed;
	int index = 0;

	parsed = strtok(input, separator);
	while (parsed != NULL) {
		command[index] = parsed;
		index ++;

		parsed = strtok(NULL, separator);
	}

	command[index] = NULL;
	return command;
}

// Making the shell support cd
// cd is not a system program and even if it does work, it's executed in a child process due to how the program flow is in a shell.
// As a result, in the main process, we are still in the current directory.
int cd(char *path) {
	return chdir(path);
}

// Main function
int main() {
	char **command;
	char *input;
	// Signed int data type capable of representing a process ID.
	pid_t child_pid;
	int stat_loc;

	// Having this line here will make the parent process ignore Ctrl+C signal interrupt.
	// However during subsequent fork, current signal configurations (signal dispositions) also get copied to the child process.
	// As a result, the child process also ignore the Ctrl+C signal interrupt.
	signal(SIGINT, SIG_IGN);

	while (1) {
		input = readline("seash> ");
		command = get_input(input);

		// Check for detecting cd command
		if (strcmp(command[0], "cd") == 0) {
			// If command returns -1
			if (cd(command[1]) < 0) {
				perror(command[1]);
			}

			// Skip the fork
			continue;
		}

		child_pid = fork();

		if (child_pid == 0) {
			// Restoring default behaviour of SIGINT in child process after forking.
			// This has to be done before execvp() system call because that replaces the entire program with new one that's to be executed.
			signal(SIGINT, SIG_DFL);

			// Never returns if call is successful
			// If -1 is returned, execution as failed.
			if (execvp(command[0], command) < 0){
				perror(command[0]);
				// This exit call only terminates the child process
				exit(1);
			}
		}
		// If OS runs out of memory or reaches the max number of allowed processes, a child process will not be created and it will return -1.
		else if (child_pid < 0) {
			perror("Fork Failed.");
			// This exit call terminates the entire program.
			exit(1);
		}
		else {
			waitpid(child_pid, &stat_loc, WUNTRACED);
		}

		free(input);
		free(command);
	}

	return 0;
}