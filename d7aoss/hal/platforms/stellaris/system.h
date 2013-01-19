/*
 *  Created on: Nov 22, 2012
 *  Authors:
 * 		maarten.weyn@artesis.be
 *  	glenn.ergeerts@artesis.be
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "../../../types.h"

#define ENTER_CRITICAL_SECTION(x)  	__disable_interrupt(); //st( x = __read_status_register(); __disable_interrupt(); )
#define EXIT_CRITICAL_SECTION(x)    __enable_interrupt(); //__write_status_register(x)

extern u8 tag_id[8]; // TODO: keep this as global?

void system_init();

void system_watchdog_timer_init();
void system_watchdog_timer_stop();
void system_watchdog_timer_start();
void WatchdogIntHandler(void);

void system_lowpower_mode(unsigned char mode, unsigned char enableInterrupts);

void system_get_unique_id(unsigned char *tagId);

#endif // __SYSTEM_H__
