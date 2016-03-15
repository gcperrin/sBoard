/*
 * uart.c
 *
 *  Created on: Jan 19, 2016
 *      Author: Greg Perrin
 */

#include <string.h>
#include "../inc/uart.h"

/*
 * Sensor commands (will stay constant)
 */
const char *command_mode  		= "K 0\r\n";
const char *stream_mode   		= "K 1\r\n";
const char *poll_mode     		= "K 2\r\n";
const char *co2_measure   		= "z\r\n";
const char *temp_measure  		= "T\r\n";
const char *humidity_measure   	= "H\r\n";
const char *full_tilt           = "M 4164\r\n";


char cozir_buffer[20];
char* cozir_array;



Uart::Uart()
{
	// Configure TX/RX pins
	P2SEL1 |= BIT0 + BIT1;
	P2SEL0 &= ~(BIT0 + BIT1);

	// Configure UART 0
	UCA0CTL1 |= UCSWRST;
	UCA0CTL1 = UCSSEL_2;      // Set DCO = 8MHz as UCBRCLK
	UCA0BR0 = 26;             // 9600 baud
	UCA0BR1 = 0;
	UCA0MCTLW |= 0xB601;      // This one was a doosie

	UCA0CTL1 &= ~UCSWRST;     // release from reset
	UCA0IE |= UCRXIE;         // Enable RX interrupt
	__enable_interrupt();
}

Uart::~Uart()
{

}

void Uart::tx(const char* command)
{
	// Send command.
	//char commands[] = "K 1\r\n";
	int i=0;
	for(i=0; i < strlen(command); i++) {
		while (!(UCA0IFG&UCTXIFG));
		UCA0TXBUF = command[i];
	}
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
	#pragma vector = USCI_A0_VECTOR
	__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
	void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
	#error Compiler not supported!
#endif
{
	int counter;
	UCA0IFG &= ~(UCRXIFG);
	cozir_buffer[counter] = UCA0RXBUF;
	counter++;
	if(counter == 9) {
		memcpy(cozir_array, &cozir_buffer, strlen(cozir_buffer)+1);
		counter = 0;
	    __delay_cycles(1000);
	}

}
