// A program flow is interrupted on receiving a signal. When this occurs, execution of the program is passed onto the signal handler.
// Signal handler is a pre-defined function which is invoked when the signal is received.
// This small program explains the working of a signal handler.

#include <stdio.h>
#include <signal.h>

void sigint_handler(int signo) {
	printf("Caught SIGNINT\n");
}

int main() {
	// SIGINT is a predefined int for different signals.
	// Second argument is pointer to signal handler.
	signal(SIGINT, sigint_handler);
	while (1);
}