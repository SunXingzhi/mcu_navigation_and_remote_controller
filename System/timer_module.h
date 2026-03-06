#ifndef __TIMER_MODULE_H__
#define __TIMER_MODULE_H__

#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>

#define TIMER_MODULE_TIM		TIM1
#define TIMER_MODULE_IRQ		TIM1_UP_IRQn

#define TIMER_MODULE_PRESCALER		71		// 72MHz/72 = 1MHz (1us per tick)
#define TIMER_MODULE_MAX_PERIOD		0xFFFF	// 65535

typedef enum{
	TIMER_STATUS_IDLE		= 0,
	TIMER_STATUS_RUNNING	= 1,
	TIMER_STATUS_EXPIRED	= 2
}	TIMER_STATUS;

typedef struct{
	volatile uint32_t	remaining_time_ms;
	volatile uint32_t	total_time_ms;
	volatile TIMER_STATUS	status;
	volatile bool		auto_reload;
	volatile bool		enabled;
	void			(*callback)(void);
}	TIMER_MODULE_TypeDef;

void timer_module_init(void);
void timer_module_start(uint32_t time_ms, bool auto_reload);
void timer_module_stop(void);
void timer_module_set_callback(void (*callback)(void));
void timer_module_reset(void);
uint32_t timer_module_get_remaining_time(void);
TIMER_STATUS timer_module_get_status(void);
void timer_module_enable(void);
void timer_module_disable(void);

#endif
