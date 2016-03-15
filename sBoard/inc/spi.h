/*
 * spi.h
 *
 *  Created on: Jan 19, 2016
 *      Author: Gregory Perrin
 */

#ifndef SPI_H_
#define SPI_H_

#define CSn_PxOUT         P1OUT
#define CSn_PxDIR         P1DIR
#define CSn_PIN           BIT3

class Spi
{

	public:
		Spi();
		virtual ~Spi();
		void write_register(char addr, char value);
		void write_burst_register(char addr, char *buffer, char count);
		char read_register(char addr);
		void read_burst_register(char addr, char *buffer, char count);
		char read_status(char addr);
		void strobe(char strobe);

};

#endif /* SPI_H_ */
