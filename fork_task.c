#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

int main (int argc, char *argv[])
{
	if (argc != 2) {
		printf ("Undeclareted arguments, use %s 'num of arguments' to run this task"
			" %s 3 for example\n", argv[0], argv[0]);
		return -1;
	}
	char *endptr = NULL;
	long int val = strtol (argv[1], &endptr, 10);
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) 
	|| (errno != 0 && val == 0)) {
		perror ("strtol");
		return -1;
	}
	if (endptr == argv[0]) {
		perror ("strtol");
		return -1;
	}
	int retv = 0;
	int status = 0;
	for (int i = 0; i < val; i++) {
		if ((retv = fork()) > 0) {
			printf ("New process with pid %d on the %d level"
			" has been created\n", retv, i);
			wait(&status);
			printf ("Die %d\n", getpid());
			return 0;
		}
		else if (retv < 0) {
			perror ("The fork is aborted");
			return -1;
		}
	}
	return 0;
}
