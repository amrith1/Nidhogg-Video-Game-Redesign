// Lab10.c Sword Capture the Flag Game
// Runs on LM4F120 or TM4C123
// Students: Amrith Lotlikar and Kunal Jain
// Last Modified: 4/22/18

// Analog Input connected to PD2=ADC1
// displays on Sitronox ST7735
// UART1 on PC4-5

#include <stdint.h>
#include "ST7735.h"
#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include "tm4c123gh6pm.h"
#include "UART.h"
#include "FiFo.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

extern unsigned short RestYellow[];
extern unsigned short RestYellow2[];
extern unsigned short RestBlue[];
extern unsigned short RestBlue2[];
extern unsigned short Sword[];
extern unsigned short SwordDown[];
extern unsigned short HisSword[];
extern unsigned short HisSwordDown[];
extern unsigned short PullUp[];
extern unsigned short HePullUp[];
extern unsigned short Lunge[];
extern unsigned short LungeLow[];
extern unsigned short HeLunge[];
extern unsigned short HeLungeLow[];
// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
	volatile int delay = 0;
	SYSCTL_RCGCGPIO_R |= 0x20; //ENABLE PORT F
	delay ++;
	delay ++;
	GPIO_PORTF_AMSEL_R &= ~0x0E;
	GPIO_PORTF_PCTL_R &= ~0x0E;
	GPIO_PORTF_DIR_R |= 0x0E;
	GPIO_PORTF_DEN_R |= 0x0E;
	GPIO_PORTF_DATA_R = 0x08;
}

void PortB_Init(void){
	volatile int delay = 0;
	SYSCTL_RCGCGPIO_R |= 0x02; //ENABLE PORT B
	delay ++;
	delay ++;
	GPIO_PORTB_DIR_R = 0x0F;
	GPIO_PORTB_DEN_R = 0xFF;
}


//SysTick handles the frame refresh interrupts
void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = 3333333;		// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
  NVIC_ST_CTRL_R |= 0x07;  		// enable SysTick with core clock and interrupts
}


uint32_t Convert(uint32_t input){
  return (21789*input)/50000+23;
}

uint8_t control;
uint8_t measured;
uint8_t clearfreq;
uint8_t displayed;
int32_t position100;
uint8_t position[2];
uint8_t elevation[2];
uint8_t reach[2];
uint8_t jump[3]; 
uint8_t alternate[2];
uint8_t modified;
uint8_t transmitted;
uint8_t received;
uint8_t swordposition[4];
int velocity;
int8_t redvelocity[2];
int8_t jumparr [17] = {10, 9, 7, 7, 5, 3 , 2, 1, 0, -1, -2, -3, -5, -7, -7, -9, -10};
uint8_t lungearr [7] = {19, 0, 0, 0, 0, 0, 0 };
uint8_t jumpindex[2];
uint8_t lungeindex[2];
uint8_t winner;
uint8_t dead[2];
uint8_t blocked[2]; //This is player 0. blocked[0] is 1 if I get blocked
uint8_t deadindex[2];
uint8_t gameswon[2];
uint8_t nextgame;
uint8_t over;

void Measure()
{
	if((swordposition[2] < 3) && (dead[0] == 0))
	{
			blocked[0] = 0;
			velocity = Convert(ADC_In());
			velocity -= 900;
			velocity /= 2;
		
			if(136 - position[1] <= position[0])
			{
				if(velocity < 0)
						{
							velocity = 0;
						}
							
			
			}
		
			
			if(velocity<100 && velocity>-100) velocity = 0;
			redvelocity[0] = velocity/100;
			
		jump[2] = 1;
			
		
			if(jump[0] == 1)
			{
				jump[2] = GPIO_PORTB_DATA_R & 0x10;
				jump[2] /= 16;
			}
			
			
			if(jump[0] == 0)
			{jump[0] = GPIO_PORTB_DATA_R & 0x10;
			 jump[0] /= 16;
			}
			
			if(swordposition[2] == 2)
			{swordposition[0] = 0;}
			
			uint32_t swordvar = Convert(ADC_In2());
			if(swordvar>1500) swordposition[0] = 0;
			if(swordvar < 200) swordposition[0] =1;
			
			
			
			if(jump[0] == 0)
					{
						if((GPIO_PORTB_DATA_R & 0x40) != 0)
								{
									swordposition[0] = swordposition[2] + 3;
									if(swordposition[0] > 4) swordposition[0] =4;
								}
					
						if((GPIO_PORTB_DATA_R & 0x20) != 0)
								{
									swordposition[0] = 2;
								}
						
					}
			
		}
	measured = 1;
}

void Modify(void)
{
		if(dead[0] == 0)
		{
					if(swordposition[0] < 2)
					
					{				
									blocked[0] = 0;
									if(velocity)
											{alternate[0] += 1;
											alternate[0] %= 4;}
									if(velocity > 200 || velocity < -200)
											{alternate[0] += 2;
											alternate[0] %= 4;}
									
									
									position100 += velocity;
											
									if(position100 < 0) position100 = 0;
									if(position100>14000) position100 = 14000;
											
									position[0] = position100 / 100;

									if(jump[0] == 1)
										{
											
											if( (jumpindex[0] < 8) & (jump[2] == 0) )
											{
												jumpindex[0] = 17 - jumpindex[0];
											}
										

											if(jumpindex[0] == 17){jumpindex[0] = 0; jump[0] = 0;}
											else
												{
													elevation[0] += jumparr[jumpindex[0]];
													jumpindex[0]++;
												}
										}
					
					}
					
					if(swordposition[0]>2)
						{
							if(lungeindex[0] == 6){lungeindex[0] = 0; swordposition[0] = swordposition[2] - 3; reach[0] = 0;}
							else
								{
									reach[0] += lungearr[lungeindex[0]];
									lungeindex[0]++;
								}
						
						}
			}
		modified = 1;
	
}
uint8_t drawpack[2];
void Transmit()
{
	DisableInterrupts();
	UART_OutChar(0xFF);
	UART_OutChar(position[0]);
	UART_OutChar(redvelocity[0]);
	UART_OutChar(elevation[0]);
	drawpack[0] = 8 * jumpindex[0] + 2 * alternate[0] + jump[0];
	UART_OutChar(drawpack[0]);
	drawpack[1] = 32 * swordposition[0] + reach[0]; 
	UART_OutChar(drawpack[1]);               //available transmission space
	UART_OutChar(0x00);								//available transmission space
	UART_OutChar(0xFE);               //end
	transmitted = 1;
	control = 0;
	EnableInterrupts();
	
}

void DrawMe()
{

if(swordposition[0] < 2){
					if (alternate[0] > 1)
					{
					ST7735_DrawBitmap(position[0], 110 - elevation[0], RestYellow, 24, 27);
					
					}
					else
					{
					ST7735_DrawBitmap(position[0], 110 - elevation[0], RestYellow2, 24, 27);
					
					}
					
					if(jump[0] == 1){}
					else if(swordposition[0] == 0){
					ST7735_DrawBitmap(position[0] + 16, 93 - elevation[0], Sword, 3, 9);
					ST7735_DrawPixel(position[0] + 17, 95 - elevation[0], 0xC6F8);
					}
					else{
					ST7735_DrawBitmap(position[0] + 16, 103 - elevation[0], SwordDown, 3, 11);
					ST7735_DrawPixel(position[0] + 17, 95 - elevation[0], 0xC6F8);
					}
				}
else if (swordposition[0] == 2)
		{
				ST7735_DrawBitmap(position[0], 110, PullUp, 24, 27);
				ST7735_DrawBitmap(position[0] + 10, 85, Sword, 3, 9);
				ST7735_DrawPixel(position[0] + 11, 84, 0xC6F8);
				ST7735_FillRect(position[0] + 11, 73, 1, 15, 0xC6F8);
		 }
else if (swordposition[0] == 3)
		{
				ST7735_DrawBitmap(position[0], 110, Lunge, 24, 27);
		 }	

else
		{
				ST7735_DrawBitmap(position[0], 110, LungeLow, 24, 27);
		 }			 

	
	if(jump[0])
	{	
	if(jumparr[jumpindex[0] - 1] > 0) 
	ST7735_FillRect(position[0] - redvelocity[0], 110-elevation[0], 30, jumparr[jumpindex[0] - 1], 0x0000);
	if(jumparr[jumpindex[0] - 1] < -1) 
	ST7735_FillRect(position[0] - redvelocity[0], 110-elevation[0]- 27 + jumparr[jumpindex[0] - 1], 30, -1 * jumparr[jumpindex[0] - 1] + 1, 0x0000);
	}
}

void DrawHim()
{
if(swordposition[1] < 2)
	{
	if (alternate[1] > 1)
	{
	ST7735_DrawBitmap(136 - position[1], 110 - elevation[1], RestBlue, 24, 27);
	}
	else
	{
	ST7735_DrawBitmap(136 - position[1], 110 - elevation[1], RestBlue2, 24, 27);	   
	}
					
					if(jump[1] == 1)
					{
						
					}
					else if(swordposition[1] == 0){
					ST7735_DrawBitmap(136 - position[1] + 5, 93 - elevation[1], HisSword, 3, 9);
					ST7735_DrawPixel(136 - position[1] + 6, 95 - elevation[1], 0xC6F8);
					}
					else{
					ST7735_DrawBitmap(136 - position[1] + 5, 103 - elevation[1], HisSwordDown, 3, 11);
					ST7735_DrawPixel(136 - position[1] + 6, 97 - elevation[1], 0xC6F8);
					}
	
	
}
	
	else if (swordposition[1] == 2)
		{
			ST7735_DrawBitmap(136 - position[1], 110, HePullUp, 24, 27);
			ST7735_DrawBitmap(147 - position[1], 85, Sword, 3, 9);
			ST7735_DrawPixel(148 - position[1], 84, 0xC6F8);
			ST7735_FillRect(148 - position[1], 73, 1, 15, 0xC6F8);
		
		}	
		
	else if(swordposition[1] == 3)
		{
			ST7735_DrawBitmap(136 - position[1], 110, HeLunge, 24, 27);
		
		}
	else 
		{
			ST7735_DrawBitmap(136 - position[1], 110, HeLungeLow, 24, 27);
		
		}
		
		
		if(jump[1])
	{	
	if(jumparr[jumpindex[1] - 1] > 0) 
	ST7735_FillRect(136 - position[1] + redvelocity[1], 110-elevation[1], 30, 2 + elevation[1], 0x0000);
	if(jumparr[jumpindex[1] - 1] < -1) 
	ST7735_FillRect(136 - position[1] + redvelocity[1], 40, 30, 50 - elevation[1], 0x0000);
	}

}

void CleanUp()
{
if(swordposition[2] == 2 || swordposition[3] == 2)
					{
					ST7735_FillRect(position[0] -3, 73, 30, 15, 0x000);
					ST7735_FillRect(136 - position[1] -3, 73, 30, 15, 0x000);
					}	
				
			
	if(swordposition[2] == 3) ST7735_FillRect(position[0] + 24, 92, 25, 1, 0x0000);
	if(swordposition[2] == 4) ST7735_FillRect(position[0] + 24, 99, 25, 1, 0x0000);
	if(swordposition[3] == 3) ST7735_FillRect(136 - position[1] -25, 92, 25, 1, 0x0000);
	if(swordposition[3] == 4) ST7735_FillRect(136 - position[1] - 25, 99, 25, 1, 0x0000);
			if(deadindex[0] == 0)
			{
				if(dead[0] == 1) deadindex[0]++;
			}
		else 
			{
				if(deadindex[0] < 20) deadindex[0]++;
			}
	if(deadindex[0] == 1)
			{
					ST7735_FillRect(position[0] - 6, 75 - elevation[0], 36, 35 + elevation[0],0x0000);
			}
	if(deadindex[1] == 0)
			{
				if(dead[1] == 1) deadindex[1]++;
			}
		else 
			{
				if(deadindex[1] < 20) deadindex[1]++;
			}
	if(deadindex[1] == 1)
			{
					ST7735_FillRect(136 - position[1] - 6, 75 - elevation[1], 36, 35 + elevation[1],0x0000);
			}
	/*	
		uint8_t exp_x[11] = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};
		uint8_t exp_y[11] = {0, 3, 4, 4, 5, 5, 5, 4, 4, 3, 0};
		
		if(deadindex[0] == 2)
		{
			for(int i = 0; i < 11; i++)
			ST7735_FillRect(position[0] + 11 + 3*exp_x[i], 111 - elevation[0] - 14 - 3 * exp_y[i], 3, 3, me_color) ;
		}
		
		if(deadindex[0] == 4)
		{
			for(int i = 0; i < 11; i++)
			ST7735_FillRect(position[0] + 11 + 3*exp_x[i], 111 - elevation[0] - 14 - 3 * exp_y[i], 3, 3, 0x0000) ;
			for(int i = 0; i < 11; i++)
			ST7735_FillRect(position[0] + 11 + 6*exp_x[i], 111 - elevation[0] - 14 - 6 * exp_y[i], 3, 3, me_color) ;
		}
		
		if(deadindex[0] == 6)
		{
			for(int i = 0; i < 11; i++)
			ST7735_FillRect(position[0] + 11 + 6*exp_x[i], 111 - elevation[0] - 14 - 6 * exp_y[i], 3, 3, 0x0000) ;
			for(int i = 0; i < 11; i++)
			ST7735_FillRect(position[0] + 11 + 9*exp_x[i], 111 - elevation[0] - 14 - 9 * exp_y[i], 3, 3, me_color) ;
		}
		
		if(deadindex[0] == 8)
		{
			for(int i = 0; i < 11; i++)
			ST7735_FillRect(position[0] + 11 + 9*exp_x[i], 111 - elevation[0] - 14 - 9 * exp_y[i], 3, 3, 0x0000) ;
			for(int i = 0; i < 11; i++)
			ST7735_FillRect(position[0] + 11 + 12*exp_x[i], 111 - elevation[0] - 14 - 12 * exp_y[i], 3, 3, me_color) ;
		}
		
			if(deadindex[0] == 10)
		{
			for(int i = 0; i < 11; i++)
			ST7735_FillRect(position[0] + 11 + 12*exp_x[i], 111 - elevation[0] - 14 - 12 * exp_y[i], 3, 3, 0x0000) ;
		}
		
		if(deadindex[1] == 2)
		{
			for(int i = 0; i < 11; i++)
			ST7735_FillRect(position[1] + 11 + 3*exp_x[i], 111 - elevation[1] - 14 - 3 * exp_y[i], 3, 3, enemy_color) ;
		}
		
		if(deadindex[1] == 4)
		{
			for(int i = 0; i < 11; i++)
			ST7735_FillRect(position[1] + 11 + 3*exp_x[i], 111 - elevation[1] - 14 - 3 * exp_y[i], 3, 3, 0x0000) ;
			for(int i = 0; i < 11; i++)
			ST7735_FillRect(position[1] + 11 + 6*exp_x[i], 111 - elevation[1] - 14 - 6 * exp_y[i], 3, 3, enemy_color) ;
		}
		
		if(deadindex[1] == 6)
		{
			for(int i = 0; i < 11; i++)
			ST7735_FillRect(position[1] + 11 + 6*exp_x[i], 111 - elevation[1] - 14 - 6 * exp_y[i], 3, 3, 0x0000) ;
			for(int i = 0; i < 11; i++)
			ST7735_FillRect(position[1] + 11 + 9*exp_x[i], 111 - elevation[1] - 14 - 9 * exp_y[i], 3, 3, enemy_color) ;
		}
		
		if(deadindex[1] == 8)
		{
			for(int i = 0; i < 11; i++)
			ST7735_FillRect(position[1] + 11 + 9*exp_x[i], 111 - elevation[1] - 14 - 9 * exp_y[i], 3, 3, 0x0000) ;
			for(int i = 0; i < 11; i++)
			ST7735_FillRect(position[1] + 11 + 12*exp_x[i], 111 - elevation[1] - 14 - 12 * exp_y[i], 3, 3, enemy_color) ;
		}
		
			if(deadindex[1] == 10)
		{
			for(int i = 0; i < 11; i++)
			ST7735_FillRect(position[1] + 11 + 12*exp_x[i], 111 - elevation[1] - 14 - 12 * exp_y[i], 3, 3, 0x0000) ;
		}
		
		*/
		


}
void ShowStatus()
{
	ST7735_SetCursor(0,0);
	ST7735_OutString("You: ");
	LCD_OutDec(gameswon[0]);
	ST7735_SetCursor(12, 0);
	ST7735_OutString("Them: ");
	LCD_OutDec(gameswon[1]);
	ST7735_SetCursor(3,2);
	
	if(winner == 1)
	{ST7735_OutString("Y'won this round");}
	else if(winner == 2)
	{ST7735_OutString("Y'lost this round");}
	else if(dead[1] == 1)
		{
			ST7735_OutString("You murderer!");
		}
	else if(dead[0] == 1)
		{
			ST7735_OutString("You ded fam! ");
		}
	else if (blocked[0])
	{
			ST7735_OutString("Rejected!    ");
	}
	else if (blocked[1])
	{
			ST7735_OutString("Nice block!! ");
	}

}
void Display()
{
			
			if(transmitted && measured && modified)
			{
			
			CleanUp();
							
					
			if(dead[0] == 0)			
			{DrawMe();}
			if(dead[1] == 0)
			{DrawHim();}
			
			if(dead[0] == 0 && blocked[0] == 0)
			{
			if(swordposition[0] == 3) ST7735_FillRect(position[0] + 24, 92, reach[0], 1, 0xC6F8);
			if(swordposition[0] == 4) ST7735_FillRect(position[0] + 24, 99, reach[0], 1, 0xC6F8);
			}
			if(dead[1] == 0 && blocked[1] == 0)
			{
			if(swordposition[1] == 3) ST7735_FillRect(136 - position[1] -reach[1], 92, reach[1], 1, 0xC6F8);
			if(swordposition[1] == 4) ST7735_FillRect(136 - position[1] - reach[1], 99, reach[1], 1, 0xC6F8);
			}
			
			
			

			swordposition[2] = swordposition[0];
			swordposition[3] = swordposition[1];
			
			ShowStatus();
			
			
			if(winner != 0) nextgame = 1;
			
			
			}
			measured  = 0;
			modified = 0;
			transmitted = 0;
			displayed = 1;
		}


void Initialize_Game()
	{
		ST7735_FillRect(0, 0, 160, 112, 0x0000);
		ST7735_FillRect(0, 112, 160, 16,0x0911);
		ST7735_SetCursor(0,0);
		ST7735_OutString("You: ");
		LCD_OutDec(gameswon[0]);
		ST7735_SetCursor(12, 0);
		ST7735_OutString("Them: ");
		LCD_OutDec(gameswon[1]);
		position100 = 0;
		position[0] = 0;
		position[1] = 0;
		elevation[0] = 0;
		elevation[1] = 0;
		jump[0] = 0;
		jump [1] = 0;
		redvelocity[1] = 0;
		redvelocity[0] = 0;
		jumpindex[0] = 0;
		jumpindex[1] = 0;
		alternate[0] = 0;
		swordposition[0] = 0;
		swordposition[3] = 0;
		swordposition[2] = 0;
		lungeindex[0] = 0;
		alternate[1] = 0;
		
		nextgame = 0;
		measured = 0;
		modified = 0;
		transmitted = 0;
		displayed = 1;  
		control = 1;
		
		dead[0] = 0;
		dead[1] = 0;
		blocked[0] = 0;
		blocked[1] = 0;
		winner = 0;
		deadindex[0] = 0;
		deadindex[1] = 0;
	}
void GameOver()
{
	ST7735_FillScreen(0x0000);
	if(gameswon[0] == 5)
	{
		ST7735_SetCursor(5,5);
		ST7735_OutString("Congratulations!");
		ST7735_SetCursor(5,6);
		ST7735_OutString("You Won!");
	}
	else
	{
		ST7735_SetCursor(5,5);
		ST7735_OutString("I'm sorry.");
		ST7735_SetCursor(5,6);
		ST7735_OutString("You lost.");
		
	}


}
	void StartNewRound()
		{
			DisableInterrupts();
			if(winner == 1){gameswon[0]++;}
			if(winner == 2){gameswon[1]++;}
			if(gameswon[0] == 5 || gameswon[1] == 5){over = 1;}
			if(!over)
			{	
			volatile int delay = 0;
			for(int i = 1; i< 10000000; i++)
			{delay++;}
			Initialize_Game();
			for(int i = 1; i< 10000000; i++)
			{delay++;}
			ST7735_SetCursor(3,2);
			ST7735_OutString("Go!          ");
			EnableInterrupts();
		}
		}
	void PlayGame()
	{
		gameswon[0] = 0;
		gameswon[1] = 0;
		over = 0;
		Initialize_Game();
		MoveToRAM();
		
		EnableInterrupts();
		while(!over)
		{
			if(!displayed){Display();}
			if(nextgame) StartNewRound();
			if(!over)
			{
			if(control && !measured){Measure();}
			if(control && measured && !modified){Modify();}
			if(control && measured && modified && !transmitted){Transmit();}
			}
			if(over) GameOver();
		}
	
	
	}
	

int main(void)
	{ 
		// The grid is 160 by 128
		TExaS_Init();       // Bus clock is 80 MHz 
		ST7735_InitR(INITR_REDTAB);
		ADC_Init();    // initialize to sample ADC
		PortF_Init();
		PortB_Init();
		UART_Init();       // initialize UART
		ST7735_SetRotation(1);
		SysTick_Init();
		PlayGame();
	}

	

void SysTick_Handler(void)
	{ 
		displayed = 0;
	}
	
	uint8_t filler;
	uint8_t unpackaging[3];
void UART1_Handler(void){
	GPIO_PORTF_DATA_R ^= 0x04;					// PF1 - PF3 are heartbeats
	GPIO_PORTF_DATA_R ^= 0x04;					// PF1 - PF3 are heartbeats
  while((uint8_t)UART_InChar() != 0xFF){}
	position[1] = UART_InChar();
	redvelocity[1] = UART_InChar();
  elevation[1] = UART_InChar();
	unpackaging[0] = UART_InChar();
		
	jumpindex[1] = unpackaging[0]/8;
	alternate[1] = (unpackaging[0] % 8)/2;
	jump[1] = unpackaging[0] % 2;
		
	unpackaging[1] = UART_InChar();
	swordposition[1] = unpackaging[1]/32;
	reach[1] = unpackaging[1] % 32;
	unpackaging[2] = UART_InChar();
	
	blocked[0] = unpackaging[2] % 2;
	unpackaging[2] /= 2;
	blocked[1] = unpackaging[2] % 2;
	unpackaging[2] /= 2;
	dead[0] = unpackaging[2] % 2;
	unpackaging[2] /= 2;
	dead[1] = unpackaging[2] % 2;
	winner = (unpackaging[2] / 2)%4;
	
	
	
	filler = UART_InChar();
	UART1_ICR_R = 0x10;
	GPIO_PORTF_DATA_R ^= 0x04;					// PF1 - PF3 are heartbeats
	control = 1;
	}
