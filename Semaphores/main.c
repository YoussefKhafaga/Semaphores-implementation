#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#define N 10
#define SIZE 100
#define True 1

int counter = 0;

int buffer[SIZE];
int Rear = - 1;
int Front = - 1;


void enqueue(int value)
{
    if (Rear == SIZE - 1)
        printf("Buffer is Full \n");
    else
    {
        if (Front == - 1)
            Front = 0;
        Rear = Rear + 1;
        buffer[Rear] = value;
    }
}

int dequeue()
{
    int value;
    if (Front == - 1 || Front > Rear)
    {
        printf("Buffer is empty \n");
    }
    else
    {
        value = buffer[Front];
        Front = Front + 1;
        return value;
    }
}


sem_t counterSemaphore;
sem_t bufferSemaphore;
sem_t emptySemaphore;
sem_t fullSemaphore;


_Noreturn void *mCounterFunc(void *i){
    int iterator = (int )i;
    while (True) {
        printf("Counter thread %d: waiting to write\n", iterator);
        sem_wait(&counterSemaphore);
        printf("Counter thread %d: now adding to counter, counter value = %d\n",iterator, ++counter);
        sem_post(&counterSemaphore);
        sleep(rand() % 4);
        printf("Counter thread %d received a message\n", iterator);
    }
}

_Noreturn void *mMonitorFunc(){
    while (True){
        printf("Monitor thread: waiting to read counter\n");
        sem_wait(&counterSemaphore);
        printf("Monitor thread: reading a count value of %d\n",counter);
        int count = counter;
        counter = 0;
        sem_post(&counterSemaphore);
        sem_wait(&emptySemaphore);
        sem_wait(&bufferSemaphore);
        if(Rear == SIZE-1)
            printf("Monitor thread: Buffer full!!\n");
        else{
            printf("Monitor thread: writing to buffer at position %d\n", Rear + 1);
            enqueue(count);
        }
        sem_post(&bufferSemaphore);
        sem_post(&fullSemaphore);
        sleep(rand() % 7);
    }
}

_Noreturn void *mCollectorFunc(){
    while (True){
        sem_wait(&fullSemaphore);
        sem_wait(&bufferSemaphore);
        if (Front == Rear == -1)
            printf("Collector thread: nothing is in the buffer!\n");
        else {
            printf("Collector thread: reading from the buffer at position %d\n", Front);
            int value = dequeue();
        }
        sem_post(&bufferSemaphore);
        sem_post(&emptySemaphore);
        sleep(rand() % 7);
    }
}

int main()
{
        sem_init(&counterSemaphore, 0, 1);
        sem_init(&bufferSemaphore, 0, 1);
        sem_init(&emptySemaphore, 0, SIZE);
        sem_init(&fullSemaphore, 0, 0);
        pthread_t mCounter[N];
        int arr [N];
        for (int i = 0; i < N; i++) {
            arr[i] = i;
            pthread_create(&mCounter[i], NULL, &mCounterFunc, (void *)i);
        }
        pthread_t mMonitor;
        pthread_create(&mMonitor, NULL, &mMonitorFunc, NULL);
        pthread_t mCollector;
        pthread_create(&mCollector, NULL, &mCollectorFunc, NULL);
        for(int i = 0; i<N; i++)
        {
            pthread_join(mCounter[i], NULL);
        }
    pthread_join(mMonitor, NULL);
    pthread_join(mCollector, NULL);
    return 0;
}
