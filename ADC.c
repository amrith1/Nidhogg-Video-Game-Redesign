// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 3/28/2018 
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(void){ 
	volatile int delay = 0;

	SYSCTL_RCGCGPIO_R |= 0x08; //ENABLE PORT D
	delay ++;
	delay ++;
	delay ++;
	delay ++;
	delay /= 2;

	GPIO_PORTD_DIR_R &= ~0x04;      // 2) make PD2 input
  GPIO_PORTD_AFSEL_R |= 0x04;     // 3) enable alternate function on PD2
  GPIO_PORTD_DEN_R &= ~0x04;      // 4) disable digital I/O on PD2
  GPIO_PORTD_AMSEL_R |= 0x04;     // 5) enable analog functionality on PD2
	delay ++;
		SYSCTL_RCGCADC_R |= 0x01;
	for(delay = 0; delay < 1000; delay++){}
	delay *= 10;
	delay = 0;
	
	
	ADC0_PC_R &= ~0x0F;
  ADC0_PC_R |= 0x01;
	ADC0_SSPRI_R = 0x0123;
	ADC0_ACTSS_R &= ~0x08;
	ADC0_EMUX_R &= ~0xF000;
	ADC0_SSMUX3_R &= ~0xFF;
	ADC0_SSMUX3_R |= 0x05; //CHANNEL 5
	//ADC0_SSCTL3_R &= ~0xFF;
	ADC0_SSCTL3_R = 0x06;
	ADC0_IM_R &= ~0x08;
	ADC0_ACTSS_R |= 0x08;
	
	
	SYSCTL_RCGCADC_R |= 0x00000001; // 1) activate ADC0
	for(delay = 0; delay < 1000; delay++){}
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4; // 1) activate clock for Port E
  delay = SYSCTL_RCGCGPIO_R;      // 2) allow time for clock to stabilize
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTE_DIR_R &= ~0x10;      // 3) make PE4 PE5 input
  GPIO_PORTE_AFSEL_R |= 0x10;     // 4) enable alternate function on PE4 PE5
  GPIO_PORTE_DEN_R &= ~0x10;      // 5) disable digital I/O on PE4 PE5
                                  // 5a) configure PE4 as ?? (skip this line because PCTL is for digital only)
  //GPIO_PORTE_PCTL_R = GPIO_PORTE_PCTL_R&0xFF00FFFF;
  GPIO_PORTE_AMSEL_R |= 0x10;     // 6) enable analog functionality on PE4 PE5
  ADC0_PC_R &= ~0x0F;              // 8) clear max sample rate field
  ADC0_PC_R |= 0x01;               //    configure for 125K samples/sec
  ADC0_SSPRI_R = 0x0123;          // 9) Sequencer 3 is lowest priority
  ADC0_ACTSS_R &= ~0x0004;        // 10) disable sample sequencer 2
  ADC0_EMUX_R &= ~0x0F00;         // 11) seq2 is software trigger
  ADC0_SSMUX2_R = 0x0089;         // 12) set channels for SS2
  ADC0_SSCTL2_R = 0x0060;         // 13) no TS0 D0 IE0 END0 TS1 D1, yes IE1 END1
  ADC0_IM_R &= ~0x0004;           // 14) disable SS2 interrupts
  ADC0_ACTSS_R |= 0x0004;         // 15) enable sample sequencer 2
	
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){  
	ADC0_PSSI_R = 0x0008;            								// 1) initiate SS3
  while((ADC0_RIS_R&0x08)==0){};  					  		// 2) wait for conversion done
  uint32_t result = ADC0_SSFIFO3_R & 0xFFF;   		// 3) read result
  ADC0_ISC_R = 0x0008;            						 		// 4) acknowledge completion
  return result;
}

uint32_t ADC_In2(){ 
  uint32_t result;
	ADC0_PSSI_R = 0x0004;            // 1) initiate SS2
	//ADC0_PSSI_R = 0x0008;            // 1) initiate SS3
  //while((ADC0_RIS_R&0x08)==0){};   // 2) wait for conversion done
  while((ADC0_RIS_R&0x04)==0){};   // 2) wait for conversion done
	result = ADC0_SSFIFO2_R&0xFFF;   // 3) read result
  ADC0_ISC_R = 0x0004;             // 4) acknowledge completion
	//ADC0_ISC_R = 0x0008;             // 4) acknowledge completion
  return result;
	// HERE is the successive approximations
}


