// UART1.c
// Runs on LM4F120/TM4C123
// Use UART1 to implement bidirectional data transfer to and from 
// another microcontroller in Lab 9.  This time, interrupts and FIFOs
// are used.
// Daniel Valvano
// November 17, 2014
// Modified by EE345L students Charlie Gough && Matt Hawk
// Modified by EE345M students Agustinus Darmawan && Mingjie Qiu

/* Lab solution, Do not post
 http://users.ece.utexas.edu/~valvano/
*/

// U1Rx (VCP receive) connected to PC4
// U1Tx (VCP transmit) connected to PC5
#include <stdint.h>
#include "ST7735.h"
#include "FiFo.h"
#include "UART.h"
#include "tm4c123gh6pm.h"
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

uint32_t DataLost; 
// Initialize UART1
// Baud rate is 115200 bits/sec
// Make sure to turn ON UART1 Receiver Interrupt (Interrupt 6 in NVIC)
// Write UART1_Handler
void UART_Init(void){
  SYSCTL_RCGCUART_R |= 0x02;																				// initialize UART 1
	SYSCTL_RCGCGPIO_R |= 0x04;																				// initialize Port C
	UART1_CTL_R &= ~0x01;																							// Disable UART while baud rate is set
	UART1_IBRD_R = 10;																								//(80000000)/(16*115200) = 43.40277778
	UART1_FBRD_R = 22;																								// (.40277778 * 64) = 25.77777792
	UART1_LCRH_R = 0x0070;																						// defined in the book
	UART1_CTL_R = 0x0301;																							// enable RXE, TXE, and UART
	GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R & 0xFF00FFFF)+ 0x00220000;
	GPIO_PORTC_AMSEL_R &= ~0x30;
	GPIO_PORTC_AFSEL_R |= 0x30;
	GPIO_PORTC_DEN_R |= 0x30;
	
	UART1_IM_R |= 0x10;
	UART1_IFLS_R &= ~0x28;
	UART1_IFLS_R |= 0x10;
	NVIC_PRI1_R |= 0x03000000;
	NVIC_EN0_R |= 0X40;
	
	}

// input ASCII character from UART
// spin if RxFifo is empty
char UART_InChar(void){
  while((UART1_FR_R & 0x0010) != 0);		// waiting until RXFE = 0
	return((uint8_t)(UART1_DR_R & 0xFF));		
}
//------------UART1_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART_OutChar(char data){
  while((UART1_FR_R & 0x20) != 0);		// waiting until TXFF = 0
	UART1_DR_R = data;	
}

