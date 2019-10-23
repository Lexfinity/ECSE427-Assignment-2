#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>

//declaring/initializing shared data
int read_count = 0;
static sem_t mutex;
static sem_t rw_mutex;
// int inputData = 0;

//declaring/initializing time variables
double min_waitW = 1000000000000000;
double max_waitW = -1;
double min_waitR = 1000000000000000;
double max_waitR = -1;
double avg_waitW, avg_waitR;

int SleepTime(){
    return rand() % 100000;
}

struct timespec before, after;

static void *reader(void *arg)
{

  int total = *((int *)arg);
  int i = 0;
  while (i < total)
  {
    float random = SleepTime(); 
    clock_t start_time = clock();
    // clock_gettime(CLOCK_MONOTONIC, &before);
    sem_wait(&mutex);

    read_count++;
    if (read_count == 1)
      sem_wait(&rw_mutex);

    sem_post(&mutex);
    clock_t wait = clock() -start_time;
    // clock_gettime(CLOCK_MONOTONIC, &after);
    double time = ((double)wait) / CLOCKS_PER_SEC;

  // double seconds = (double) (after.tv_sec - before.tv_sec) * 1000000;
  // double nanosec = (after.tv_nsec - before.tv_nsec) / 1000;
  // double time = seconds + nanosec;

    if (time < min_waitR)
    {
      min_waitR = time;
    }

    if (time > max_waitR)
    {
      max_waitR = time;
    }

    avg_waitR += time;

    printf("reader ");
    sem_wait(&mutex);
    read_count--;
    if (read_count == 0)
      sem_post(&rw_mutex);
    sem_post(&mutex);
    usleep(random);
    i++;
  }
}

static void *writer(void *arg)
{
  int total = *((int *)arg);
  int i = 0;
  while (i < total)
  {
    float random = SleepTime(); 
    clock_t start_time = clock();
    // clock_gettime(CLOCK_MONOTONIC, &before);
    sem_wait(&rw_mutex);
    // clock_gettime(CLOCK_MONOTONIC, &after);
    clock_t wait = clock() - start_time;
    double time = ((double)wait) / CLOCKS_PER_SEC;
    
    // double seconds = (double) (after.tv_sec - before.tv_sec) * 1000000;
    // double nanosec = (after.tv_nsec - before.tv_nsec) / 1000;
    // double time = seconds + nanosec;



    if (time < min_waitW)
    {
      min_waitW = time;
    }

    if (time > max_waitW)
    {
      max_waitW = time;
    }

    avg_waitW += time;

    printf("writer ");
    sem_post(&rw_mutex);
    usleep(random);
    i++;
  }
}

int main(int argc, char *argv[])
{
  int total_readers = 500;
  int total_writers = 10;
  int reader_count = atoi(argv[1]);
  int writer_count = atoi(argv[2]);

  sem_init(&rw_mutex, 0, 1);
  sem_init(&mutex, 0, 1);

  pthread_t *writer_threads = malloc(sizeof(pthread_t) * total_writers);
  pthread_t *reader_threads = malloc(sizeof(pthread_t) * total_readers);

  // printf("Start creation of Writers\n");

  for (int i = 0; i < total_writers; i++)
  {
    pthread_create(&writer_threads[i], NULL, writer, &writer_count);
  }

  // printf("Start creation of Readers\n");

  for (int i = 0; i < total_readers; i++)
  {
    pthread_create(&reader_threads[i], NULL, reader, &reader_count);
  }

  printf("Wait for Readers\n");

  for (int i = 0; i < total_readers; i++)
  {
    printf("Waiting for : %d\n", i);
    pthread_join(reader_threads[i], NULL);
  }

  printf("Wait for Writers\n");

  for (int i = 0; i < total_writers; i++)
  {
    printf("Waiting for : %d\n", i);
    pthread_join(writer_threads[i], NULL);
  }

  avg_waitR = avg_waitR / (total_readers * reader_count);
  printf("Min read time: %lf\n", min_waitR);
  printf("Max read time: %lf\n", max_waitR);
  printf("The average wait time for reader is: %lf\n", avg_waitR);
  printf("Writer:\n");
  avg_waitW = avg_waitW / (total_writers * writer_count);
  printf("Min read time: %lf\n", min_waitW);
  printf("Max read time: %lf\n", max_waitW);
  printf("The average wait time for writer is: %lf\n", avg_waitW);
}
