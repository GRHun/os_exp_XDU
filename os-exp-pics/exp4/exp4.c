#include <stdio.h>   
#include <stdlib.h>   
#include <pthread.h>   
#include <semaphore.h>   
#include <unistd.h>   
#include <time.h>  
#define BUFFER_SIZE 5

typedef int buffer_item;
buffer_item buffer[BUFFER_SIZE];   
int front = -1;   
int rear = -1;   
pthread_mutex_t mutex;   
sem_t empty, full;   

int insert_item(buffer_item item); 
int remove_item(buffer_item *item);   
void producer();   
void consumer();  
void init();   

int main(int argc, char* argv[]) {
	int i=0; 
	if (argc != 4) 
	{  
		fprintf(stderr, "请输入参数: %s 次数 producer数量 consumer数量\n", argv[0]);  
		return -1;  
	}   
	init();   
	int times = atoi(argv[1]); 
	int producer_nums = atoi(argv[2]);  
	int consumer_nums = atoi(argv[3]);
	pthread_t producers[producer_nums];  
	pthread_t consumers[consumer_nums];   
	
	for (i = 0; i < producer_nums; ++i) 
	{   
		pthread_create(&producers[i], NULL, (void*)producer, NULL);   
	}   
	for (i = 0; i < consumer_nums; ++i) 
	{  
		pthread_create(&consumers[i], NULL, (void*)consumer, NULL);   
	}   
	sleep(times);  
	return 0;  
}   

int insert_item(buffer_item item) { 
	rear = (rear + 1) % BUFFER_SIZE;   
	buffer[rear] = item;  
	return 0; 
}   

int remove_item(buffer_item *item) {  
	front = (front + 1) % BUFFER_SIZE;   
	*item = buffer[front]; 
	return 0;   
}

void producer() {   
	buffer_item item = rand() % 100;
	sem_wait(&empty);  
	pthread_mutex_lock(&mutex);   
	insert_item(item);  
	printf("[+] Producer: producer put %d to buffer\n", item);  
	pthread_mutex_unlock(&mutex);  
	sem_post(&full);   
}

void consumer() {   
	buffer_item item = 0;   
	sem_wait(&full);   
	pthread_mutex_lock(&mutex);   
	remove_item(&item);  
	printf("[-] Consumer: consumer get %d from buffer\n", item);  
	pthread_mutex_unlock(&mutex);   
	sem_post(&empty);   
}

void init() {   
srand((unsigned)time(NULL));   
pthread_mutex_init(&mutex, NULL);  
sem_init(&empty, 0, BUFFER_SIZE);   
sem_init(&full, 0, 0);  
}
