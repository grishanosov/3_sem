#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <fcntl.h>
#include <pthread.h>

#include "matrix.h"

#define MAX_FILE_NAME 256

typedef struct {
	int as;
	int ae;
	matrix* a;
	matrix* b;
	matrix* c;
	pthread_t pid;
	size_t size;
	size_t th_num;
} m_thread_info;

void mul (m_thread_info* th_info)
{
	int as = th_info->as;
	int ae = th_info->ae;
	int sum = 0;
	int size = th_info->a->size;
	for (int i = as; i <= ae; i++) {
		for (int k = 0; k < size; k++) {
			for (int j = 0; j < size; j++) {
				sum += th_info->a->m[i][j] * th_info->b->m[j][k];	
			}
			th_info->c->m[i][k] = sum;
			sum = 0;
		}
	}
}

void* parallel_calc (void* arg)
{
	m_thread_info* th_info = (m_thread_info*) arg;
   
	mul (th_info);
	pthread_exit (NULL);
}

int prepare_and_start (matrix* a, matrix* b, matrix* c, int num)
{	
	if (a == NULL || b == NULL || c == NULL) {
		errno = EINVAL;
		return FAIL;
	}
	
	struct timespec start_time;
	struct timespec end_time;

	m_thread_info* th_info = (m_thread_info*) calloc (num, sizeof (m_thread_info));
	if (th_info == NULL) 
		return FAIL;
	
	int rows_per_th = (a->size + num / 2) / num;
	for (int i = 0; i < num; i++) {
		th_info[i].as = i * rows_per_th;
		th_info[i].ae = (i + 1) * rows_per_th - 1;
		if (i == num - 1 && th_info[i].ae != a->size - 1)
			th_info[i].ae = a->size - 1;
		th_info[i].size = rows_per_th;
		th_info[i].a = a;
		th_info[i].b = b;
		th_info[i].c = c;
		th_info[i].th_num = i;
	}

	clock_gettime(CLOCK_REALTIME, &start_time);
	
	for (int i = 0; i < num; i++) {
		pthread_create (&(th_info[i].pid), NULL, parallel_calc, &th_info[i]);
	}

	for (int i = 0; i < num; i++) {
		void* ret = NULL;
		pthread_join (th_info[i].pid, &ret);
	}

	clock_gettime(CLOCK_REALTIME, &end_time);

	double accum = ( end_time.tv_sec - start_time.tv_sec )
					  + ((double)( end_time.tv_nsec - start_time.tv_nsec ))
								  / 1000000000L;
	printf("time usage: %lf\n", accum);
	
	return SUCCESS;
}

int main(int argc, char* argv[])
{
	if (argc < 5) {
		printf ("Few arguments. Please write -n for threads numbers, -f for file name and -s for matrix size\n"); 
		return -1;
	}

	int opt = 0;
	char err = 0;
	int n = 0;
	int size = 0;
	char filename[MAX_FILE_NAME];
	int fd = 0;

	while ((opt = getopt (argc, argv, "s:n:f:")) != -1) {

		switch (opt) {

			case 'n':
				sscanf (optarg, "%d%c", &n, &err);
				if (n <= 0 || err != '\0') {
					printf ("Incorrect number of threads\n");
					return -1;
				}
				break;

			case 'f':
				sscanf (optarg, "%s", filename); 
				fd = open (filename, O_RDONLY);
				if (fd == -1) {
					printf ("Incorrect name of file\n");
					return -1;
				}
				break;

			case 's':
				sscanf (optarg, "%d%c", &size, &err);
				if (size < 0 || err != '\0') {
					printf ("Incorrect size of matrix\n");
					return -1;
				}
				break;
		}
	}


	matrix a;
	matrix b;
	matrix c;
	m_create (&a, size);
	m_create (&b, size);
	m_create (&c, size);
	m_read (&a, fd);
	m_read (&b, fd);
	//m_print (&a);
	//m_print (&b);
	prepare_and_start (&a, &b, &c, n);
	//m_print (&c);
	m_clear (&a);
	m_clear (&b);
	m_clear (&c);
	close (fd);

	return 0;
}
