#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <mqueue.h>

// #define DEBUG true
#define LINUX true

int NUM_INT, BUFFER_SIZE, NUM_PROD, NUM_CON, child_pid, status, i;
int ioCounter = 0, multiProcess = 0;

struct timeval tv;
double t1;
double t2;

#ifdef LINUX
// Initialize the message queue
int spaceQueueMsgSize = 1, itemQueueMsgSize;
mqd_t spaceQueue, itemQueue;
char  spaceQueueName[] = "/254_harry_space", itemQueueName[] = "/254_harry_item";
mode_t mode = S_IRUSR | S_IWUSR;
struct mq_attr attr;
pid_t child_pid;
#endif

int signalSpaceQueue(){
  if (mq_send(spaceQueue, &NUM_INT, spaceQueueMsgSize, 0) != -1) return 1;
  perror("Send to space queue failed");
  return 0;
}

int waitSpaceQueue(){
  if (mq_receive(spaceQueue, &NUM_INT, spaceQueueMsgSize, 0) != -1) return 1;
  perror("Receive from space queue failed");
  return 0;
}

int signalItemQueue(int* ptr){
  if (mq_send(itemQueue, ptr, itemQueueMsgSize, 0) != -1) return 1;
  perror("Send to item queue failed");
  return 0;
}

int waitItemQueue(int* ptr){
  if (mq_receive(itemQueue, ptr, itemQueueMsgSize, 0) != -1) return 1;
  perror("Receive from item queue failed");
  return 0;
}

int produce(int pid, int i) {
#ifdef DEBUG
  printf("Producer %d produced %d.\n", pid, (pid + NUM_PROD * i));
#endif
  // generate a INT i where i%P = pid
  // E.g.: pid = 3 with 7 producers -> 3,10,17,...
  return (pid + NUM_PROD * i);
}

//int consume(int cid, int value, int ctotal) {
int consume(int cid, int value) {
#ifdef DEBUG
  printf("Consumer %d consumed.\n", cid);
#endif
  // find the square root in the buffer and if the number is prefect square, 
  // print: cid value squareRootOfValue
  int sqrt_value = sqrt((double)value);
  if (sqrt_value * sqrt_value == value) {
    printf("%d %d %d\n", cid, value, sqrt_value);
  }
}

void *producer(int pid) {
  int newValue, i = 0;

  do {
    newValue = produce(pid, i);
    waitSpaceQueue();
    signalItemQueue(&newValue);
    ioCounter--;
    i++;
  } while (ioCounter);
}

void *consumer(int cid) {
  int newValue;

  do {
    waitItemQueue(&newValue);
    signalSpaceQueue();
    consume(cid, newValue);
    ioCounter--;
  } while (ioCounter);
}

int main(int argc, char **argv) {
  /*
  #ifdef DEBUG
    NUM_PROD = 5;
    NUM_CON = 6;
    BUFFER_SIZE = 10;
    NUM_INT = 20;
  #endif
  */

  // Check the number of arguments
  if (argc != 5) {
    printf("Error! Wrong number of arguments.\n");
    return -1;
  }

  // Assign arguments to global variables
  NUM_INT = atoi(argv[1]);
  BUFFER_SIZE = atoi(argv[2]);
  NUM_PROD = atoi(argv[3]);
  NUM_CON = atoi(argv[4]);

  // Check if arguments are valid
  if (NUM_INT <= 0 || BUFFER_SIZE <= 0 || NUM_PROD <= 0 || NUM_CON <= 0) {
    printf("Error! Invalid arguments.\nN: %d, B: %d, P: %d, C: %d.\n", NUM_INT, BUFFER_SIZE, NUM_PROD, NUM_CON);
    return -1;
  }
  // Check if the buffer size is enough
  if (BUFFER_SIZE > 9) {
    printf("Error! Invalid arguments.\nB: %d. \n",BUFFER_SIZE);
    return -1;
  }

  #ifdef LINUX
  itemQueueMsgSize = sizeof(NUM_INT);
  attr.mq_maxmsg  = BUFFER_SIZE;
  attr.mq_flags   = 0;  // a blocking queue
  attr.mq_msgsize = spaceQueueMsgSize;  // notification queue require no msg size
  spaceQueue  = mq_open(spaceQueueName, O_RDWR | O_CREAT, mode, &attr);

  attr.mq_msgsize = itemQueueMsgSize;
  itemQueue  = mq_open(itemQueueName, O_RDWR | O_CREAT, mode, &attr);


  if (!itemQueue || !spaceQueue) {
    perror("SETUP: Filling space queue failed");
    exit(1);
  }

  for (i = 0; i < BUFFER_SIZE; i++) {
    // filling up the whole space queue
    if (!signalSpaceQueue()) {
      perror("SETUP: Filling space queue failed");
      exit(2);
    }
  }
  #endif

  // Initialize timer
  gettimeofday(&tv, NULL);
  t1 = tv.tv_sec + tv.tv_usec / 1000000.0;

  // Create consumers
  for (i = 0; i < NUM_CON; i++){
    child_pid = fork ();
    if (child_pid == 0) {
      /* This is the consumer process.  */
      #ifdef DEBUG
        printf("%dth Consumer \n", i+1);
      #endif
      ioCounter = NUM_INT / NUM_CON;
      ioCounter = (NUM_INT % NUM_CON) > i ? ++ioCounter : ioCounter;
      consumer(i);
      exit(0);
    }
  }

  // Create producers
  for (i = 0; i < NUM_PROD; i++){
    child_pid = fork ();
    if (child_pid == 0) {
      /* This is the producers process.  */
      #ifdef DEBUG
        printf("%dth Producer \n", i+1);
      #endif
      ioCounter = NUM_INT / NUM_PROD;
      ioCounter = (NUM_INT % NUM_PROD) > i ? ++ioCounter : ioCounter;
      producer(i);
      exit(0);
    }
  }



  // Wait for all the processes to be finished
  multiProcess = NUM_PROD + NUM_CON;
  /* This is the parent process.  */
  while (multiProcess > 0) {
    child_pid = wait(&status);
    #ifdef DEBUG
    printf("Child with PID %ld exited with status 0x%x.\n", (long)child_pid, status);
    #endif
    --multiProcess;
  }

  gettimeofday(&tv, NULL);
  t2 = tv.tv_sec + tv.tv_usec / 1000000.0;
  printf("System execution time: %.6lf seconds\n", t2 - t1);

  #ifdef DEBUG
  printf("all processes done and exited\n");
  #endif

  #ifdef LINUX
  // Deleting the queues
  if (mq_close(spaceQueue) == -1 || mq_close(itemQueue) == -1 ) {
    perror("mq_close() failed");
    exit(3);
  }
  #ifdef DEBUG
  printf("all queues are closed \n");
  #endif
  if (mq_unlink(spaceQueueName) != 0 || mq_unlink(itemQueueName) != 0 ) {
    perror("mq_unlink() failed");
    exit(4);
  }
  #ifdef DEBUG
  printf("all queues are closed \n");
  #endif
  #endif

  return 0;
}
