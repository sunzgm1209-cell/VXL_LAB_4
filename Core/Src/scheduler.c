/*
 * scheduler.c
 *
 *  Created on: Nov 11, 2026
 *      Author: ASUS
 */
#include <string.h>
#include "../Inc/scheduler.h"

typedef struct {
	void (*pTask)(void);
	uint32_t Delay;
	uint32_t Period;
	uint8_t RunMe;
	uint32_t TaskID;
} sTask;

static sTask SCH_tasks_G[SCH_MAX_TASKS];
static uint8_t  SCH_count = 0;
static uint32_t newTaskID = 0;

static uint32_t Get_New_Task_ID(){
	if (++newTaskID == NO_TASK_ID) newTaskID++;
	return newTaskID;
}

void SCH_Init(){
	memset(SCH_tasks_G, 0, sizeof(SCH_tasks_G));
	SCH_count = 0;
	newTaskID = 0;
}

void SCH_Update(){
	if (SCH_count == 0) return;
	if (SCH_tasks_G[0].Delay > 0) SCH_tasks_G[0].Delay--;
	if (SCH_tasks_G[0].Delay <= 0) SCH_tasks_G[0].RunMe = 1;
}

static uint32_t Insert(void (*pF)(void), uint32_t delay, uint32_t period, uint32_t id){
	uint32_t m = SCH_Lock();
	if (SCH_count >= SCH_MAX_TASKS){
		SCH_Unlock(m);
		return NO_TASK_ID;
	}
	uint8_t idx = 0;
	uint32_t sum = 0;
	while (idx < SCH_count && sum + SCH_tasks_G[idx].Delay <= delay){
		sum += SCH_tasks_G[idx].Delay;
		idx++;
	}

	for (uint8_t i = SCH_count; i > idx; --i){
		SCH_tasks_G[i] = SCH_tasks_G[i - 1];
	}

	sTask *t  = &SCH_tasks_G[idx];
	t->pTask  = pF;
	t->Delay  = delay - sum;
	t->Period = period;
	t->TaskID = id;
	t->RunMe  = (idx == 0 && t->Delay == 0);

	if (idx < SCH_count) {
		SCH_tasks_G[idx + 1].Delay -= t->Delay;
	}
	SCH_count++;
	SCH_Unlock(m);
	return id;
}

uint32_t SCH_Add_Task(void(*pFunction)(void), uint32_t DELAY, uint32_t PERIOD) {
	uint32_t DELAY_ticks  = DELAY / TICKS;
	uint32_t PERIOD_ticks = PERIOD / TICKS;
	uint32_t m = SCH_Lock();
	uint32_t id = Get_New_Task_ID();
	SCH_Unlock(m);
	return Insert(pFunction, DELAY_ticks, PERIOD_ticks, id);
}

void SCH_Dispatch_Tasks() {
	while (SCH_count > 0 && SCH_tasks_G[0].RunMe){
		sTask t = SCH_tasks_G[0];
		t.pTask();
		if (SCH_Delete_Task(t.TaskID) && t.Period > 0) {
			Insert(t.pTask, t.Period, t.Period, t.TaskID);
		}
	}
}

uint8_t SCH_Delete_Task(uint32_t taskID){
	if (taskID == NO_TASK_ID) return 0;
	uint8_t found = 0;
	uint32_t m = SCH_Lock();
	for (uint8_t i = 0; i < SCH_count; i++) {
		if (SCH_tasks_G[i].TaskID != taskID) continue;
		if (i + 1 < SCH_count){
			SCH_tasks_G[i + 1].Delay += SCH_tasks_G[i].Delay;
			SCH_tasks_G[i + 1].RunMe = (i == 0 && SCH_tasks_G[i + 1].Delay == 0);
		}

		for (uint8_t j = i; j + 1 < SCH_count; j++) {
			SCH_tasks_G[j] = SCH_tasks_G[j + 1];
		}
		SCH_count--;
		memset(&SCH_tasks_G[SCH_count], 0, sizeof(sTask));
		found = 1;
		break;
	}
	SCH_Unlock(m);
	return found;
}
