/*!
 *
 * \copyright (C) Copyright 2013 University of Antwerp (http://www.cosys-lab.be) and others.\n
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.\n
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * Contributors:
 * 		maarten.weyn@uantwerpen.be
 *     	glenn.ergeerts@uantwerpen.be
 *
 */

#include "cc430_addresses.h"
#include "../ping.h"
#include "platforms/platform.h"
#include "../../framework/timer.h"

//#include "inc/hw_memmap.h"
#include "driverlib/5xx_6xx/gpio.h"
#include "driverlib/5xx_6xx/wdt.h"


#define PING_BASEADDRESS __MSP430_BASEADDRESS_PORT2_R__
#define PING_PORT GPIO_PORT_P2
#define PING_PIN GPIO_PIN0



void ping_init() {
	ping_initWrite();
	ping_off();
}

void ping_initAsOutput() {
	GPIO_setAsOutputPin(PING_BASEADDRESS, PING_PORT, PING_PIN);
}
void ping_initWrite() {
	ping_initAsOutput();
}
void ping_initAsInput() {
	GPIO_setAsInputPin(PING_BASEADDRESS, PING_PORT, PING_PIN);
}
void ping_initRead() {
	ping_initAsInput();
}

void ping_off() {
	GPIO_setOutputLowOnPin(PING_BASEADDRESS, PING_PORT, PING_PIN);
}
void ping_on() {
	GPIO_setOutputHighOnPin(PING_BASEADDRESS, PING_PORT, PING_PIN);
}
void ping_toggle() {
	GPIO_toggleOutputOnPin(PING_BASEADDRESS, PING_PORT, PING_PIN);
}

unsigned char ping_is_high() {
	return (GPIO_INPUT_PIN_LOW == GPIO_getInputPinValue(PING_BASEADDRESS, PING_PORT, PING_PIN));
}


void ping_enable_interrupt_low_to_high() {
	GPIO_enableInterrupt(PING_BASEADDRESS, PING_PORT, PING_PIN);

	GPIO_interruptEdgeSelect(PING_BASEADDRESS, PING_PORT, PING_PIN,
		GPIO_LOW_TO_HIGH_TRANSITION);

	ping_clear_interrupt_flag();
}
void ping_enable_interrupt_high_to_low() {
	GPIO_enableInterrupt(PING_BASEADDRESS, PING_PORT, PING_PIN);

	GPIO_interruptEdgeSelect(PING_BASEADDRESS, PING_PORT, PING_PIN,
		GPIO_HIGH_TO_LOW_TRANSITION);

	ping_clear_interrupt_flag();
}
void ping_enable_interrupts()
{
	ping_enable_interrupt_low_to_high();
}

void ping_disable_interrupts()
{
    GPIO_disableInterrupt(PING_BASEADDRESS, PING_PORT, PING_PIN);
}

void ping_clear_interrupt_flag()
{
    GPIO_clearInterruptFlag(PING_BASEADDRESS, PING_PORT, PING_PIN);
}

unsigned char ping_is_active()
{
	return (GPIO_INPUT_PIN_HIGH == GPIO_getInputPinValue(PING_BASEADDRESS, PING_PORT, PING_PIN));
}
unsigned char ping_is_inactive()
{
	return (GPIO_INPUT_PIN_LOW == GPIO_getInputPinValue(PING_BASEADDRESS, PING_PORT, PING_PIN));
}

