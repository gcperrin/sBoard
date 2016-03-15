/*
 * spi.cpp
 *
 *  Created on: Jan 19, 2016
 *      Author: Greg Perrin
 *
 *      P2.2: SCLK
 *      P1.6: SIMO
 *      P1.7: SOMI
 *      P1.3: CSn
 *      P1.5: GDO0 (TX Buffer confirm)   // Note P4.1 is bad for whatever reason
 *      P1.2: GDO2 (RX Bufer confirm)
 *      P2.0: UART TX
 *      P2.1: UART RX
 */

#include "msp430.h"
#include "../inc/spi.h"
#include "../inc/cc1101_registers.h"

/*
 * SPI interface Constructor
 *
 * args: none
 */
Spi::Spi()
{
	UCB0CTLW0 |= UCSWRST;                                              // Put Interface into reset
	UCB0CTLW0 |= UCSYNC + UCMSB + UCCKPH + UCMST + UCMODE_2 + UCSTEM;  // + UCMODE_2 + UCSTEM + UCCKPL;
	UCB0CTLW0 |= UCSSEL_2;                                             // Set to SCLK
	UCB0BR0 = 0x01;                                                    // 9600 baud
	UCB0BR1 = 0;
	UCA0MCTLW |= 0x0001;                                               // No modulation, clear anyway

	P1DIR  |= BIT3;
	P1SEL1 |= (BIT6 + BIT7);
	P2SEL1 |= BIT2;

	UCB0CTLW0 &= ~(UCSWRST);                                           // Take out of reset

	// Enable TX interrupt, enter LPM0...THIS IS FOR LATER
	//UCB0IE |= UCTXIE;
	//UCB0IE |= UCRXIE;

	//__bis_SR_register(GIE);
}

/*
 * SPI interface Deconstructor
 *
 * args: none
 */
Spi::~Spi()
{
	UCB0CTLW0 |= UCSWRST;		                                         // Put Interface into reset
	UCB0CTLW0 &= ~(UCSYNC + UCMSB + UCCKPH + UCMST);
	UCB0CTLW0 &= ~(UCSSEL_2);                         // Unset SCLK

	//UCB0MCTLW &= ~(0x0001);

	P2SEL1 &= ~(BIT0 + BIT1);
	P1SEL1 &= ~(BIT5);

	UCB0CTLW0 &= ~UCSWRST;                            // Take out of reset
}

/*
 * SPI write register at address w/ value
 *
 * args:
 *   addr  - CC1101 address
 *   value - CC1101 value
 */
void Spi::write_register(char addr, char value)
{
	CSn_PxOUT &= ~(CSn_PIN);                          // CS enable (bit has to go low before transmit is active)
	while (!(UCB0IFG&UCTXIFG));                       // Wait for TXBUF ready
	UCB0TXBUF = addr;                                 // Send address
	while (!(UCB0IFG&UCTXIFG));                       // Wait for TXBUF ready
	UCB0TXBUF = value;                                // Send data
	while (UCB0STATW & UCBUSY);                       // Wait for TX complete
	CSn_PxOUT |= CSn_PIN;                             // CS disable
}

/*
 * SPI write burst -- CC1101 construct, see data sheet
 *
 * args:
 *   addr   - CC1101 starting address
 *   buffer - value buffer
 *   value  - CC1101 value
 */
void Spi::write_burst_register(char addr, char *buffer, char count)
{
	unsigned int i;
	CSn_PxOUT &= ~(CSn_PIN);                          // CS enable
	while (!(UCB0IFG & UCTXIFG));                     // Wait for TXBUF ready
	UCB0TXBUF = (addr | WRITE_BURST);                 // Send address
	for (i = 0; i < count; i++) {
	    while (!(UCB0IFG & UCTXIFG));                 // Wait for TXBUF ready
	    UCB0TXBUF = buffer[i];                        // Send data
	}
	while (UCB0STATW & UCBUSY);                       // Wait for TX complete
	CSn_PxOUT |= CSn_PIN;                             // CS disable
}

/*
 * SPI read register
 *
 * args:
 *   addr - CC1101 	address
 */
char Spi::read_register(char addr)
{
	volatile char x;

	CSn_PxOUT &= ~CSn_PIN;                            // CS enable
	while (!(UCB0IFG&UCTXIFG));                       // Wait for TXBUF ready
	UCB0TXBUF = (addr | READ_SINGLE);                 // Send address
	while (!(UCB0IFG&UCTXIFG));                       // Wait for TXBUF ready
	UCB0TXBUF = 0;                                    // Dummy write so we can read data
	while (UCB0STATW & UCBUSY);                       // Wait for TX complete
	x = UCB0RXBUF;                                    // Read data
	CSn_PxOUT |= CSn_PIN;                             // CS disable

	return x;
}

/*
 * SPI read burst -- CC1101 construct, see data sheet
 *
 * args:
 *   addr   - CC1101 starting address
 *   buffer - Read buffer
 *   count  - character count
 */
void Spi::read_burst_register(char addr, char *buffer, char count)
{
	volatile char i;

	CSn_PxOUT &= ~(CSn_PIN);                           // CS enable
	while (!(UCB0IFG & UCTXIFG));                      // Wait for TXBUF ready
	UCB0TXBUF = (addr | READ_BURST);                   // Send address
	while (UCB0STATW & UCBUSY);                        // Wait for TX complete
	UCB0TXBUF = 0;                            	       // Dummy write to read 1st data byte

	// Addr byte is now being TX'ed, with dummy byte to follow immediately after
	UCB0IFG &= ~(UCRXIFG);                             // Clear flag
	while (!(UCB0IFG & UCRXIFG));                        // Wait for end of addr byte TX
	// First data byte now in RXBUF
	for (i = 0; i < (count-1); i++) {
		UCB0TXBUF = 0;                                 // Initiate next data RX, meanwhile..
		buffer[i] = UCB0RXBUF;                         // Store data from last data RX
		while (!(UCB0IFG&UCRXIFG));                    // Wait for RX to finish
	}
	buffer[count-1] = UCB0RXBUF;                       // Store last RX byte in buffer
	CSn_PxOUT |= CSn_PIN;                              // CS disable
}

/*
 * SPI read status
 *
 * args:
 *   addr - CC1101 starting address
 */
char Spi::read_status(char addr)
{
	volatile char status;

	CSn_PxOUT &= ~CSn_PIN;                             // CS enable
	while (!(UCB0IFG & UCTXIFG));                      // Wait for TXBUF ready
	UCB0TXBUF = (addr | READ_SINGLE);                   // Send address
	while (!(UCB0IFG & UCTXIFG));                      // Wait for TXBUF ready
	UCB0TXBUF = 0;                                     // Dummy write so we can read data
	while (UCB0STATW & UCBUSY);                        // Wait for TX complete
	status = UCB0RXBUF;                                // Read data
	CSn_PxOUT |= CSn_PIN;                              // CS disable

	return status;
}

/*
 * SPI strobe
 *
 * args:
 *   strobe - CC1101 strobe address
 */
void Spi::strobe(char strobe)
{
	CSn_PxOUT &= ~(CSn_PIN);                             // CS enable
	while (!(UCB0IFG&UCTXIFG));                        // Wait for TXBUF ready
	UCB0TXBUF = strobe;                                // Send strobe

	// Strobe addr is now being TX'ed
	while (UCB0STATW & UCBUSY);                        // Wait for TX complete
	CSn_PxOUT |= CSn_PIN;                              // CS disable
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
	#pragma vector=USCI_B0_VECTOR
	__interrupt void USCI_B0_ISR(void)
#elif defined(__GNUC__)
	void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
	#error Compiler not supported!
#endif
{
		volatile unsigned int i;

		switch(__even_in_range(UCA0IV, 0x04)) {
		    case 0: break;                   		       // Vector 0 - no interrupt
		    case 2:                                        // Data recieved
		    	char RXData = UCB0RXBUF;
		        UCB0IFG &= ~(UCRXIFG);
		        break;
		    case 4:                                        // Data sending
		    	//UCA0TXBUF = TXData;                      // Transmit characters
		    	//UCB0IFG &= ~(UCTXIFG);
		    	break;
		    default: break;
		  }
}

