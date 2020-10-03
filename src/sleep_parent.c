// This file explains the usage of sleep system call when made the parent sleep for 1 second.
// We make a blocking call to the parent process so the child process get's executed while the parent process waits for 1 second before showing the output.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	pid_t child_pid = fork();

	// The child process
	if (child_pid == 0) {
		printf("### Child ###\nCurrent PID: %d and Child PID: %d\n", getpid(), child_pid);
	}
	else {
		// Sleep for 1 second
		sleep(1);
		printf("### Parent ###\nCurrent PID: %d and Child PID: %d\n", getpid(), child_pid);
	}

	return 0;
}