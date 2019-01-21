/*
 *
 *		I2C Driver 
 *		Author: BMaguire
 *
 */

#ifndef USCI_I2C_H
#define USCI_I2C_H

#include <msp430x24x.h>
#include <stdint.h>
#include "main.h"

/*
 *		BUFFER SIZE
 *		
 *		Most data that we send will usually only need to fit into 1 or 2
 *		bytes meant for registers.
 */
#define STAGE_DATA_END_INDEX 3
#define STAGE_DATA_LENGTH 4

/*
 *		TRANSMIT STATES | UNUSED
 *
 *		These states exist for keeping track of state inside and outside
 *		of the interrupts. This implementation of the code does not use interrupts
 *		but I've left these states as they worked and were useful. 
 */
#define I2C_ADDR_REQ 0
#define I2C_TRANSMITTING_DATA 2
#define I2C_STOP_TRANSMITTING_DATA 4
/*
 *		RECEIVE STATES | UNUSED
 *
 *		Much the same as above
 */
#define I2C_RECEIVING_DATA 10
#define I2C_STOP_RECEIVING_DATA 12

// This acts as a buffer for all the data we want to send over i2c
volatile unsigned int i2c_DataStage[STAGE_DATA_LENGTH];
volatile unsigned int i2c_ByteCtr;

void i2c_init(void);
void i2c_transmit(unsigned int byte_count);
void i2c_receive(unsigned int byte_count);
void i2c_no_interrupts_receive(void);
void i2c_no_interrupts_transmit(void);
void i2c_clear_data_buffer(void);
void i2c_set_data(unsigned int index, unsigned int byte);
void i2c_set_slave(unsigned int address);

#endif
