/*
 * sensor.cpp
 *
 *  Created on: Jan 21, 2016
 *      Author: Greg Perrin
 */

#include "../inc/sensor.h"

/*
 * Sensor interface Constructor
 *
 * args: none
 */
Sensor::Sensor()
{

}

/*
 * Sensor interface Constructor
 *
 * args: none
 */
Sensor::~Sensor()
{

}

/*
 * Sensor setup
 *
 * args: type
 * 			1: UART
 * 			2: ADC
 */
void Sensor::setup(int type)
{
	this->type = type;
	if (type == 1) {

	} else if (type == 2) {

	} else {

	}
}

/*
 * Sensor setup
 *
 * args: type
 * 			1: UART
 * 			2: ADC
 */
void Sensor::get_values()
{
	if (this->type == 1) {
		uart->tx();


	} else if (type == 2) {

	} else {

	}
}
