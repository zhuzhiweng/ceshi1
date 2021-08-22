/*
 * queue.h
 *
 *  Created on: May 30, 2018
 *      Author: houxd
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "rtos_wrapper.h"

struct Queue_t {
	uint32_t item_size;
	uint8_t *buffer_beg;
	uint8_t *buffer_end;
	uint8_t *head;
	uint8_t *tail;
	int empty;
};

/**
 * @param buffer		queue data buffer, the size must be equals param buffersize
 * @param buffersize	equals with param buffer size
 * @param itemsize		ensure buffersize%itemsize=0. the item count calc from
 * 							buffersize/itemsize.
 */
extern void queue_create(struct Queue_t* q, void *buffer, uint32_t buffer_size,
		uint32_t item_size);
/**
 * @param pdat			the item structure instance pointer that want push to it.
 * @return				0 is success, -1 is full queue.
 */
extern int queue_push(struct Queue_t* q, const void *pdat);
extern int queue_pop(struct Queue_t* q, void *pdat);
extern void queue_reset(struct Queue_t* q);
extern uint32_t queue_size(struct Queue_t* q);

#endif /* QUEUE_H_ */
