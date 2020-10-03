// Using sleep is not the best approach to control process execution flow. Hence, we use waitpid system call instead.
// waitpid() system call suspends execution of the calling process until a child specified by pid argument has changed state.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	pid_t child_pid;
	pid_t wait_result;
	int stat_loc;

	child_pid = fork();

	// The child process
	if (child_pid == 0) {
		printf("### Child ###\nCurrent PID: %d and Child PID: %d\n", getpid(), child_pid);
		// Sleep for 1 second
		sleep(1);
	}
	else {
		wait_result = waitpid(child_pid, &stat_loc, WUNTRACED);
		printf("### Parent ###\nCurrent PID: %d and Child PID: %d\n", getpid(), child_pid);
	}

	return 0;
}