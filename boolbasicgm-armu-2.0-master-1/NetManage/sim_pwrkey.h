#ifndef __SIM_PWRKEY_H
#define __SIM_PWRKEY_H			    
#include "sys.h" 
#include "stdio.h"
#include "n32g45x_gpio.h"


void set_sim_pwrkey(void);
void reset_sim_pwrkey(void);
void turn_on_sim_pwr(void);
void turn_off_sim_pwr(void);
void sim_gpio_init(void);
//void sim_power_on(void);
//void sim_power_off(void);
#endif

