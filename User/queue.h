/*
 * queue.h
 *
 *  Created on: Sep 27, 2024
 *      Author: Jim Merkle
 */

#ifndef __QUEUE_H
#define __QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

#define QUEUE_SIZE 32

// Definitions / rules:
// "head" : index (or pointer) for next item to be removed from queue
// "tail" : index (or pointer) for next item to be stored into queue
// EMPTY  : "head" == "tail"
// FULL   : "tail" + 1 == "head"
// Note: Using the above rules produces a queue that holds QUEUE_SIZE - 1 items
//
// Alternatively, a Full and/or Empty flag can be added to the queue structure allowing
//   both EMPTY and FULL to be "head" == "tail"  (NOT IMPLEMENTED ! )
//
// This queue stores via "copy", allowing a stack based buffer to pass data into the queue

typedef struct {
    uint8_t head; // index into queue
    uint8_t tail; // index into queue
    uint8_t data[QUEUE_SIZE];
} QUEUE_ITEM;

typedef enum {
    QUEUE_SUCCESS = 0,
    QUEUE_FULL,
    QUEUE_EMPTY,
    QUEUE_NOT_EMPTY
} QUEUE_STATUS;

// Function prototypes
void queue_init(QUEUE_ITEM * pqueue);
int queue_enqueue(QUEUE_ITEM * pqueue, uint8_t data);
int queue_dequeue(QUEUE_ITEM * pqueue, uint8_t *pdata);
int queue_empty(QUEUE_ITEM * pqueue);
// Queue test functions
void queue_fill(QUEUE_ITEM * pqueue);
void queue_remove_all(QUEUE_ITEM * pqueue);


#ifdef __cplusplus
}
#endif

#endif // __QUEUE_H
