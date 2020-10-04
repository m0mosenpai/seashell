// For a basic shell, we are going to accept commands as user input
// Call fork to create a child process.
// Execute the command in the child process while the parent waits for the command to complete.
// Return back to Step-1

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define RL_BUFSIZE 1024
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"

// Forward declaring functions
int seash_cd(char **args);
int seash_help(char **args);
int seash_exit(char **args);
int seash_execute(char **args);
int seash_launch(char **args);
int seash_loop(void);

// Reading input character by character without readline().
char *read_line(void) {
	int bufsize = RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;

	if (!buffer) {
		fprintf(stderr, "seash: allocation error.\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		// Read a character from the input
		c = getchar();

		// If we hit EOF, replace it with a null character and return.
		if (c == EOF || c == '\n') {
			buffer[position] = '\0';
			return buffer;
		}
		else {
			buffer[position] = c;
		}
		position ++;

		// If we exceed buffer, allocate more space
		if (position >= bufsize) {
			bufsize += RL_BUFSIZE;
			buffer = realloc(buffer, bufsize);
			if (!buffer) {
				fprintf(stderr, "seash: allocation error.\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	return buffer;
}

// Parsing read input
char **split_line(char *line) {
	int bufsize = TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if (!tokens) {
		fprintf(stderr, "seash: allocation error.\n");
		exit(EXIT_FAILURE);
	}

	// Get the first token in the line entered.
	token = strtok(line, TOK_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		position ++;

		// Expand buffer size if inputted characters exceed the amount of allocated memory.
		if (position >= bufsize) {
			bufsize += TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "seash: allocation error.\n");
				exit(EXIT_FAILURE);
			}
		}
		// Get newtoken by taking the string from where the first NULL is found after the previous call.
		// strtok() stores pointer in static variable so it remember's the string you entered the first time.
		// Hence, everytime you call it with NULL, it continues from where it left off.
		token = strtok(NULL, TOK_DELIM);
	}
	// Null terminate the final parsed command.
	tokens[position] = NULL;
	return tokens;
}

// List of builtin commands followed by their functions
char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};

// Array of function pointers that takes an array of strings and returns an int.
int (*builtin_func[]) (char **) = {
	&seash_cd,
	&seash_help,
	&seash_exit
};

// Returns the number of builtin functions
int seash_num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}

// Making the shell support cd
// cd is not a system program and even if it does work, it's executed in a child process due to how the program flow is in a shell.
// As a result, in the main process, we are still in the current directory.
int seash_cd(char **args) {
	if (args[1] == NULL) {
		fprintf(stderr, "seash: expected argument to \"cd\"n");
	}
	else {
		if (chdir(args[1]) != 0) {
			perror("seash");
		}
	}
	return 1;
}

// help command implementation
int seash_help(char **args) {
	int i;
	printf("Seashell: A simple Unix Shell in C.\n");
	printf("This was made as a small project for Operating Systems course in University.\n\n");
	printf("Type program names and arguments and hit enter.\n");

	for (i = 0; i < seash_num_builtins(); i ++) {
		printf("[*] %s\n", builtin_str[i]);
	}

	printf("Use the man command for information on other programs.\n");
	return 1;
}

// exit command implementation
int seash_exit(char **args) {
	return 0;
}

// Compares input with each possible supported command and executes it.
int seash_execute(char **args) {
	int i;

	if (args[0] == NULL) {
		// Empty command was entered
		return 1;
	}

	for (i = 0; i < seash_num_builtins(); i ++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}

	return seash_launch(args);
}

// Main process control flow of the shell
int seash_launch(char **args) {
	pid_t pid;
	int status;

	pid = fork();

		if (pid == 0) {
			// Child process

			// Restoring default behaviour of SIGINT in child process after forking.
			// This has to be done before execvp() system call because that replaces the entire program with new one that's to be executed.
			signal(SIGINT, SIG_DFL);

			// Never returns if call is successful
			// If -1 is returned, execution as failed.
			if (execvp(args[0], args) < 0){
				perror(args[0]);
				// This exit call only terminates the child process
				exit(1);
			}
		}
		// If OS runs out of memory or reaches the max number of allowed processes, a child process will not be created and it will return -1.
		else if (pid < 0) {
			perror("seash: fork Failed.\n");
			// This exit call terminates the entire program.
			exit(1);
		}
		else {
			// Parent process
			// WIFSIGNALED return true when child exits. This macro is always successful.
			// WIFEXITED returns true if child exited normally
			do {
				waitpid(pid, &status, WUNTRACED);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
	return 1;
}

// while loop that takes in the input for the shell
int seash_loop(void) {
	char *line;
	char **args;
	int status;

	do {
		printf("\033[1;31mseash>\033[0m ");
		line = read_line();
		args = split_line(line);
		status = seash_execute(args);

		free(line);
		free(args);
	}
	while (status);

}

// Main function
int main(int argc, char **argv) {
	seash_loop();

	return EXIT_SUCCESS;
}