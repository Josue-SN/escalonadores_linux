#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <linux/sched.h>

pthread_barrier_t barrier;
sem_t semaforo_binario;
int counter = 0;
char *buffer;
int threadCount = 0;
int indiceBuffer = 0;
int tamBuffer = 0;
char identificadores[7] = {'1','2','3','4','5','6','7'};

int setpriority(pthread_t *thr, int newpolicy, int newpriority)
{
	int policy, ret;
	struct sched_param param;

	if (newpriority > sched_get_priority_max(newpolicy) || newpriority < sched_get_priority_min(newpolicy)){
		//printf("Invalid priority: MIN: %d, MAX: %d", sched_get_priority_min(newpolicy), sched_get_priority_max(newpolicy));
		return -1;
	}

	pthread_getschedparam(*thr, &policy, &param);

	param.sched_priority = newpriority;
	ret = pthread_setschedparam(*thr, newpolicy, &param);
	if (ret != 0)
		perror("perror(): ");

	pthread_getschedparam(*thr, &policy, &param);

	return 0;
}

void *run(void *data)
{

	char * letra = (char *)data;
	
	pthread_barrier_wait(&barrier);
	printf("Ordem de chegada: %c \n", *letra);
	while(indiceBuffer < tamBuffer) {
		sem_wait(&semaforo_binario);
		buffer[indiceBuffer] = *letra;
		indiceBuffer += 1;
		sem_post(&semaforo_binario);
	}

	return 0;
}

int main(int argc, char **argv)
{
	int quantidadeThreads;
	int politica = SCHED_OTHER;	

	threadCount = atoi(argv[1]);
	pthread_barrier_init(&barrier, NULL,threadCount);
	sem_init(&semaforo_binario, 1, 1);
	tamBuffer = atoi(argv[2]) * 1000;
	buffer = malloc(sizeof(char) * atoi(argv[2]));	

	quantidadeThreads = atoi(argv[1]);
	pthread_t threads[quantidadeThreads];

	for (int i = 0; i < quantidadeThreads; i++)
	{
		pthread_create(&threads[i], NULL, run, &identificadores[6-i]);
		setpriority(&threads[i], politica, atoi(argv[4]));
	}

	for (int i = 0; i < quantidadeThreads; i++)
	{
		pthread_join(threads[i], NULL);
	}
		
	printf("%s", buffer);	

	sem_destroy(&semaforo_binario);
	return 0;
}
