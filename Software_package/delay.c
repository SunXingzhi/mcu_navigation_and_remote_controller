#include "delay.h"
#include "stm32f10x.h"

volatile uint32_t systick_counter = 0;

void delay_init(void)
{
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1);
	}
}

void delay_ms(uint32_t ms)
{
	if (ms == 0) return;
	
	uint32_t start = systick_counter;
	while ((systick_counter - start) < ms);
}

void delay_us(uint32_t us)
{
	if (us == 0) return;

	uint32_t count = us * 8;
	
	for (uint32_t i = 0; i < count; i++) {
		__NOP();
	}
}

void delay_s(uint32_t s)
{
	if (s == 0) return;
	delay_ms(s * 1000);
}

uint32_t get_systick_counter(void)
{
	return systick_counter;
}

void reset_systick_counter(void)
{
	__disable_irq();
	systick_counter = 0;
	__enable_irq();
}

bool delay_check_timeout(uint32_t start_time, uint32_t timeout_ms)
{
	return ((systick_counter - start_time) >= timeout_ms);
}

void SysTick_Handler(void)
{
	systick_counter++;
}
