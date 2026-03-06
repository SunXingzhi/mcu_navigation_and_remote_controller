#ifndef __DELAY_H
#define __DELAY_H

#include <stdint.h>
#include <stdbool.h>

extern volatile uint32_t systick_counter;

void delay_init(void);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
void delay_s(uint32_t s);
uint32_t get_systick_counter(void);
void reset_systick_counter(void);
bool delay_check_timeout(uint32_t start_time, uint32_t timeout_ms);

#endif
