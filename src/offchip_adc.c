
#include "offchip_adc.h"

void ofc_configure_adc(unsigned int i2c_address, unsigned int adc_port) {
	dint();
	i2c_set_slave(i2c_address);				// point to slave device
	eint();
	ofc_configure_adc_port(adc_port);   // configure the device to read off the correct port
}


void ofc_configure_adc_port(unsigned int adc_port) {

	// amount of bytes in the transaction. includes data you want to send as well as initial slave
	// address send
	//((1 << 7) & (3 << 4)  & (1 << 2))
														// First byte  : 0b10010000 (first 7 - bit I2C address followed by a low R / W bit)
	i2c_set_data(0, 0x01);								// Second byte : 0b00000001 (points to Config register)
	i2c_set_data(1, ((1 << 7) | adc_port | (1 << 2) | (1 << 0)) ); // Third byte  : 0b11000100 (MSB of the Config register to be written)
	i2c_set_data(2, ((1 << 7) | 3 ));					// Fourth byte : 0b10000011 (LSB of the Config register to be written)
	i2c_transmit(4);
	
	// Set back to conversion register
						  // First byte  : Set slave address
	i2c_set_data(2, 0x0); // Second byte : 0b00000000 (points to Conversion register)
	i2c_transmit(2);
}

void ofc_refresh_adc(unsigned int i2c_address, unsigned int adc_port) {
	ofc_configure_adc(i2c_address, adc_port); // Configure ADC
	i2c_receive(2);
	int combined = (i2c_get_data(STAGE_DATA_END_INDEX - 1) << 8) | (i2c_get_data(STAGE_DATA_END_INDEX -2) & 0xff);
	adc_values[_get_adc_index(i2c_address, adc_port)] = combined;
}

int ofc_get_adc_value(unsigned int i2c_address, unsigned int adc_port) {
	return adc_values[_get_adc_index(i2c_address, adc_port)];
}

int _get_adc_index(unsigned int i2c_address, unsigned int adc_port) {
	switch (i2c_address) {
	case ADC1:
		switch (adc_port) {
		case AIN0:
			return 0;
			break;
		case AIN1:
			return 1;
			break;
		case AIN2:
			return 2;
			break;
		case AIN3:
			return 3;
			break;
		}
		break;
	case ADC2:
		switch (adc_port) {
		case AIN0:
			return 4;
			break;
		case AIN1:
			return 5;
			break;
		case AIN2:
			return 6;
			break;
		case AIN3:
			return 7;
			break;
		}
		break;
	}
}
