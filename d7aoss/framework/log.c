/*
 *  Created on: Nov 22, 2012
 *  Authors:
 * 		maarten.weyn@artesis.be
 *  	glenn.ergeerts@artesis.be
 */

#include "log.h"
#include "string.h"

#include "../hal/uart.h"

// TODO only in debug mode?

void log_print_string(char* message)
{
	uint8_t len = strlen(message);
	uart_transmit_data(0xDD);
	uart_transmit_data(LOG_TYPE_STRING);
	uart_transmit_data(len);
	uart_transmit_message((unsigned char*) message, len);
}

void log_print_data(uint8_t* message, uint8_t length)
{
	uart_transmit_data(0xDD);
	uart_transmit_data(LOG_TYPE_DATA);
	uart_transmit_data(length);
	uart_transmit_message((unsigned char*) message, length);
}

void log_phy_rx_res(phy_rx_data_t* res)
{
	// transmit the log header
	uart_transmit_data(0xDD);
	uart_transmit_data(LOG_TYPE_PHY_RX_RES);
	uart_transmit_data(LOG_TYPE_PHY_RX_RES_SIZE + res->length);

	// transmit struct member per member, so we are not dependent on packing
	uart_transmit_data(res->rssi);
	uart_transmit_data(res->lqi);
	uart_transmit_data(res->length);

	// transmit the packet
	uart_transmit_message(res->data, res->length);
}

void log_dll_rx_res(dll_rx_res_t* res)
{
	uart_transmit_data(0xDD);
	uart_transmit_data(LOG_TYPE_DLL_RX_RES);
	uart_transmit_data(LOG_TYPE_DLL_RX_RES_SIZE);
	uart_transmit_data(res->frame_type);
	uart_transmit_data(res->spectrum_id);
}
