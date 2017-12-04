/**
 * @file memory.c
 * @brief: ECE254 Lab: memory allocation algorithm comparison template file
 * @author: Youdongchen Zhao & Long Chung Chan
 * @date: 2017-12-03
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "mem.h"

/* defines */
typedef enum Boolean { true,
                       false } boolean;

// Linked list structure for memory node
typedef struct mem_node
{
  size_t node_size;
  boolean isAllocated;
  struct mem_node *prev;
  struct mem_node *next;
} mem_node_t;

/* global variables */
// Best fit
mem_node_t *best_fit_head;

// Worst fit
mem_node_t *worst_fit_head;

/* Functions */

/* memory initializer */
int best_fit_memory_init(size_t size)
{
  if (size < 4 + sizeof(mem_node_t))
  {
    printf("Insufficient memory size: %d", (int)size);
    return -1;
  }

  // Assign linked list variables
  best_fit_head = malloc(size);
  best_fit_head->node_size = size - sizeof(mem_node_t);
  best_fit_head->isAllocated = false;
  best_fit_head->prev = NULL;
  best_fit_head->next = NULL;

  if (best_fit_head == NULL)
    return -1;

  return 0;
}

int worst_fit_memory_init(size_t size)
{
  if (size < 4 + sizeof(mem_node_t))
  {
    printf("Insufficient memory size: %d", (int)size);
    return -1;
  }

  // Assign linked list variables
  worst_fit_head = malloc(size);
  worst_fit_head->node_size = size - sizeof(mem_node_t);
  worst_fit_head->isAllocated = false;
  worst_fit_head->prev = NULL;
  worst_fit_head->next = NULL;

  if (worst_fit_head == NULL)
    return -1;

  return 0;
}

/* memory allocators */
void *best_fit_alloc(size_t size)
{
  if (size <= 0)
  {
    printf("Invalid memory block size: %d", (int)size);
    return NULL;
  }

  size = size % 4 == 0 ? size : size + 4 - size % 4;

  mem_node_t *best_node = NULL;
  mem_node_t *split_node = NULL;
  int best;

  mem_node_t *current_node = best_fit_head;

  // Traverse the linked list
  while (current_node != NULL)
  {
    if ((current_node->isAllocated == false) && (current_node->node_size >= size + 2 * sizeof(mem_node_t)))
    {
      if (best_node == NULL)
      {
        best_node = current_node;
      }

      if (current_node->node_size <= best_node->node_size)
      {
        best = current_node->node_size;
        best_node = current_node;
      }
    }
    current_node = current_node->next;
  }

  if (best_node == NULL || best <= 0)
  {
    return NULL;
  }

  if (best_node->node_size == size)
  {
    best_node->isAllocated = true;
    mem_node_t *ptr = (mem_node_t *)((size_t)best_node + sizeof(mem_node_t));
    return ptr;
  }

  // Split
  split_node = (mem_node_t *)((size_t)best_node + size + sizeof(mem_node_t));

  split_node->node_size = best_node->node_size - size - sizeof(mem_node_t);
  split_node->isAllocated = false;
  split_node->prev = best_node;
  split_node->next = best_node->next;

  // Allocate best_node
  best_node->node_size = size;
  best_node->isAllocated = true;
  best_node->next = split_node;

  if (split_node->next != NULL)
  {
    // If not tail
    split_node->next->prev = split_node;
  }

  mem_node_t *ptr = (mem_node_t *)((size_t)best_node + sizeof(mem_node_t));
  return ptr;
}

void *worst_fit_alloc(size_t size)
{
  if (size <= 0)
  {
    printf("Invalid memory block size: %d", (int)size);
    return NULL;
  }

  size = size % 4 == 0 ? size : size + 4 - size % 4;

  mem_node_t *worst_node = NULL;
  mem_node_t *split_node = NULL;
  int worst;

  mem_node_t *current_node = worst_fit_head;

  // Traverse the linked list
  while (current_node != NULL)
  {
    if ((current_node->isAllocated == false) && (current_node->node_size >= size + 2 * sizeof(mem_node_t)))
    {
      if (worst_node == NULL)
      {
        worst_node = current_node;
      }

      if (current_node->node_size >= worst_node->node_size)
      {
        worst = current_node->node_size;
        worst_node = current_node;
      }
    }
    current_node = current_node->next;
  }

  if (worst_node == NULL || worst <= 0)
  {
    return NULL;
  }

  if (worst_node->node_size == size)
  {
    worst_node->isAllocated = true;
    mem_node_t *ptr = (mem_node_t *)((size_t)worst_node + sizeof(mem_node_t));
    return ptr;
  }

  // Split
  split_node = (mem_node_t *)((size_t)worst_node + size + sizeof(mem_node_t));

  split_node->node_size = worst_node->node_size - size - sizeof(mem_node_t);
  split_node->isAllocated = false;
  split_node->prev = worst_node;
  split_node->next = worst_node->next;

  // Allocate worst_node
  worst_node->node_size = size;
  worst_node->isAllocated = true;
  worst_node->next = split_node;

  if (split_node->next != NULL)
  {
    // If not tail
    split_node->next->prev = split_node;
  }

  mem_node_t *ptr = (mem_node_t *)((size_t)worst_node + sizeof(mem_node_t));
  return ptr;
}

/* memory de-allocator */
void best_fit_dealloc(void *ptr)
{
  if (ptr == NULL)
  {
    return;
  }

  // Get nodes
  mem_node_t *current_node = (mem_node_t *)((size_t)ptr - sizeof(mem_node_t));
  mem_node_t *prev_node = current_node->prev;
  mem_node_t *next_node = current_node->next;

  boolean isPrevFree = false;
  boolean isNextFree = false;

  // Assess if previous/next node is free
  if ((prev_node != NULL) && (prev_node->isAllocated == false))
  {
    isPrevFree = true;
  }

  if ((next_node != NULL) && (next_node->isAllocated == false))
  {
    isNextFree = true;
  }

  // CASE 1: prev and next are both free
  if (isPrevFree && isNextFree)
  {
    prev_node->node_size = prev_node->node_size + current_node->node_size + next_node->node_size + 2 * sizeof(mem_node_t);
    next_node->next->prev = prev_node;
    prev_node->next = next_node->next;
  }
  // CASE 2: prev is free but not next
  else if (isPrevFree && !isNextFree)
  {
    prev_node->node_size = prev_node->node_size + current_node->node_size + sizeof(mem_node_t);
    if (next_node != NULL)
    {
      // If not tail
      next_node->prev = prev_node;
    }
    prev_node->next = next_node;
  }
  // CASE 3: next is free but not prev
  else if (!isPrevFree && isNextFree)
  {
    current_node->node_size = current_node->node_size + next_node->node_size + sizeof(mem_node_t);
    next_node->prev = current_node;
    next_node = next_node->next;
  }

  // CASE 4: neither prev nor next is free
  current_node->isAllocated = false;

  return;
}

void worst_fit_dealloc(void *ptr)
{
  if (ptr == NULL)
  {
    return;
  }

  // Get nodes
  mem_node_t *current_node = (mem_node_t *)((size_t)ptr - sizeof(mem_node_t));
  mem_node_t *prev_node = current_node->prev;
  mem_node_t *next_node = current_node->next;

  boolean isPrevFree = false;
  boolean isNextFree = false;

  // Assess if previous/next node is free
  if ((prev_node != NULL) && (prev_node->isAllocated == false))
  {
    isPrevFree = true;
  }

  if ((next_node != NULL) && (next_node->isAllocated == false))
  {
    isNextFree = true;
  }

  // CASE 1: prev and next are both free
  if (isPrevFree && isNextFree)
  {
    prev_node->node_size = prev_node->node_size + current_node->node_size + next_node->node_size + 2 * sizeof(mem_node_t);
    next_node->next->prev = prev_node;
    prev_node->next = next_node->next;
  }
  // CASE 2: prev is free but not next
  else if (isPrevFree && !isNextFree)
  {
    prev_node->node_size = prev_node->node_size + current_node->node_size + sizeof(mem_node_t);
    if (next_node != NULL)
    {
      // If not tail
      next_node->prev = prev_node;
    }
    prev_node->next = next_node;
  }
  // CASE 3: next is free but not prev
  else if (!isPrevFree && isNextFree)
  {
    current_node->node_size = current_node->node_size + next_node->node_size + sizeof(mem_node_t);
    next_node->prev = current_node;
    next_node = next_node->next;
  }

  // CASE 4: neither prev nor next is free
  current_node->isAllocated = false;

  return;
}

/* memory algorithm metric utility function(s) */

/* count how many free blocks are less than the input size */
int best_fit_count_extfrag(size_t size)
{
  int count = 0;
  while (best_fit_head != NULL)
  {
    if (best_fit_head->node_size < size && best_fit_head->node_size > 0 && best_fit_head->isAllocated == false)
    {
      count++;
    }
    best_fit_head = best_fit_head->next;
  }
  return count;
}

int worst_fit_count_extfrag(size_t size)
{
  int count = 0;
  while (worst_fit_head != NULL)
  {
    if (worst_fit_head->node_size < size && worst_fit_head->node_size > 0 && worst_fit_head->isAllocated == false)
    {
      count++;
    }
    worst_fit_head = worst_fit_head->next;
  }
  return count;
}
