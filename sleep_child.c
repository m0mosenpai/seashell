// This file explains the usage of sleep system call when made the child sleep for 1 second.
// In this case, the parent process terminates instantly because it has nothing to do after printing out the output.
// The child process on the other hand, gets blocked for 1 second, after which the print statement is executed.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	pid_t child_pid = fork();

	// The child process
	if (child_pid == 0) {
		// Sleep for 1 second
		sleep(1);
		printf("### Child ###\nCurrent PID: %d and Child PID: %d\n", getpid(), child_pid);
	}
	else {
		printf("### Parent ###\nCurrent PID: %d and Child PID: %d\n", getpid(), child_pid);
	}

	return 0;
}