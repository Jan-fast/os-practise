#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>

#define CAPACITY 4
int buffer1[CAPACITY];
int buffer2[CAPACITY];
int in1, in2;
int out1, out2;

int buffer1_is_empty()
{
	return in1 == out1;
}

int buffer2_is_empty()
{
	return in2 == out2;
}

int buffer1_is_full()
{
	return (in1 + 1) % CAPACITY == out1;
}

int buffer2_is_full()
{
	return (in2 + 1) % CAPACITY == out2;
}

int get2_item()
{
	int item;
	
	item = buffer2[out2];
	out2 = (out2 + 1) % CAPACITY;
	return item;
}

int get1_item()
{
	int item;
	
	item = buffer1[out1];
	out1 = (out1 + 1) % CAPACITY;
	return item;
}

void put1_item(int item)
{
	buffer1[in1] = item;
	in1 = (in1 + 1) % CAPACITY;
}

void put2_item(int item)
{
	buffer2[in2] = item;
	in2 = (in2 + 1) % CAPACITY;
}

pthread_mutex_t mutex1, mutex2;
pthread_cond_t wait_empty_buffer1, wait_empty_buffer2;
pthread_cond_t wait_full_buffer1, wait_full_buffer2;

#define ITEM_COUNT (CAPACITY * 2)

void* consume (void *arg)
{
	int i, item;
	
	for (i = 0; i < ITEM_COUNT; i++)
	{
		pthread_mutex_lock(&mutex2);
		
		while (buffer2_is_empty())
		{
			pthread_cond_wait(&wait_full_buffer2, &mutex2);
		}
		
		item = get2_item();
		printf("consume item: %c\n", item);
		pthread_cond_signal(&wait_empty_buffer2);
		pthread_mutex_unlock(&mutex2);
	}
	
	return NULL;
}

void *calculate (void *arg)
{
	int i, item;
	
	for (i = 0; i < ITEM_COUNT; i++)
	{
		pthread_mutex_lock(&mutex1);
		
		while (buffer1_is_empty())
		{
			pthread_cond_wait(&wait_full_buffer1, &mutex1);
		}
		
		item = get1_item();
		item = toupper(item);
		
		pthread_cond_signal(&wait_empty_buffer1);
		pthread_mutex_unlock(&mutex1);
		
		pthread_mutex_lock(&mutex2);
		while (buffer2_is_full())
		{
			pthread_cond_wait(&wait_empty_buffer2, &mutex2);
		}
		
		put2_item(item);
		
		pthread_cond_signal(&wait_full_buffer2);
		pthread_mutex_unlock(&mutex2);
	}
	return NULL;
}

void produce()
{
    int i, item;

    for (i = 0; i < ITEM_COUNT; i++) {
        pthread_mutex_lock(&mutex1);
        while (buffer1_is_full()) 
            pthread_cond_wait(&wait_empty_buffer1, &mutex1);

        item = i + 'a';
        printf("produce item: %c\n", item);
        put1_item(item);

        pthread_cond_signal(&wait_full_buffer1);
        pthread_mutex_unlock(&mutex1);
    }
}

int main()
{
    pthread_t consumer_tid, calculater_tid;

    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_cond_init(&wait_empty_buffer1, NULL);
    pthread_cond_init(&wait_empty_buffer2, NULL);
    pthread_cond_init(&wait_full_buffer1, NULL);
    pthread_cond_init(&wait_full_buffer2, NULL);

    pthread_create(&consumer_tid, NULL, consume, NULL);
    pthread_create(&calculater_tid, NULL, calculate, NULL);
    produce();
    return 0;
}

