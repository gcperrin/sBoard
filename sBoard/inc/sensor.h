/*
 * sensor.h
 *
 *  Created on: Jan 21, 2016
 *      Author: Greg Perrin
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include "uart.h"

class Sensor {

	typedef struct data_entry {
		float value;
		char unit;
	} data_entry;

	public:
		Sensor();
		virtual ~Sensor();
		void setup(int type);

	private:
		int type;
		Uart uart_bus;
		//Adc adc_bus;
};

#endif /* SENSOR_H_ */
