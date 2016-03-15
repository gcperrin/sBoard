/*
 * uart.h
 *
 *  Created on: Jan 19, 2016
 *      Author: Greg Perrin
 */

#ifndef UART_H_
#define UART_H_

#include "msp430.h"

class Uart
{

	public:
		Uart();
		virtual ~Uart();
		void tx(const char* command);

};



#endif /* INC_UART_H_ */
