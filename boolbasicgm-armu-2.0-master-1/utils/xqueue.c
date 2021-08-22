/*
 * queue.c
 *
 *  Created on: May 30, 2018
 *      Author: houxd
 */

#include <xqueue.h>
#include <stdlib.h>
#include <stdint.h>

void queue_create(struct Queue_t* q, void *buffer, uint32_t buffer_size,
		uint32_t item_size) {
	q->item_size = item_size;
	q->buffer_beg = buffer;
	q->buffer_end = q->buffer_beg + buffer_size / item_size * item_size;
	q->head = buffer;
	q->tail = buffer;
	q->empty = 1;
}
int queue_isempty(struct Queue_t* q) {
	return q->empty;
}
int queue_isfull(struct Queue_t* q) {
	return (q->head == q->tail) && (!q->empty);
}
int queue_push(struct Queue_t* q, const void *pdat) {
	if (q->head == q->tail) {
		if (!q->empty)
			return -1;
		q->empty = 0;
	}
	memcpy(q->head, pdat, q->item_size);
	if (q->head == (q->buffer_end - q->item_size))
		q->head = q->buffer_beg;
	else
		q->head += q->item_size;
	return 0;
}
int queue_pop(struct Queue_t* q, void *pdat) {
	if (q->empty)
		return -1;
	memcpy(pdat, q->tail, q->item_size);
	if (q->tail == (q->buffer_end - q->item_size))
		q->tail = q->buffer_beg;
	else
		q->tail += q->item_size;
	if (q->head == q->tail)
		q->empty = 1;
	return 0;
}
void queue_reset(struct Queue_t* q) {
	q->head = q->buffer_beg;
	q->tail = q->buffer_beg;
	q->empty = 1;
}
uint32_t queue_size(struct Queue_t* q) {
	if (q->head >= q->tail)
		return q->head - q->tail;
	else
		return (q->buffer_beg - q->buffer_end) - (q->tail - q->head);
}
