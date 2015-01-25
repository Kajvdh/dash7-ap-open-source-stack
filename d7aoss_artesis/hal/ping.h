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

void ping_init();

void ping_on();
void ping_off();
void ping_toggle();
void ping_initWrite();
void ping_initRead();



#endif // __LEDS_H__
