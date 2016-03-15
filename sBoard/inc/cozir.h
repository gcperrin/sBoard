/*
 * cozir.h
 *
 *  Created on: Mar 3, 2016
 *      Author: Greg Perrin
 */

#ifndef INC_COZIR_H_
#define INC_COZIR_H_


#include "uart.h"

class Cozir {

	typedef struct data_entry {
		float value;
		char unit;
	} data_entry;

	public:
		Cozir();
		virtual ~Cozir();
		void setup();
		void get_co2();
		void get_temp();
		void get_humidity();
		void get_all();

	private:
		Uart* uart_bus;
};

#endif /* INC_COZIR_H_ */
