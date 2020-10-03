// In order to execute commmands, we are going to use exec family of system calls.
// execvp() accepts the name of a file, for which it searched the $PATH variable of the system.

#include <unistd.h>

int main() {
	char *argv[] = {"ls", "-l", "-h", "-a", NULL};
	// The first argument is name of the command
	// Second argument consists of the name of the command and the arguments passed to the command. Also terminated by NULL.
	execvp(argv[0], argv);

	return 0;
}