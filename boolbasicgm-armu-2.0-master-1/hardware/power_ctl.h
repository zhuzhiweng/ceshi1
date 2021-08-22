#ifndef __POWER_CTL_H
#define __POWER_CTL_H			    
#include "sys.h" 
#include "stdio.h"
#include <string.h>
#include "n32g45x_gpio.h"

void turn_off_3v3(void);
void turn_on_3v3(void);
void turn_on_sim_3v8(void);
void turn_off_sim_3v8(void);
void power_ctl_gpio_init(void);
void is_sleep(void);
void MCU_RESET(void);
#endif

