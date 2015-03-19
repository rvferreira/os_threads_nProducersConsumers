/*
 ============================================================================
 Name        : so_nProdutorConsumidor.c
 Author      : Raphael Ferreira, Jessika Darambaris e Andressa Andriao
 Version     : 1.0
 Copyright   : Código desenvolvido para a disciplina de SO. Use à vontz!
 Description : N Produtores e consumidores distribuidos em threads
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_PRODUCED 100
#define MAX_QUEUE 10
#define N_PROD_CONS 3

sem_t  mutex, empty, full, max_produced_empty, max_produced_full, acochambrada, acochambrada2;

int queue[MAX_QUEUE], item_available=0, produced=0, consumed=0;


int create_item(void) {
	return (rand()%1000);
}

void insert_into_queue(int item, int prod_id) {
	queue[item_available++] = item;
	produced++;
	printf("producing item:%d, value:%d, queued:%d, producer:%d\n", produced, item, item_available, prod_id);
	return;
}

int extract_from_queue(int cons_id) {
	consumed++;
	printf("cosuming item:%d, value:%d, queued:%d, consumer:%d\n", consumed, queue[item_available-1], item_available-1, cons_id);

	return(queue[--item_available]);
}

void process_item(int my_item) {
	static int printed=0;
	//	printf("Printed:%d, value:%d, queued:%d \n", printed++, my_item, item_available);
	return;
}


void *producer(void *prod_id) {
	int item;

	while(1){
		sem_wait(&acochambrada);
		if (produced < MAX_PRODUCED ) {
			item = create_item();
			sem_wait(&empty);
			sem_wait(&mutex);
			insert_into_queue(item, (int) prod_id);
			sem_post(&mutex);
			sem_post(&full);
			sem_post(&acochambrada);
		}
		else {
			sem_post(&acochambrada);
			break;
		}
	}

	printf("\nThread producer %d saindo.\n\n", (int) prod_id);
	fflush(0);

	pthread_exit(0);
}

void *consumer(void *cons_id) {
	int my_item = 0;

	while(1){
		sem_wait(&acochambrada2);
		if (consumed < MAX_PRODUCED) {
			sem_wait(&full);
			sem_wait(&mutex);
			my_item = extract_from_queue((int) cons_id);
			sem_post(&mutex);
			sem_post(&empty);
			process_item(my_item);
			sem_post(&acochambrada2);
		}
		else{
			sem_post(&acochambrada2);
			break;
		}

	}

	printf("\nThread consumer no. %d saindo.\n\n", (int) cons_id);
	fflush(0);

	pthread_exit(0);
}


int main(void) {
	pthread_t prod_handle[N_PROD_CONS], cons_handle[N_PROD_CONS];

	/* declarations and initializations */
	item_available = 0;

	sem_init (&mutex, 0 , 1);
	sem_init(&empty, 0, MAX_QUEUE);
	sem_init(&full, 0, 0);
	sem_init(&acochambrada, 0, N_PROD_CONS);
	sem_init(&acochambrada2, 0, N_PROD_CONS);

	int i;

	/* create and join producer and consumer threads */
	for (i = 0; i < N_PROD_CONS; i++){

		if (pthread_create(&(prod_handle[i]), 0, (void *) producer, (void *) i) != 0) {
			printf("Error creating thread producer no. %d! Exiting! \n", i);
			exit(0);
		}
		if (pthread_create(cons_handle + i, 0, (void *) consumer, (void *) i) != 0) {
			printf("Error creating thread consumer no. %d! Exiting! \n", i);
			exit(0);
		}
	}

	printf("\n Thread pai vai esperar filhas.\n\n");
	fflush(0);

	for (i = 0; i < N_PROD_CONS; i++){
		pthread_join(prod_handle[i], 0);
		pthread_join(cons_handle[i], 0);
	}

	getchar();

	printf("Thread pai saindo.\n");
	fflush(0);

	return EXIT_SUCCESS;
}
