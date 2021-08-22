/*
 * rtos_wripper.h
 *
 *  Created on: May 28, 2018
 *      Author: houxd
 */

#ifndef RTOS_WRAPPER_H_
#define RTOS_WRAPPER_H_

#include <stdint.h>
#include "cmsis_os.h"

extern void rtos_init(void);
extern void rtos_schedule_start(void);
extern void rtos_schedule_suspend(void);
extern void rtos_schedule_resume(void);

extern void *rtos_malloc(uint32_t sz);
extern void rtos_free(void *p);

struct RtosTask_t;
extern struct RtosTask_t *rtos_task_create(void(*func)(void*), uint32_t stack_size, const char *name, uint32_t prio, void *param);
extern void rtos_task_destroy(struct RtosTask_t *task);

#define RTOS_TIME_MAX	(0xFFFFFFFFU)
extern void rtos_sleep_ms(uint32_t ms);
extern uint32_t rtos_get_tick_count(void);

struct RtosMutex_t;
extern struct RtosMutex_t *rtos_mutex_create(void);
extern void rtos_mutex_destroy(struct RtosMutex_t *mtx);
extern int rtos_mutex_lock(struct RtosMutex_t *mtx);
extern int rtos_mutex_unlock(struct RtosMutex_t *mtx);

struct RtosSem_t;
extern struct RtosSem_t *rtos_sem_create(uint32_t count);
extern void rtos_sem_destroy(struct RtosSem_t *sem);
extern int rtos_sem_down(struct RtosSem_t *sem, uint32_t timeout);
extern int rtos_sem_up(struct RtosSem_t *sem);
extern int rtos_isr_sem_up(struct RtosSem_t *sem);

struct RtosQueue_t;
extern struct RtosQueue_t *rtos_queue_create(uint32_t queue_size, uint32_t item_size);
extern void rtos_queue_destroy(struct RtosQueue_t *queue);
extern int rtos_queue_push(struct RtosQueue_t *queue, const void *p_data);
extern void rtos_isr_queue_push(struct RtosQueue_t *queue, const void *data);
extern int rtos_queue_pop(struct RtosQueue_t *queue, void *p_data, uint32_t timeout);
extern void rtos_queue_clear(struct RtosQueue_t *queue);

#define RTOS_TRACE(...)		
#define RTOS_ASSERT(s)											\
	if(!(s)){													\
		RTOS_TRACE("assert in %s call: %s\n",__FUNCTION__,#s);	\
		for(;;);												\
	}

#endif /* OS_WRAPPER_H_ */
