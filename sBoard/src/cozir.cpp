/*
 * cozir.cpp
 *
 *  Created on: Jan 21, 2016
 *      Author: Greg Perrin
 */

#include "../inc/cozir.h"

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

/*
 * Sensor interface Constructor
 *
 * args: none
 */
Cozir::Cozir()
{
	this->uart_bus = new Uart();
}

/*
 * Cozir interface Constructor
 *
 * args: none
 */
Cozir::~Cozir()
{

}

/*
 * Cozir setup
 *
 * args: none
 */
void Cozir::setup()
{

}

/*
 * Cozir setup
 *
 * args: none
 */
void Cozir::get_co2()
{
	this->uart_bus->tx(co2_measure);
}

/*
 * Sensor setup
 *
 * args: none
 */
void Cozir::get_temp()
{
	this->uart_bus->tx(temp_measure);
}

/*
 * Sensor setup
 *
 * args: none
 */
void Cozir::get_humidity()
{
	this->uart_bus->tx(humidity_measure);
}

/*
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_A0_VECTOR             // USCI ISR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	UCA0IFG &=~ UCRXIFG;
	/*
	cozir_buffer[counter] = UCA0RXBUF;
	counter++;
	if(counter == 9)
	{
		memcpy(cozir_array, &cozir_buffer, strlen(cozir_buffer)+1);
	    __delay_cycles(1000);
	}


}
*/
