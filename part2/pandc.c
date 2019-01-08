#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#define TRUE 1



/*
 *all variables of the amount of buffers, producer and consumer threads
 *amount of consumers and extra consumers
 ∗ N is the number of buffers to maintain.
 ∗ P is the number of producer threads.
 ∗ C is the number of consumer threads.
 ∗ X is the number of items each producer thread will produce.

 */
int N, X; //Bounded Buffer queue of N elements
int NCons = 0;
int ExtraConsume = 0;
int itemsConsume = 0;
int temp = 0;


//int sharedVal = 0;
int globalCounter = 0;
int bufferIndex = 0;

int *buffer;
int ProducerVal = 0;
int *pro;
int ConsumerVal = 0;
int *con;

int use = 0;
int fill = 0;


//how long each producer thread will sleep after producing an item.
 //how long each consumer thread will sleep after consuming an item.
struct timespec Ptime, time1, Ctime, time2;
pthread_mutex_t mutex;

sem_t full;
sem_t empty;

/*
 * Function to remove item.
 * Item removed is returned
 */
int dequeue(int item) {
    
    sem_wait(&full);
    pthread_mutex_lock( & mutex);
    
    int checkVal;
    if (bufferIndex > 0) {
        int val;
        val = buffer[use];
        nanosleep(&Ctime, &time2);
        use = (use + 1) % N;
        bufferIndex--;
        con[ConsumerVal] = val;
        ConsumerVal++;
        printf("   %-2d was consumed by consumer->     %-2d \n", val, item);
        checkVal = 1;
    } else {
        checkVal = -1;
    }
    
   
    pthread_mutex_unlock( & mutex);
     sem_post(&empty);
    return checkVal;
    
}

/*
 * Function to add item.
 * Item added is returned.
 * It is up to you to determine
 * how to use the ruturn value.
 * If you decide to not use it, then ignore
 * the return value, do not change the
 * return type to void.
 */
int enqueue(int item) {
    
    sem_wait(&empty);
    pthread_mutex_lock(& mutex);
    
    int checkVal;
    if (bufferIndex < N) {
        globalCounter++;
        buffer[fill] = globalCounter;
        
        nanosleep(&Ptime, &time1);
        
        bufferIndex++; fill = (fill +1)%N;
        pro[ProducerVal] = globalCounter;
        ProducerVal++;
        printf("   %-2d was produced by producer->     %-2d \n", globalCounter, item);
        checkVal = 1;
    } else {
        checkVal = -1;
    }
    
    
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    return checkVal;
}



void * producer(void * args) {
    int ProdID = (int)((long) args);
    int ProducerCount = 0;
    
    do {
        int checkVal = enqueue(ProdID);
        if (checkVal == 1) {
            ProducerCount++;
        }
    } while (ProducerCount < X);
    pthread_exit(NULL);
}

/*
  Each consumer thread must sleep for Ctime AFTER consuming each item
 */
void * consumer(void * args) {
    int ConsID = (int)((long) args);
    int c_count = 0;
    
    int check = NCons;
    if (ExtraConsume > 0) {
        check = NCons + 1;
        ExtraConsume--;
    }
    do {
        if (dequeue(ConsID) == 1) {
            c_count++;
        }
    } while (c_count < check);
    pthread_exit(NULL);
}



int main(int argc, char** argv) {
    
    time1.tv_nsec = 0;
    time2.tv_nsec = 0;
    
   //Set up command line args
    time_t start, end;
    time(&start);
    N = atoi(argv[1]);  //The number of buffers to maintain. 
    int P = atoi(argv[2]); //The number of producer threads. 
    int C = atoi(argv[3]); //The number of consumer threads. 
    X = atoi(argv[4]);   //The number of items each producer thread will produce.
    Ptime.tv_sec = atoi(argv[5]); //Waiting time for producer thread
    Ctime.tv_sec = atoi(argv[6]); //Waiting time for consumer thread
    
    NCons= (P*X)/C;
    ExtraConsume = (P*X)%C;
    
    if (ExtraConsume != 0){
        temp = ExtraConsume;
        temp++;
    }
    
    if (ExtraConsume == 0)
        itemsConsume = 0;
    else
        itemsConsume = 1;
    
    int bArray[N];
    int proArray[(P*X)];
    int cArray[(P*X)];
    
    buffer = bArray;
    pro = proArray;
    con = cArray;
    
    pthread_t pThread[P];
    pthread_t cThread[C];
    
   /* Create the full semaphore and initialize to 0 */
  sem_init(&full, 0, 0);
 /* Create the empty semaphore and initialize to BUFFER_SIZE */
  sem_init(&empty, 0, N);
 if(pthread_mutex_init(&mutex, NULL) != 0){
   printf("mutex init failed\n");
  return 1;
  }
    
    time_t clock = time(NULL);  // current time
    printf("Current time: %s \n", ctime(&clock));
    printf("Number of Buffers = %d \n", N);
    printf("Number of Producers = %d \n", P);
    printf("Number of Consumers = %d \n", C);
    printf("Number of items Produced by each producer = %d \n", X);
    printf("Number of items Consumed by each consumer = %d \n", NCons);
    printf("Over Consume on? = %d \n", itemsConsume);
    printf("Over Consume Amount = %d \n", temp);
    printf("Time each Producers Sleeps  = %ld \n", Ptime.tv_sec);
    printf("Time each Consumer Sleeps = %ld \n\n", Ctime.tv_sec);
   
  /* Create the producer threads */   
    for (long i = 0; i < P; i++) {
        pthread_create(&pThread[i], NULL, producer, (void*) i + 1);
    }
    
    for (long i = 0; i < C; i++) {
        pthread_create(&cThread[i], NULL, consumer, (void*) i + 1);
    }
    
  /* Join all the threads and print them*/
    for (int i = 0; i < P; i++) {
        pthread_join(pThread[i], NULL);
        printf("Producer Thread Join:  %-2d \n", i + 1);
    }
    
    for (int i = 0; i < C; i++) {
        pthread_join(cThread[i], NULL);
        printf("Consumer Thread Join:  %-2d \n", i + 1);
    }
    
    clock = time(NULL);
    printf("Current time: %s \n", ctime( & clock));
    
    int check = 0;
    printf("Producer Array  | Consumer Array \n");
    for (int i = 0; i < (P*X); i++) {
        printf("%-2d              | %-2d \n", pro[i], con[i]);
    }
    
    if (ProducerVal == ConsumerVal) {
        printf("Consumer and Producer Arrays Match!\n");
    }
  
   sem_destroy(&full);
   sem_destroy(&empty);
   pthread_mutex_destroy(&mutex);    
    

    time( & end);
    
    int total_time = (end - start);
    printf("Total Time: %d secs \n", total_time);
    
    pthread_exit(NULL);
    return 0;
}



