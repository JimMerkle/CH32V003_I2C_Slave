/*
 * queue.c
 *
 *  Created on: Sep 27, 2024
 *      Author: Jim Merkle
 */
#include "debug.h"
#include "queue.h"

//*********************************************************************
void queue_init(QUEUE_ITEM * pqueue)
{
    pqueue->head = 0;
    pqueue->tail = 0;
}

// Store a single byte to the end of the queue
// Assumes a single client storing and single service pulling data from the queue
// Returns QUEUE_SUCCESS, QUEUE_FULL
int queue_enqueue(QUEUE_ITEM * pqueue, uint8_t data)
{
    // Is queue full?  (tail + 1 == head)
    uint8_t tailplusone = pqueue->tail +1;
    if(tailplusone >= QUEUE_SIZE) tailplusone = 0; // wrap index
    if(tailplusone == pqueue->head) return QUEUE_FULL;
    // Store byte
    pqueue->data[pqueue->tail] = data;
    pqueue->tail = tailplusone;
    return QUEUE_SUCCESS;
}

// Remove a single byte from the front of the queue
// Assumes a single client storing and single service pulling data from the queue
// Returns QUEUE_SUCCESS, QUEUE_EMPTY
int queue_dequeue(QUEUE_ITEM * pqueue, uint8_t *pdata)
{
    // Is queue empty?
    if(pqueue->head == pqueue->tail) return QUEUE_EMPTY;
    // Get byte
    *pdata = pqueue->data[pqueue->head];
    // Advance head index
    uint8_t nexthead = pqueue->head +1;
    if(nexthead >= QUEUE_SIZE) nexthead = 0; // wrap index
    pqueue->head = nexthead;
    return QUEUE_SUCCESS;
}

// Returns QUEUE_EMPTY, QUEUE_NOT_EMPTY
int queue_empty(QUEUE_ITEM * pqueue)
{
    if(pqueue->head == pqueue->tail)
        return QUEUE_EMPTY; // queue is empty

    return QUEUE_NOT_EMPTY;
}

// Queue test routines
QUEUE_ITEM data_queue;

// Test routine to fill the queue..  Queue should hold QUEUE_SIZE - 1 items
void queue_fill(QUEUE_ITEM * pqueue)
{
    int rc;
    unsigned i;
    for(i=0;i<=QUEUE_SIZE;i++) {
        rc = queue_enqueue(pqueue,(uint8_t)i);
        if(QUEUE_FULL == rc) {
            printf("%s: FULL, Count: %u\r\n",__func__,i);
            return;
        }
        printf("Store %02X\r\n",i);
    }
    printf("%s: ERROR! Count: %u\r\n",__func__,i);
}

// Test routine to empty the queue..  A full queue should hold QUEUE_SIZE - 1 items
void queue_remove_all(QUEUE_ITEM * pqueue)
{
    int rc;
    unsigned i;
    uint8_t data_byte;
    for(i=0;i<=QUEUE_SIZE;i++) {
        rc = queue_dequeue(pqueue,&data_byte);
        if(QUEUE_EMPTY == rc) {
            printf("%s: EMPRY, Count: %u\r\n",__func__,i);
            return;
        }
        printf("Retrieve %02X\r\n",data_byte);
    }
    printf("%s: ERROR! Count: %u\r\n",__func__,i);
}

