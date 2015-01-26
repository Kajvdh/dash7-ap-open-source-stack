/*
 * (C) Copyright 2013 University of Antwerp (http://www.cosys-lab.be) and others.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * Contributors:
 *     maarten.weyn@uantwerpen.be
 *
 * 	Example of Request Response Dialog
 * 	This is the responder
 *
 * 	add the link to d7aoss library in de lnk_*.cmd file, e.g. -l "../../../d7aoss/Debug/d7aoss.lib"
 * 	Make sure to select the correct platform in d7aoss/hal/cc430/platforms.platform.h
 * 	If your platform is not present, you can add a header file in platforms and commit it to the repository.
 * 	Exclude the stub directories in d7aoss from the build when building for a device.
 */


#include <string.h>

#include <d7aoss.h>
#include <hal/system.h>
#include <hal/button.h>
#include <hal/leds.h>
#include <hal/ping.h>
#include <hal/rtc.h>
#include <hal/uart.h>
#include <framework/log.h>
#include <framework/timer.h>
#include <msp430.h>

// Conversion from msec to ACLK timer ticks
#define CONV_MS_TO_TICKS(msec)         			(((msec) * 32768) / 1000)
#define CONV_MV_TO_CELCIUS(mv)					((0.185*mv) - 128)


#define TEMPERATURE_INTERVAL_MS 10000
#define TX_EIRP 10
//#define USE_LEDS


static bool start_channel_scan = false;
static volatile uint8_t add_sensor_event = 0;

static uint8_t receive_channel[2] = {0x04,0x04};
uint8_t tx_buffer[128];
uint8_t rx_buffer[128];

static uint8_t counter = 0;
#define CLOCKS_PER_1us	20
static volatile uint16_t adc12_result;
static volatile uint8_t adc12_data_ready;

static volatile uint8_t interrupt_ping = 0;


uint8_t ping_interrupt_type;

#define TRANSIT_LOW_TO_HIGH 0x01
#define TRANSIT_HIGH_TO_LOW 0x02
#define NO_INTERRUPT 0x00
uint32_t time = 0;

#define PING_EVENT_NONE 0x00
#define PING_EVENT_START 0x01
#define PING_EVENT_LOW 0x02
#define PING_EVENT_HIGH 0x03
#define PING_EVENT_INTERRUPT_LOW_TO_HIGH 0x04
#define PING_EVENT_INTERRUPT_HIGH_TO_LOW 0x05

static volatile uint8_t ping_event = 0;

void start_rx()
{
	counter++;

	start_channel_scan = false;
	trans_rx_query_start(0xFF, receive_channel);
}

void get_temperature()
{
	add_sensor_event = true;
}


uint16_t adc12_single_conversion(uint16_t ref, uint16_t sht, uint16_t channel)
{
	// Initialize the shared reference module
	REFCTL0 |= REFMSTR + ref + REFON;    		// Enable internal reference (1.5V or 2.5V)

	// Initialize ADC12_A
	ADC12CTL0 = sht + ADC12ON;					// Set sample time
	ADC12CTL1 = ADC12SHP;                     	// Enable sample timer
	ADC12MCTL0 = ADC12SREF_1 + channel;  		// ADC input channel
	ADC12IE = 0x001;                          	// ADC_IFG upon conv result-ADCMEMO

  	// Wait 2 ticks (66us) to allow internal reference to settle
	__delay_cycles(66*CLOCKS_PER_1us);

	// Start ADC12
	ADC12CTL0 |= ADC12ENC;

	// Clear data ready flag
  	adc12_data_ready = 0;

  	// Sampling and conversion start
    ADC12CTL0 |= ADC12SC;

    // Wait until ADC12 has finished
    __delay_cycles(200*CLOCKS_PER_1us);
	while (!adc12_data_ready);

	// Shut down ADC12
	ADC12CTL0 &= ~(ADC12ENC | ADC12SC | sht);
	ADC12CTL0 &= ~ADC12ON;

	// Shut down reference voltage
	REFCTL0 &= ~(REFMSTR + ref + REFON);

	ADC12IE = 0;

	// Return ADC result
	return (adc12_result);
}

int16_t temperature_measurement()
{
	uint16_t adc_result;
	volatile int16_t temperaturemV;

	// Convert internal temperature diode voltage
	adc_result = adc12_single_conversion(REFVSEL_0, ADC12SHT0_8, ADC12INCH_10);
	temperaturemV = (int16_t) (adc_result * 0.3662109375); //(= /4096*1500)

    return temperaturemV;
}



void tx_callback(Trans_Tx_Result result)
{
	if(result == TransPacketSent)
	{
		#ifdef USE_LEDS
		led_off(3);
		#endif
		log_print_string("RESPONSE SEND");
	}
	else
	{
		#ifdef USE_LEDS
		led_toggle(2);
		#endif
		log_print_string("TX RESPONSE CCA FAIL");
	}

	// Restart channel scanning
	start_channel_scan = true;
}

int16_t convertMvToCelcius(int16_t i) {
	int16_t y;
	//y=0.185*i-128;
	y= (int16_t)((i*(int16_t)(1))-784);

	return (int16_t)(y);
	//y=0.185x-128
}





void startPingEvent() {
	ping_event = PING_EVENT_START;
}
void pingInitLowEvent() {
	ping_event = PING_EVENT_LOW;
}
void pingInitHighEvent() {
	ping_event = PING_EVENT_HIGH;
}
void pingInterruptLowToHigh() {
	ping_event = PING_EVENT_INTERRUPT_LOW_TO_HIGH;
}
void pingInterruptHighToLow() {
	ping_event = PING_EVENT_INTERRUPT_HIGH_TO_LOW;
}





int main(void) {
	//timer_event event = { &getTemperature, TEMPERATURE_INTERVAL_MS} ;
	timer_event event = { &startPingEvent, 5000} ;
	timer_event event_pingLow = { &pingInitLowEvent, 0.002};
	timer_event event_pingHigh= { &pingInitHighEvent, 0.005};

	int16_t temperature_internal;
	int16_t temperature;
	file_handler fh;


	// Initialize the OSS-7 Stack
	d7aoss_init(tx_buffer, 128, rx_buffer, 128);

	trans_set_tx_callback(&tx_callback);
	// The initial Tca for the CSMA-CA in
	dll_set_initial_t_ca(200);

	start_channel_scan = true;



	timer_add_event(&event);


	log_print_string("responder started");

	// Log the device id
	log_print_data(device_id, 8);

	while(1)
	{
		if (start_channel_scan)
		{
			start_rx();
		}

		if (ping_event == PING_EVENT_START) {
			ping_event = PING_EVENT_NONE;
			ping_disable_interrupts();
			ping_interrupt_type = NO_INTERRUPT;
			ping_initAsOutput(); //Initialise PING sensor as Ouput
			ping_off();
			timer_add_event(&event_pingLow);
		}
		if (ping_event == PING_EVENT_LOW) {
			ping_event = PING_EVENT_NONE;
			ping_on();
			timer_add_event(&event_pingHigh);
		}
		if (ping_event == PING_EVENT_HIGH) {
			ping_event = PING_EVENT_NONE;
			ping_off();
			ping_initAsInput();
			ping_enable_interrupt_low_to_high();
			ping_interrupt_type = TRANSIT_LOW_TO_HIGH;
		}
		//if (ping_event == PING_EVENT_INTERRUPT_LOW_TO_HIGH) {
		//	led_off(1);
		//	ping_event = PING_EVENT_NONE;
		//	//Start timer
		//	hal_benchmarking_timer_init();
		//	hal_benchmarking_timer_start();
		//	ping_enable_interrupt_high_to_low();
		//	ping_interrupt_type = TRANSIT_HIGH_TO_LOW;
		//}
		if (ping_event == PING_EVENT_INTERRUPT_HIGH_TO_LOW) {
			ping_event = PING_EVENT_NONE;
			file_handler fh;
			//Stop timer
			//Time in ms -> calculate distance
			//hal_benchmarking_timer_stop();
			//time = hal_benchmarking_timer_getvalue();


			fs_open(&fh, 32, file_system_user_user, file_system_access_type_write);

			uint8_t data[2];
			//data[0] = (uint8_t) (temperature_internal>> 8);
			//data[1] = (uint8_t) (temperature_internal);

			temperature = convertMvToCelcius(temperature_internal);


			data[0] = (uint8_t) (temperature >> 8);
			data[1] = (uint8_t) (temperature);
			//data[0] = counter;
			//data[1] = counter;
			//data[0] = 41; //0x29
			//data[1] = 8; //0x08
			//data[2] = 25; //0x19
			//data[3] = 147; //0x93
			fs_write_data(&fh, 2, data, 2,true);

			fs_close(&fh);

			timer_add_event(&event);
		}


		if (add_sensor_event)
		{
			add_sensor_event = false;
			temperature_internal = temperature_measurement();

			fs_open(&fh, 32, file_system_user_user, file_system_access_type_write);

			uint8_t data[2];
			//data[0] = (uint8_t) (temperature_internal>> 8);
			//data[1] = (uint8_t) (temperature_internal);

			temperature = convertMvToCelcius(temperature_internal);


			data[0] = (uint8_t) (temperature >> 8);
			data[1] = (uint8_t) (temperature);
			//data[0] = counter;
			//data[1] = counter;
			//data[0] = 41; //0x29
			//data[1] = 8; //0x08
			//data[2] = 25; //0x19
			//data[3] = 147; //0x93
			fs_write_data(&fh, 2, data, 2,true);

			fs_close(&fh);

			timer_add_event(&event);
		}

		// Don't know why but system reboots when LPM > 1 since ACLK is uses for UART
		system_lowpower_mode(0,1);
	}
}


#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR (void)
{
	led_off(1);
	if ((ping_interrupt_type == TRANSIT_LOW_TO_HIGH) && (ping_is_active())) {
		led_on(1);
		ping_disable_interrupts();
		ping_interrupt_type = NO_INTERRUPT;
		//pingInterruptLowToHigh();

		//Start timer
		hal_benchmarking_timer_init();
		hal_benchmarking_timer_start();

		while (ping_is_active()) {
			//Sleep untill GPIO LOW
		}
		hal_benchmarking_timer_stop();
		led_off(1);
		pingInterruptHighToLow();
		//ping_enable_interrupt_high_to_low();
		//ping_interrupt_type = TRANSIT_HIGH_TO_LOW;
	}
	if ((ping_interrupt_type == TRANSIT_HIGH_TO_LOW) && (ping_is_inactive())) {
		led_off(1);
		ping_disable_interrupts();
		ping_interrupt_type = NO_INTERRUPT;
		//hal_benchmarking_timer_stop();
		pingInterruptHighToLow();
	}
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
  switch(__even_in_range(ADC12IV,34))
  {
//  case  0: break;                           // Vector  0:  No interrupt
//  case  2: break;                           // Vector  2:  ADC overflow
//  case  4: break;                           // Vector  4:  ADC timing overflow
  case  6:                                  // Vector  6:  ADC12IFG0
    		adc12_result = ADC12MEM0;                       // Move results, IFG is cleared
    		adc12_data_ready = 1;
    		//_BIC_SR_IRQ(LPM3_bits);   						// Exit active CPU
    		break;
//  case  8: break;                           // Vector  8:  ADC12IFG1
//  case 10: break;                           // Vector 10:  ADC12IFG2
//  case 12: break;                           // Vector 12:  ADC12IFG3
//  case 14: break;                           // Vector 14:  ADC12IFG4
//  case 16: break;                           // Vector 16:  ADC12IFG5
//  case 18: break;                           // Vector 18:  ADC12IFG6
//  case 20: break;                           // Vector 20:  ADC12IFG7
//  case 22: break;                           // Vector 22:  ADC12IFG8
//  case 24: break;                           // Vector 24:  ADC12IFG9
//  case 26: break;                           // Vector 26:  ADC12IFG10
//  case 28: break;                           // Vector 28:  ADC12IFG11
//  case 30: break;                           // Vector 30:  ADC12IFG12
//  case 32: break;                           // Vector 32:  ADC12IFG13
//  case 34: break;                           // Vector 34:  ADC12IFG14
  default: break;
  }
}
