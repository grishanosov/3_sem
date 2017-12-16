#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define type 20
#define TABLE_LIMIT 4
#define N_UTENSILS 6

struct utensils
{
	char utensil[type]; 	//тип посуды
	int time;		//время время выполнения
}washer[N_UTENSILS], wiper[N_UTENSILS];

char pathname[] = "key.c"; 
key_t key;
int semid;
struct sembuf mybuf;
void SemFunc(int n_sem, int n)
{
	mybuf.sem_op = n;
	mybuf.sem_flg = 0;
	mybuf.sem_num = n_sem;
	if (semop(semid , &mybuf , 1) < 0) 
	{
		printf("Can`t wait for condition\n");
		exit(-1);
	}
}


int main()
{
	FILE* f;
	int i, j, fd[2], count, result, k = 0, sz;
	char washerName[type];
	char wiperName[type];
	if ((key = ftok(pathname, 1)) < 0)
	{
		printf("Can't generate key\n");
		exit(-1);
	}
	if ((semid = semget(key , 1 , 0666 | IPC_CREAT)) < 0) 
    	{
        	printf("Can`t get semid\n");
        	exit(-1);
	}
	semctl(semid, 0, SETVAL, 0);
	if ((f = fopen("washer.txt","r")) == NULL) //считываем из файла для мойщика в массив структур для мойщика
	{
		printf("File not found.\n");
	}
	for(i = 0; i < N_UTENSILS; i++)
	{
		fscanf(f, "%s %*c %d", washer[i].utensil, &washer[i].time);
	}
	fclose(f);
	
	if ((f = fopen("wiper.txt","r")) == NULL) //считываем из файла для протирающего в структуру для протерающего
	{
		printf("File not found.\n");
	}
	for(i = 0; i < N_UTENSILS; i++)
	{
		fscanf(f, "%s %*c %d", wiper[i].utensil, &wiper[i].time);
	}
	fclose(f);
	SemFunc(0,TABLE_LIMIT); //создаем место на столе для  посуды
	pipe(fd);
	result = fork();
	if(result > 0) // Washer
	{
		if ((f = fopen("utensils.txt","r")) == NULL) //открываем фаил
		{
			printf("File not found.\n");
		}
		while(fscanf(f, "%s %*c %d", washerName, &count) != EOF)  //читаем строки до конца(washerName - название посуды, n - кол-во)
			for(i = 0; i < N_UTENSILS; i++)  //перебираем название посуды мойщика и сравниваем с читаемым типом 
				if(strcmp(washerName, washer[i].utensil) == 0) 
				{
					for(j = 0; j < count; j++)  //делаем для n посуд этого типа
					{
						sleep(washer[i].time); //моем посудy
						SemFunc(0, -1);  //занимаем место на столе
						printf("Мойщик помыл %s за %d секунды\n", washer[i].utensil, washer[i].time);
						write(fd[1], washer[i].utensil, type); //пишем название типа в pipe
					}
					break;
			}
		int o =	0; // сигнал конца
		write(fd[1], &o, sizeof(int));
		fclose(f);
	int zero = 0;
	wait(&zero);// ожидание
	}
	else // Wiper
	{
		while(sz != sizeof(int))
		{	
	
			sz = read(fd[0], wiperName, type);

			for(i = 0; i < N_UTENSILS; i++)  //перебираем название посуды вытиральщика и сравниваем с читаемым типом
				if(strcmp(wiperName, wiper[i].utensil) == 0) 
					{
						SemFunc(0,1); //добавляем место на столе (берем со стола)				
						sleep(wiper[i].time); //вытираем посуду
						printf("Вытиральщик протер %s за %d секунды\n", wiper[i].utensil, wiper[i].time);
					}
		}
	}
	return 0;
}
