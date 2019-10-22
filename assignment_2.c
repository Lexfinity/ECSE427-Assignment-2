#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

//declaring/initializing shared data
int read_count = 0;
static sem_t mutex;
static sem_t rw_mutex;
// int inputData = 0;

static void *reader(void *arg)
{
  do
  {
    //clock_t start_wait = clock(); 
    sem_wait(&mutex);
    read_count++;
    if (read_count == 1)
      sem_wait(&rw_mutex);

    sem_post(&mutex);
    printf("reader\n");
    sem_wait(&mutex);
    read_count--;
    if (read_count == 0)
      sem_post(&rw_mutex);
    sem_post(&mutex);
  } while (1);
}

static void *writer(void *arg)
{
  do
  {
    //clock_t start_wait = clock(); 
    sem_wait(&rw_mutex);
    printf("writer\n");
    sem_post(&rw_mutex);
  } while (1);
}

int main(int argc, char *argv[])
{

// printf("hellomain");
  int total_readers = 500;
  int total_writers = 10;
  int reader_count = atoi(argv[1]);
  int writer_count = atoi(argv[2]);

  // Initation of semaphores
  sem_init(&rw_mutex, 0, 1);
  sem_init(&mutex, 0, 1);

  // Instantiating the threads for the writters and readers
  pthread_t *writer_threads = malloc(sizeof(pthread_t) *total_writers);
  pthread_t *reader_threads= malloc(sizeof(pthread_t)* total_readers);

  printf("Start creation of Writers\n");
  // Creating the writers threads
  for (int i = 0; i < total_writers; i++)
  {
    pthread_create(&writer_threads[i], NULL, writer, &writer_count);
  }

  printf("Start creation of Readers\n");
  // Creating the readers threads
  for (int i = 0; i < total_readers; i++)
  {
    pthread_create(&reader_threads[i], NULL, reader, &reader_count);
  }

  // All the actions is hapenning here
  printf("Wait for Readers\n");
  // Collect all the readers
  for (int i = 0; i < total_readers; i++)
  {
    printf("Waiting for : %d\n", i);
    pthread_join(reader_threads[i], NULL);
  }

  printf("Wait for Writers\n");
  // Collect all the writers
  for (int i = 0; i < total_writers; i++)
  {
    printf("Waiting for : %d\n", i);
    pthread_join(writer_threads[i], NULL);
  }
}


// int main()
// {
//   int i,b;
//   pthread_t rtid[5],wtid[5];
//   sem_init(&mutex,0,1);
//   sem_init(&rw_mutex,0,1);
//   for(i=0;i<=2;i++)
//   {
//     pthread_create(&wtid[i],NULL,writer,(void *)i);
//     pthread_create(&rtid[i],NULL,reader,(void *)i);
//   }
//   for(i=0;i<=2;i++)
//   {
//     pthread_join(wtid[i],NULL);
//     pthread_join(rtid[i],NULL);
//   }
//   return 0;
// }

// do {
//     sem_wait(rw_mutex);

// } while (true);

// do {
//     sem_wait(mutex);
//     read_count++;
//     if (read_count == 1) {
//         sem_wait(rw_mutex);
//     }
//     // SIGNAL MUTEX

//     sem_wait(mutex);
//     read_count--;
//     if (read_count == 0) {
//         // SIGNAL RW_MUTEX
//     }
// } while (true);