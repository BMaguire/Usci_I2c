
/* 
 *		ADS1015 Driver
 *		Authour: BMaguire
 *
 *		On the boards we have adc chips that are not in the MSP430, we need to access and configure these
 *		through the i2c interface.
 *
 */
#ifndef OFFCHIP_ADC_H
#define OFFCHIP_ADC_H

#include <msp430x24x.h>
#include <signal.h>
#include "usci_i2c.h"

/*
 *		ADC Device Addresses
 *	
 *		Each ADS1015 has a hard coded address
 */
#define ADC_DEVICES 2
#define ADC1 0x48 // this address is the same on both Mass I/O and the Profet boards
#define ADC2 0x49 // this chip is only on the Mass I/O board

/* 
 *		ADC PORTS
 *	
 *		Each ADS1015 has 4 ports that can be configured
 *		these values are configured between the port and the ground
 */
#define ADC_PORTS 4
#define AIN0 (4 << 4) /* 0b01000000 : AINP = AIN0 and AINN = GND */
#define AIN1 (5 << 4) /* 0b01010000 : AINP = AIN1 and AINN = GND */
#define AIN2 (6 << 4) /* 0b01100000 : AINP = AIN2 and AINN = GND */
#define AIN3 (7 << 4) /* 0b01110000 : AINP = AIN3 and AINN = GND */

// simple array to store the latest returned values from the 8 ADC's or 
// 4 if we're on the profet boards.
// |		ADC1			|			ADC2		|
// [ AIN0, AIN1, AIN2, AIN3, AIN0, AIN1, AIN2, AIN3 ]
unsigned int adc_values[8];


void ofc_configure_adc(unsigned int i2c_address, unsigned int adc_port);
void ofc_refresh_adc(unsigned int i2c_address, unsigned int adc_port);
int ofc_get_adc_value(unsigned int i2c_address, unsigned int adc_port);
void ofc_configure_adc_port(unsigned int adc_port);
int _get_adc_index(unsigned int i2c_address, unsigned int adc_port);
#endif
