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
#include "../gpio.h"
#include "platforms/platform.h"
#include "../../framework/timer.h"

//#include "inc/hw_memmap.h"
#include "driverlib/5xx_6xx/gpio.h"
#include "driverlib/5xx_6xx/wdt.h"



#define GPIO0_BASEADDRESS __MSP430_BASEADDRESS_PORT2_R__
#define GPIO0_PORT GPIO_PORT_P2
#define GPIO0_PIN GPIO_PIN0

#define GPIO1_BASEADDRESS __MSP430_BASEADDRESS_PORT2_R__
#define GPIO1_PORT GPIO_PORT_P2
#define GPIO1_PIN GPIO_PIN1


void gpio_initAsOutput(unsigned char gpio_nr) {
	switch (gpio_nr)
	{
		case 0:
			GPIO_setAsOutputPin(GPIO0_BASEADDRESS, GPIO0_PORT, GPIO0_PIN);
		case 1:
			GPIO_setAsOutputPin(GPIO1_BASEADDRESS, GPIO1_PORT, GPIO1_PIN);
		default:
			return false;
	}
}
void gpio_initAsinput(unsigned char gpio_nr) {
	switch (gpio_nr)
	{
		case 0:
			GPIO_setAsInputPin(GPIO0_BASEADDRESS, GPIO0_PORT, GPIO0_PIN);
		case 1:
			GPIO_setAsInputPin(GPIO1_BASEADDRESS, GPIO1_PORT, GPIO1_PIN);
	}
}

void gpio_on(unsigned char gpio_nr) {
	switch (gpio_nr)
	{
		case 0:
			GPIO_setOutputHighOnPin(GPIO0_BASEADDRESS, GPIO0_PORT, GPIO0_PIN);
		case 1:
			GPIO_setOutputHighOnPin(GPIO1_BASEADDRESS, GPIO1_PORT, GPIO1_PIN);
	}
}

void gpio_off(unsigned char gpio_nr) {
	switch (gpio_nr)
	{
		case 0:
			GPIO_setOutputLowOnPin(GPIO0_BASEADDRESS, GPIO0_PORT, GPIO0_PIN);
		case 1:
			GPIO_setOutputLowOnPin(GPIO1_BASEADDRESS, GPIO1_PORT, GPIO1_PIN);
	}
}

void gpio_toggle(unsigned char gpio_nr) {
	switch (gpio_nr)
	{
		case 0:
			GPIO_toggleOutputOnPin(GPIO0_BASEADDRESS, GPIO0_PORT, GPIO0_PIN);
		case 1:
			GPIO_toggleOutputOnPin(GPIO1_BASEADDRESS, GPIO1_PORT, GPIO1_PIN);
	}
}


unsigned char gpio_enable_interrupt_low_to_high(unsigned char gpio_nr) {
	switch (gpio_nr)
	{
		case 0:
			GPIO_enableInterrupt(GPIO0_BASEADDRESS, GPIO0_PORT, GPIO0_PIN);
			GPIO_interruptEdgeSelect(GPIO0_BASEADDRESS, GPIO0_PORT, GPIO0_PIN,
				GPIO_LOW_TO_HIGH_TRANSITION);
			gpio_clear_interrupt_flag(gpio_nr);
		case 1:
			GPIO_enableInterrupt(GPIO1_BASEADDRESS, GPIO1_PORT, GPIO1_PIN);
			GPIO_interruptEdgeSelect(GPIO1_BASEADDRESS, GPIO1_PORT, GPIO1_PIN,
				GPIO_LOW_TO_HIGH_TRANSITION);
			gpio_clear_interrupt_flag(gpio_nr);
	}
}

unsigned char gpio_enable_interrupt_high_to_low(unsigned char gpio_nr) {
	switch (gpio_nr)
	{
		case 0:
			GPIO_enableInterrupt(GPIO0_BASEADDRESS, GPIO0_PORT, GPIO0_PIN);
			GPIO_interruptEdgeSelect(GPIO0_BASEADDRESS, GPIO0_PORT, GPIO0_PIN,
				GPIO_HIGH_TO_LOW_TRANSITION);
			gpio_clear_interrupt_flag(gpio_nr);
		case 1:
			GPIO_enableInterrupt(GPIO1_BASEADDRESS, GPIO1_PORT, GPIO1_PIN);
			GPIO_interruptEdgeSelect(GPIO1_BASEADDRESS, GPIO1_PORT, GPIO1_PIN,
				GPIO_HIGH_TO_LOW_TRANSITION);
			gpio_clear_interrupt_flag(gpio_nr);
	}
}


void gpio_disable_interrupt(unsigned char gpio_nr)
{
	switch (gpio_nr)
	{
		case 0:
			GPIO_disableInterrupt(GPIO0_BASEADDRESS, GPIO0_PORT, GPIO0_PIN);
		case 1:
			GPIO_disableInterrupt(GPIO1_BASEADDRESS, GPIO1_PORT, GPIO1_PIN);
	}
}

void gpio_clear_interrupt_flag(unsigned char gpio_nr)
{
	switch (gpio_nr)
	{
		case 0:
			GPIO_clearInterruptFlag(GPIO0_BASEADDRESS, GPIO0_PORT, GPIO0_PIN);
		case 1:
			GPIO_clearInterruptFlag(GPIO1_BASEADDRESS, GPIO1_PORT, GPIO1_PIN);
	}
}

unsigned char gpio_is_active(unsigned char gpio_nr)
{
	switch (gpio_nr)
	{
		case 0:
			return (GPIO_INPUT_PIN_HIGH == GPIO_getInputPinValue(GPIO0_BASEADDRESS, GPIO0_PORT, GPIO0_PIN));
		case 1:
			return (GPIO_INPUT_PIN_HIGH == GPIO_getInputPinValue(GPIO1_BASEADDRESS, GPIO1_PORT, GPIO1_PIN));
		default:
			return false;
	}
}
