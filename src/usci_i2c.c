
#include "usci_i2c.h"

/*
 *		Initialise I2C
 *		
 *		We're configuring the MSP430's to be in master mode and will switch between
 *		receiver and transmission modes. This will initialise the adc chips attached to
 *		the i2c interface attached to USCI_B1. We don't set the initial slave address to communicate with.
 *		should be done before each interaction with. 		
 *
 *		Preconditions:
 *			- interrupts are disabled
 */
void i2c_init(void) {

	// Setting up i2c to be in master mode so that it will address the connected
	i2c_ByteCtr = 0;
	P5SEL = 0x06;                            // Assign I2C pins to USCI_B1
	UCB1CTL1 = UCSWRST;                      // Enable SW reset
	UCB1CTL1 &= ~UCTR;						 // Clear bit to explicitly set reciever mode
	UCB1CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
	UCB1CTL0 &= ~UCSLA10;					  // explicit clear of for slave 7 bit addressing
	UCB1CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
	UCB1BR0 = 12;                             // fSCL = SMCLK/12 = ~100kHz
	UCB1BR1 = 0;
	i2c_set_slave(0x0);						  // set slave device to 0 initially
	UCB1CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation]
	//	UC1IE |= UCB1RXIE;                          // Enable RX interrupt
	//	UC1IE |= UCB1TXIE;                          // Enable TX interrupt

}


// Helper method for clearing the buffer data

void i2c_clear_data_buffer() {
	int i;
	for (i = 0; i <= STAGE_DATA_END_INDEX; i++) {
		i2c_set_data(i, 0x0);
	}
}

/* I'm adding this in case we want to change the underlying data structure
 * for the i2c buffer.
 * Precondition
 *	- Index is < STAGE_DATA_END_INDEX
 */
void i2c_set_data(unsigned int index, unsigned int byte) {
	i2c_DataStage[index] = byte;
}

char i2c_get_data(int index) {
	return i2c_DataStage[index];
}

void i2c_set_slave(unsigned int address) {
	UCB1I2CSA = address;
	UCB1CTL0 &= ~UCSLA10;			// explicit clear of for slave 7 bit addressing
}

void i2c_transmit(unsigned int byte_count) {
	i2c_ByteCtr = byte_count;		// Set the amount of bytes to send
	while (UCB1CTL1 & UCTXSTP);		// Ensure stop condition got sent
	UCB1CTL1 |= UCTR + UCTXSTT;     // I2C TX with start condition
	i2c_no_interrupts_transmit();	// transmit the data
	i2c_clear_data_buffer();		// clear the buffer after we've transmitted the data we need to
}

void i2c_receive(unsigned int byte_count) {
	i2c_ByteCtr = byte_count;		// Set the amount of data to receive
	while (UCB0CTL1 & UCTXSTP);     // Ensure stop condition got sent
	UCB1CTL1 &= ~UCTR;              // Clear I2C TX flag
	UCB1CTL1 |= UCTXSTT;            // I2C start condition
	i2c_clear_data_buffer();		// clear the buffer before we populate it with received data
	i2c_no_interrupts_receive();	// receive the data
}

void i2c_no_interrupts_receive(void) {

	while (UCB0CTL1 & UCTXSTT);        // Start condition sent?

	// while there is still data left to read in the buffer
	while (i2c_ByteCtr) {
		while (!(UC1IFG & UCB1RXIFG)); // loop while the interrupt flag is not set
		i2c_set_data(STAGE_DATA_END_INDEX - i2c_ByteCtr, UCB1RXBUF);
		i2c_ByteCtr--;
		UC1IFG &= ~UCB1RXIFG;		   // clear the flag
	}
	UCB1CTL1 |= UCTXSTP;
	while ((UCB1CTL1 & UCTXSTP));	  // while stop flag is still set
}

void i2c_no_interrupts_transmit(void) {
	while (UCB0CTL1 & UCTXSTT);			// Start condition sent?
										// buffer discarded// slave ready to receive
	while (i2c_ByteCtr) {
		while (!(UC1IFG & UCB1TXIFG));  // loop until the interrupt flag is set
		i2c_ByteCtr--;
		UCB1TXBUF = i2c_get_data(STAGE_DATA_END_INDEX - i2c_ByteCtr);
		UC1IFG &= ~UCB1TXIFG;
	}
	UCB1CTL1 |= UCTXSTP;
	while ((UCB1CTL1 & UCTXSTP));		// while stop flag is still set
}
