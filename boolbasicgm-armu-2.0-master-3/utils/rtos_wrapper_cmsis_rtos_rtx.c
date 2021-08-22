/*
 * rtos_wrapper_freertos.c
 *
 *  Created on: May 29, 2018
 *      Author: houxd
 */
#include "rtos_wrapper.h"
#include "stdarg.h"
#include "stdlib.h"


#define _PORT_RTOS_TIME(n)		((n!=RTOS_TIME_MAX)?:osWaitForever)

void rtos_init(void) {
	osKernelInitialize();
}
void rtos_schedule_start(void) {
	osKernelStart();
}
static uint32_t _g_s;
void rtos_schedule_suspend(){
	_g_s = os_suspend();
}
void rtos_schedule_resume(){
	os_resume(_g_s);
}
_Bool rtos_isrunning() {
	return osKernelRunning() ? 1 : 0;
}

void *rtos_malloc(uint32_t sz) {
	uint32_t s;
	if (rtos_isrunning())
		s = os_suspend();
	void*p = malloc(sz);
	if (rtos_isrunning())
		os_resume(s);
	return p;
}
void rtos_free(void *p) {
	uint32_t s;
	if (rtos_isrunning())
		s = os_suspend();
	free(p);
	if (rtos_isrunning())
		os_resume(s);
}

struct RtosTask_t {
	osThreadDef_t def;
	osThreadId id;
};
struct RtosTask_t * rtos_task_create(void (*func)(void*), uint32_t stack_size,
		const char *name, uint32_t prio, void *param) {
	struct RtosTask_t* task = (struct RtosTask_t*) rtos_malloc(
			sizeof(struct RtosTask_t));
	RTOS_ASSERT(task);
	task->def.pthread = (os_pthread) func;
	task->def.tpriority = osPriorityNormal + prio;
	task->def.instances = 1;
	task->def.stacksize = stack_size/4*4;
	RTOS_ASSERT(task->id = osThreadCreate(&task->def, param));
	return task;
}
void rtos_task_destroy(struct RtosTask_t *task) {
	RTOS_ASSERT(task);
	RTOS_ASSERT(osOK == osThreadTerminate(task->id));
}

void rtos_sleep_ms(uint32_t ms) {
	osDelay(ms);
}
uint32_t rtos_get_tick_count(void) {
	return osKernelSysTick()/(osKernelSysTickFrequency/1000*2);
}

struct RtosMutex_t {
	uint32_t name[4];
	osMutexDef_t def;
	osMutexId (id);
};
struct RtosMutex_t *rtos_mutex_create(void) {
	struct RtosMutex_t* mtx = (struct RtosMutex_t*) rtos_malloc(
			sizeof(struct RtosMutex_t));
	RTOS_ASSERT(mtx);
	memset(mtx->name, 0, sizeof(mtx->name));
	mtx->def.mutex = mtx->name;
	mtx->id = osMutexCreate(&mtx->def);
	RTOS_ASSERT(mtx->id);
	return mtx;
}
void rtos_mutex_destroy(struct RtosMutex_t *mtx) {
	if(!mtx)return;
	osMutexDelete(mtx->id);
	rtos_free(mtx);
}
int rtos_mutex_lock(struct RtosMutex_t *mtx) {
	if(!mtx)return -2;
	return (osOK==osMutexWait(mtx->id, osWaitForever))?0:-1;
}
int rtos_mutex_unlock(struct RtosMutex_t *mtx) {
	if(!mtx)return -2;
	return (osOK==osMutexRelease(mtx->id))?0:-1;
}

struct RtosSem_t {
	uint32_t name[4];
	osSemaphoreDef_t def;
	osSemaphoreId id;
};
struct RtosSem_t *rtos_sem_create(uint32_t count) {
	struct RtosSem_t* sem = (struct RtosSem_t*) rtos_malloc(
			sizeof(struct RtosSem_t));
	RTOS_ASSERT(sem);
	memset(sem->name, 0, sizeof(sem->name));
	sem->def.semaphore = sem->name;
	sem->id = osSemaphoreCreate(&sem->def, count);
	RTOS_ASSERT(sem->id);
	return sem;
}
void rtos_sem_destroy(struct RtosSem_t *sem) {
	RTOS_ASSERT(sem);
	osSemaphoreDelete(sem->id);
	rtos_free(sem);
}
int rtos_sem_down(struct RtosSem_t *sem, uint32_t timeout) {
	RTOS_ASSERT(sem);
	return (osOK==osSemaphoreWait(sem->id, timeout))?0:-1;
}
int rtos_sem_up(struct RtosSem_t *sem) {
	RTOS_ASSERT(sem);
	return (osOK==osSemaphoreRelease(sem->id))?0:-1;
}
int rtos_isr_sem_up(struct RtosSem_t *sem)
{
	RTOS_ASSERT(sem);
	return (osOK==osSemaphoreRelease(sem->id))?0:-1;
}

struct RtosQueue_t {
	uint32_t name[4];
	osMessageQDef_t def;
	osMessageQId id;
};
struct RtosQueue_t *rtos_queue_create(uint32_t queue_size, uint32_t item_size) {
	struct RtosQueue_t* queue = (struct RtosQueue_t*) rtos_malloc(
			sizeof(struct RtosQueue_t));
	RTOS_ASSERT(queue);
	memset(queue->name, 0, sizeof(queue->name));
	queue->def.pool = queue->name;
	queue->def.queue_sz = queue_size;
	queue->id = osMessageCreate(&queue->def,NULL);
	RTOS_ASSERT(queue->id);
	return queue;
}
void rtos_queue_destroy(struct RtosQueue_t *queue) {
	if(!queue)return;
	//osQueueDelete(queue->id);
	rtos_free(queue);
}
int rtos_queue_push(struct RtosQueue_t *queue, const void *data) {
	if(!queue)return -1;
	uint32_t v = (*(uint8_t*)data);
	return (osOK==osMessagePut(queue->id, v, osWaitForever))?0:-1;
}
void rtos_isr_queue_push(struct RtosQueue_t *queue, const void *data) {
	if(!queue)return;
	uint32_t v = (*(uint8_t*)data);
	osMessagePut(queue->id, v, osWaitForever);
}
int rtos_queue_pop(struct RtosQueue_t *queue, void *data, uint32_t timeout) {
	if(!queue) return -1;
	osEvent event = osMessageGet(queue->id, (uint32_t)timeout);
	if(event.status==osEventMessage){
		*((uint8_t*)data) = event.value.v;
		return 0;
	}
	return -1;
}
void rtos_queue_clear(struct RtosQueue_t *queue) {
	/* NOT Safe */
	if(!queue) return;
	osMessageGet(queue->id, 0);
}

void os_error_(uint32_t error_code) {

	/* HERE: include optional code to be executed on runtime error. */
	switch (error_code) {
	case 1://OS_ERROR_STACK_OVF:
		RTOS_TRACE("Stack overflow detected for the currently running task. \n");
		/* Thread can be identified by calling svcThreadGetId().   */
		break;
	case 2://OS_ERROR_FIFO_OVF:
		RTOS_TRACE("ISR FIFO Queue buffer overflow detected. \n");
		break;
	case 3://OS_ERROR_MBX_OVF:
		RTOS_TRACE("Mailbox overflow detected. \n");
		break;
	case 4://OS_ERROR_TIMER_OVF:
		RTOS_TRACE("User Timer Callback Queue overflow detected. \n");
		break;
	default:
		break;
	}
	for (;;)
		;
}
