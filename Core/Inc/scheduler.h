/*
 * scheduler.h
 *
 *  Created on: Nov 11, 2026
 *      Author: ASUS
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_
#include <stdint.h>
#include "sch_port.h"

#define SCH_MAX_TASKS 40
#define NO_TASK_ID 0u
#define TICKS 10

void SCH_Init(void);
void SCH_Update(void);
void SCH_Dispatch_Tasks(void);
uint32_t SCH_Add_Task(void(*pFunction)(void), uint32_t DELAY, uint32_t PERIOD);\
uint8_t SCH_Delete_Task(uint32_t taskID);

#endif /* INC_SCHEDULER_H_ */
