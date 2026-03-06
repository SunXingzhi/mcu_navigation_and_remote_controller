#include	"timer_module.h"
#include	"stm32f10x.h"
#include	<stdio.h>

static TIMER_MODULE_TypeDef timer_module = {0};

// 定时器模块初始化
// 定时器模块使用TIM1, 时钟源为APB2时钟, 预分频系数为71, 自动重装载值为9999
// 定时器模块中断优先级为2, 子优先级为1
void timer_module_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	TIM_TimeBaseStructure.TIM_Period		= TIMER_MODULE_MAX_PERIOD;
	TIM_TimeBaseStructure.TIM_Prescaler		= TIMER_MODULE_PRESCALER;
	TIM_TimeBaseStructure.TIM_ClockDivision	= TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMER_MODULE_TIM, &TIM_TimeBaseStructure);

	NVIC_InitStructure.NVIC_IRQChannel		= TIMER_MODULE_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		= 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd		= ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ITConfig(TIMER_MODULE_TIM, TIM_IT_Update, ENABLE);

	timer_module.status		= TIMER_STATUS_IDLE;
	timer_module.enabled	= false;
	timer_module.auto_reload	= false;
	timer_module.callback	= NULL;
}

// 定时器模块启动
// time_ms: 定时器时间, 单位为毫秒
// auto_reload: 是否自动重装载
void timer_module_start(uint32_t time_ms, bool auto_reload)
{
	if(time_ms == 0){
		return;
	}

	TIM_Cmd(TIMER_MODULE_TIM, DISABLE);
	TIM_SetCounter(TIMER_MODULE_TIM, 0);

	timer_module.total_time_ms		= time_ms;
	timer_module.remaining_time_ms	= time_ms;
	timer_module.auto_reload		= auto_reload;
	timer_module.status			= TIMER_STATUS_RUNNING;

	TIM_Cmd(TIMER_MODULE_TIM, ENABLE);
}

// 定时器模块停止
void timer_module_stop(void)
{
	TIM_Cmd(TIMER_MODULE_TIM, DISABLE);
	timer_module.status	= TIMER_STATUS_IDLE;
}

// 设置定时器模块回调函数
void timer_module_set_callback(void (*callback)(void))
{
	timer_module.callback	= callback;
}

// 定时器模块重置
void timer_module_reset(void)
{
	TIM_Cmd(TIMER_MODULE_TIM, DISABLE);
	TIM_SetCounter(TIMER_MODULE_TIM, 0);
	timer_module.remaining_time_ms	= timer_module.total_time_ms;
	timer_module.status		= TIMER_STATUS_IDLE;
}

// 获取定时器模块剩余时间
// 返回值: 定时器模块剩余时间, 单位为毫秒
uint32_t timer_module_get_remaining_time(void)
{
	return timer_module.remaining_time_ms;
}

// 获取定时器模块状态
// 返回值: 定时器模块状态
TIMER_STATUS timer_module_get_status(void)
{
	return timer_module.status;
}

// 定时器模块使能
void timer_module_enable(void)
{
	timer_module.enabled	= true;
}

// 定时器模块失能
void timer_module_disable(void)
{
	timer_module.enabled	= false;
}

// 计时中断
void TIM1_UP_IRQHandler(void)
{
	if(TIM_GetITStatus(TIMER_MODULE_TIM, TIM_IT_Update)!=RESET){
		TIM_ClearITPendingBit(TIMER_MODULE_TIM, TIM_IT_Update);

		if(timer_module.enabled && timer_module.status == TIMER_STATUS_RUNNING){
			if(timer_module.remaining_time_ms > 0){
				timer_module.remaining_time_ms--;

				if(timer_module.remaining_time_ms == 0){
					timer_module.status = TIMER_STATUS_EXPIRED;

					if(timer_module.callback != NULL){
						timer_module.callback();
					}

					if(timer_module.auto_reload){
						timer_module.remaining_time_ms	= timer_module.total_time_ms;
						timer_module.status		= TIMER_STATUS_RUNNING;
					} else{
						TIM_Cmd(TIMER_MODULE_TIM, DISABLE);
					}
				}
			}
		}
	}
}
