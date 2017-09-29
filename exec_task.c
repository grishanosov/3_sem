#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

int main (int argc, char *argv[])
{
	if (argc != 2) {
		printf ("Undeclareted arguments, use %s 'name of file' to run this task"
			" %s test, for example\n", argv[0], argv[0]);
		return -1;
	}
	FILE *fd = fopen(argv[1], "r");
	if (fd == NULL) {
		perror ("Can't open file\n");
		return -1;
	}
	int status = 0;
	size_t len = 0;
	char buf[256];
	char *args [256];
	char *line = NULL;
	char *tmpptr = NULL;
	ssize_t read = getline (&line, &len, fd);
	int num_of_progs = atoi (line);
	int sec = 0;
	for (int i = 0; i < num_of_progs; i++) {
		read = getline (&line, &len, fd);
		args[0] = (char *) calloc (256, sizeof (char));
		len = sscanf (line, "%s %s", buf, args[0]);
		tmpptr = line + strlen(args[0]) + strlen(buf) + 1;
		sec = atoi (buf);
		int j = 1;
		while (sscanf (tmpptr, "%s", buf) != EOF) {
			args[j] = (char *) calloc (256, sizeof (char));
			strcpy (args[j], buf);
			tmpptr += strlen (args[j]) + 1;
			j++;
		}
		args[j] = NULL;
		if (fork() == 0) {
			sleep (sec);
			int retv = execvp (args[0], args);
			if (retv == -1) {
				perror ("exec error");
				return -1;
			}
			goto end;
		}
	}
	for (int i = 0; i < num_of_progs; i++) {
		wait(&status);
	}
end:
	free (line);
	fclose (fd);
	return 0;
}
