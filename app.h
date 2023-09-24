#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#include "target_test.h"


#define MAIN_PRIORITY	5		/* HIGH_PRIORITY */

#define HIGH_PRIORITY	9	
#define MID_PRIORITY	10
#define LOW_PRIORITY	11

#ifndef STACK_SIZE
#define	STACK_SIZE		4096
#endif /* STACK_SIZE */

#ifndef LOOP_REF
#define LOOP_REF		ULONG_C(1000000)
#endif /* LOOP_REF */

#ifndef TOPPERS_MACRO_ONLY

//extern void	task(intptr_t exinf);
extern void	main_task(intptr_t exinf);
extern void update_counter(intptr_t exinf);
//extern void balance_task(intptr_t exinf);
//extern void idle_task(intptr_t exinf);
//extern void	tex_routine(TEXPTN texptn, intptr_t exinf);
//#ifdef CPUEXC1
//extern void	cpuexc_handler(void *p_excinf);
//#endif /* CPUEXC1 */
//extern void	cyclic_handler(intptr_t exinf);
//extern void	alarm_handler(intptr_t exinf);
//
//extern void	gpio_handler_initialize(intptr_t exinf);
//extern void	gpio_handler(void);
extern void	gpio_irq_dispatcher(intptr_t exinf);
//
//extern void	uart_sensor_monitor(intptr_t exinf);
//
//extern void	ev3_uart_cyclic_handler(intptr_t exinf);
//extern void	ev3_uart_daemon(intptr_t exinf);
//extern void	ev3_uart_port2_irq(void);
//
//extern void initialize_ev3(intptr_t exinf);
#endif /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif
