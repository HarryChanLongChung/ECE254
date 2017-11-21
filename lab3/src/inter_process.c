#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>

#define DEBUG true



int produce(int pid) {
#ifdef DEBUG
  printf("Producer %d produced %d.\n", pid, counter);
#endif
  // generate a INT i where i%P = pid
  // E.g.: pid = 3 with 7 producers -> 3,10,17,...
}

int consume(int cid, int value, int ctotal) {
#ifdef DEBUG
  printf("Consumer %d consumed %d.\n", cid, ctotal);
#endif
  // find the square root in the buffer,
  // if the number is prefect square, 
  // print: cid value squareRootOfValue
}

void *producer(void *arg) {
}

void *consumer(void *arg) {
}

int main(int argc, char **argv) {

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

  // Allocate and assign buffer
  int i;
  buffer = malloc(BUFFER_SIZE * sizeof(int));

  for (int i = 0; i < BUFFER_SIZE; i++) {
    buffer[i] = -1;
  }

  // Create producer threads
  // Create consumer threads

  // Wait for producer threads exit
  // Wait for consumer threads exit

  // Deallocate pointers
}
