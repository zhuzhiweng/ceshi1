#include "sim_pwrkey.h"
#include "cmsis_os.h" 

void set_sim_pwrkey(void)
{
	GPIO_ResetBits(GPIOC, GPIO_PIN_1);
}

void reset_sim_pwrkey(void)
{
	GPIO_SetBits(GPIOC, GPIO_PIN_1);
}

void turn_on_sim_pwr(void)
{
	GPIO_SetBits(GPIOB, GPIO_PIN_5);
}

void turn_off_sim_pwr(void)
{
	GPIO_ResetBits(GPIOB, GPIO_PIN_5);
}

void sim_gpio_init(void)
{
	GPIO_InitType	GPIO_InitStructure;
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
	GPIO_InitStructure.Pin        = GPIO_PIN_5;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
	
	turn_off_sim_pwr();
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
	GPIO_InitStructure.Pin        = GPIO_PIN_1;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
	set_sim_pwrkey();
}

