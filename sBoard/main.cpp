
#include "inc/cc1101.h"
#include "inc/sensor.h"

/*
 * clock_init
 *
 * Initialize all on-board clocks to 8MHz (DCO, MCLK, ACLK)
 *
 * args: none
 */
char txBuffer[4];
char rx_buffer[36];
CC1101* cc1101;
Sensor* cozir;
Sensor* mhp;
int counter;


void clock_init(void)
{
  CSCTL0_H = 0xA5;                           // Set password
  CSCTL1 |= DCOFSEL0 + DCOFSEL1;             // Set max. DCO setting
  CSCTL2 = SELA_3 + SELS_3 + SELM_3;         // set ACLK = XT1; MCLK = DCO
  CSCTL3 = DIVA_1 + DIVS_1 + DIVM_1;         // set all dividers
}


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	                    // Stop watchdog timer
	clock_init();

	cc1101 = new CC1101();                          // Set up CC1101
	cc1101->setup();
	cozir = new Sensor();                           // Set up Cozir
	cozir->setup(1);

	__delay_cycles(1000);
	bool cc1101_status = cc1101->check_config();    // Error check on CC1101, will power cycle and retry on failure
	if (cc1101_status == true) {
		// CC1101 ready

		/* while(true) {
			// Poll recieve flag every second and wait.
			__delay_cycles(1000);
			// temporarily disable GDO0 interrupt
			GDO0_PxIE &= ~(GDO0_PIN);
			char size = 3;
			txBuffer[0] = 2;		               //this is the size byte (it lets the receiver know how many bytes are following)
			txBuffer[1] = 0x01;		               //first byte
			txBuffer[2] = 0xAA;		               //second byte
			cc1101->send_packet(txBuffer, size);
			GDO0_PxIFG &= ~(GDO0_PIN);             // Clear flag
			//GDO0_PxIE |= GDO0_PIN;               // Enable int on end of packet
		} */
		 cc1101->flush_rx();
		 cc1101->set_receive();
		 while (true) {
			 // In future, wake on radio must be employed (see docs)
			 counter = 0;
			 __delay_cycles(10000);
			 cozir->get_values();
		 }
	} else {
		// Blink LED for ERR status
		P1DIR |= BIT0;
		P1OUT |= BIT0;
		while(1) {
			P1OUT ^= BIT0;
		    __delay_cycles(100000);
		}
	}

	// See sensor.h for member functions
	//Sensor* cozir = new Sensor();           // Set up Cozir C02

}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
	#pragma vector=PORT1_VECTOR
	__interrupt void PORT_1(void)
#elif defined(__GNUC__)
	void __attribute__ ((interrupt(PORT1_VECTOR))) PORT1ISR (void)
#else
	#error Compiler not supported!
#endif
{

		// Set IDLE
		cc1101->set_idle();
		char len = 32;
		if(cc1101->receive_packet(rx_buffer, &len)) {
			// Some examples
			//short s = cc1101.GetRSSI();
			//LED_PxOUT ^= LED1;
			//cc1101->set_receive();
		} else {
			//CRC did not check out or buffer was too small to hold received packet
			//cc1101->set_receive();
			//cc1101->flush_rx();
		}

		if (GDO2_PxIFG & GDO2_PIN) {
			GDO2_PxIFG &= ~(GDO2_PIN);
		}
		cc1101->flush_rx();
		cc1101->set_receive();
		//LPM3_EXIT;

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

