/*
 * sch_port.h
 *
 *  Created on: Nov 11, 2026
 *      Author: ASUS
 */

#ifndef INC_SCH_PORT_H_
#define INC_SCH_PORT_H_

#include "main.h"

static inline uint32_t SCH_Lock(void) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

static inline void SCH_Unlock(uint32_t state) {
    __set_PRIMASK(state);
}


#endif /* INC_SCH_PORT_H_ */
