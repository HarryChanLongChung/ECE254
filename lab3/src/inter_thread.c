#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <math.h>

// #define DEBUG true;

// Global variables
int NUM_INT;
int BUFFER_SIZE;
int NUM_PROD;
int NUM_CON;

int counter = 0;
int *buffer;
int *pid;
int *cid;
int buf_index = -1;
int ctotal = 0;
int cnum;

sem_t spaces;
sem_t items;

pthread_mutex_t prod_mutex;
pthread_mutex_t con_mutex;
pthread_mutex_t buffer_mutex;

struct timeval tv;
double t1;
double t2;

int produce(int pid)
{
#ifdef DEBUG
  printf("Producer %d produced %d.\n", pid, counter + 1);
#endif
  return counter++;
}

void consume(int cid, int value, int ctotal)
{
#ifdef DEBUG
  printf("Consumer %d consumed %d.\n", cid, ctotal);
#endif
  int sqrt_value = sqrt((double)value);
  if (sqrt_value * sqrt_value == value)
  {
    printf("%d %d %d\n", cid, value, sqrt_value);
  }
}

void *producer(void *arg)
{
  int *pid = (int *)arg;
  while (1)
  {
    pthread_mutex_lock(&prod_mutex);

    // When all items are produced
    if (counter == NUM_INT)
    {
      pthread_mutex_unlock(&prod_mutex);
      break;
    }
    // Produce item if mod value is pid
    else if (counter % NUM_PROD == *pid)
    {
      int v = produce(*pid);
      pthread_mutex_unlock(&prod_mutex);
      sem_wait(&spaces);
      pthread_mutex_lock(&buffer_mutex);
      buffer[++buf_index] = v;
      pthread_mutex_unlock(&buffer_mutex);
      sem_post(&items);
    }
    else
    {
      pthread_mutex_unlock(&prod_mutex);
    }
  }
  pthread_exit(NULL);
}

void *consumer(void *arg)
{
  int *cid = (int *)arg;
  while (1)
  {
    pthread_mutex_lock(&con_mutex);

    // Reduce number of consumers if number of remaining items is less than number of remaining consumers
    if (NUM_INT - ctotal < cnum)
    {
      cnum--;
      pthread_mutex_unlock(&con_mutex);
      break;
    }

    pthread_mutex_unlock(&con_mutex);
    sem_wait(&items);
    pthread_mutex_lock(&buffer_mutex);
    int temp = buffer[buf_index];
    buffer[buf_index--] = -1;
    ctotal++;
    pthread_mutex_unlock(&buffer_mutex);
    sem_post(&spaces);
    consume(*cid, temp, ctotal);
  }
  pthread_exit(NULL);
}

int main(int argc, char **argv)
{
  // Check the number of arguments
  if (argc != 5)
  {
    printf("Error! Wrong number of arguments.\n");
    return -1;
  }

  // Assign arguments to global variables
  NUM_INT = atoi(argv[1]);
  BUFFER_SIZE = atoi(argv[2]);
  NUM_PROD = atoi(argv[3]);
  NUM_CON = cnum = atoi(argv[4]);

  // Check if arguments are valid
  if (NUM_INT <= 0 || BUFFER_SIZE <= 0 || NUM_PROD <= 0 || NUM_CON <= 0)
  {
    printf("Error! Invalid arguments.\nN: %d, B: %d, P: %d, C: %d.\n", NUM_INT, BUFFER_SIZE, NUM_PROD, NUM_CON);
    return -1;
  }

  // Set producer counter
  counter = 0;

  // Allocate and assign buffer
  buffer = malloc(BUFFER_SIZE * sizeof(int));
  int i, j, k;

  // Initialize buffer
  for (i = 0; i < BUFFER_SIZE; i++)
  {
    buffer[i] = -1;
  }

  // Initialize semaphores and mutexes
  sem_init(&spaces, 0, BUFFER_SIZE);
  sem_init(&items, 0, 0);
  pthread_mutex_init(&prod_mutex, NULL);
  pthread_mutex_init(&con_mutex, NULL);
  pthread_mutex_init(&buffer_mutex, NULL);

  // Initialize thread variables
  pid = malloc(NUM_PROD * sizeof(int));
  cid = malloc(NUM_CON * sizeof(int));
  pthread_t prod[NUM_PROD];
  pthread_t con[NUM_CON];

  // Initialize timer
  gettimeofday(&tv, NULL);
  t1 = tv.tv_sec + tv.tv_usec / 1000000.0;

  // Create producer threads
  for (j = 0; j < NUM_PROD; j++)
  {
    pid[j] = j;
    pthread_create(&prod[j], NULL, producer, &pid[j]);
  }

  // Create consumer threads
  for (k = 0; k < NUM_CON; k++)
  {
    cid[k] = k;
    pthread_create(&con[k], NULL, consumer, &cid[k]);
  }

  // Wait for producer threads exit
  for (j = 0; j < NUM_PROD; j++)
  {
    pthread_join(prod[j], NULL);
  }

  // Wait for consumer threads exit
  for (k = 0; k < NUM_CON; k++)
  {
    pthread_join(con[k], NULL);
  }

  // Calculate time after all consumers exit
  gettimeofday(&tv, NULL);
  t2 = tv.tv_sec + tv.tv_usec / 1000000.0;

  // Print time
  printf("System execution time: %.6lf seconds\n", t2 - t1);

  // Deallocate pointers
  free(buffer);
  free(pid);
  free(cid);

  // Destroy semaphores and mutexes
  sem_destroy(&spaces);
  sem_destroy(&items);
  pthread_mutex_destroy(&prod_mutex);
  pthread_mutex_destroy(&con_mutex);
  pthread_mutex_destroy(&buffer_mutex);

  pthread_exit(0);
}
