// FiFo.c
// Runs on LM4F120/TM4C123
// Provide functions that implement the Software FiFo Buffer
// Last Modified: 4/10/2017 
// Student names: Kunal Jain
// Last modification date: 4/18/18

#include <stdint.h>
#include "ST7735.h"

// --UUU-- Declare state variables for FiFo
//        size, buffer, put and get indexes
#define SIZE 9

uint8_t buffer[9];
uint8_t put_i;
uint8_t get_i;
uint8_t count;


// *********** FiFo_Init**********
// Initializes a software FIFO of a
// fixed size and sets up indexes for
// put and get operations
void FiFo_Init() {
	count = 0;
	put_i = 0;
	get_i = 0;
}

// *********** FiFo_Put**********
// Adds an element to the FIFO
// Input: Character to be inserted
// Output: 1 for success and 0 for failure
//         failure is when the buffer is full
uint32_t FiFo_Put(char data) {
	
	if (count == SIZE){
		return 0;
	}
	else{
		buffer[put_i] = data;
		put_i = (put_i + 1) % 9;
		count += 1;
		//ST7735_OutChar(data);
		return 1;
	}
}

// *********** FiFo_Get**********
// Gets an element from the FIFO
// Input: Pointer to a character that will get the character read from the buffer
// Output: 1 for success and 0 for failure
//         failure is when the buffer is empty
uint32_t FiFo_Get(char *datapt)
{
	if (count == 0){
		return 0;
	}
	else{
		*datapt = buffer[get_i];
		get_i = (get_i + 1) % 9;
		count -= 1;
		return 1;
	}
}



