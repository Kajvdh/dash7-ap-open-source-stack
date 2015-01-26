/*! \file leds.h
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
 * \author maarten.weyn@uantwerpen.be
 *  \author glenn.ergeerts@uantwerpen.be
 *
 */

#ifndef __PING_H__
#define __PING_H__

#include "../types.h"

void gpio_initAsOutput(unsigned char gpio_nr);
void gpio_initAsinput(unsigned char gpio_nr);
void gpio_on(unsigned char gpio_nr);
void gpio_off(unsigned char gpio_nr);
void gpio_toggle(unsigned char gpio_nr);
unsigned char gpio_enable_interrupt_low_to_high(unsigned char gpio_nr);
unsigned char gpio_enable_interrupt_high_to_low(unsigned char gpio_nr);
void gpio_disable_interrupt(unsigned char gpio_nr);
void gpio_clear_interrupt_flag(unsigned char gpio_nr);
unsigned char gpio_is_active(unsigned char gpio_nr);

#endif // __LEDS_H__
