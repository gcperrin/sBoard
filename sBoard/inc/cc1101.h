/*
 * cc1101.h
 *
 *  Created on: Jan 19, 2016
 *      Author: Greg Perrin
 */

#ifndef CC1101_H_
#define CC1101_H_

#include "msp430.h"
#include "spi.h"
#include "../inc/cc1101_registers.h"

class CC1101 {

	public:
		CC1101();
		virtual ~CC1101();
		void setup();
		bool check_config();
		void write_config();
		void set_receive();
		void set_idle();
		void set_sleep();
		void flush_tx();
		void flush_rx();
		char receive_packet(char *rx_buffer, char *length);
		char poll_receive_flag();
		short get_rssi();
		void send_packet(char* tx_buffer, char size);

	private:
		Spi spi_bus;

};

#endif /* CC1101_H_ */
