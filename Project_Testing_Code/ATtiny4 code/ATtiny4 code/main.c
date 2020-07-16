/**
 * @file 	main.c
 * @author 	Ahmed Ashraf (ahmedashrafelnaqeeb@gmail.com)
 * @brief   This file contains the starting point (main function) of the power manager application
 * @version 1.0
 * @date 	2020-07-12
 * 
 * @copyright Copyright (c) 2020
 * 
 */


/************************************************************************/
/*                         Includes                                     */
/************************************************************************/

#include "ATtiny4.h"

#define  F_CPU 8000000UL
#include "util/delay.h"

/************************************************************************/
/*            Application entry point & testing function                */
/************************************************************************/

void testApp(void)
{
	CLKMSR = 0x00; /*Selecting the internal oscillator with 8MHz*/
	CLKPSR = 0x00; /*Setting the clk prescaler to 1*/
	SET_BIT(DDRB , DDRB_PB1);
	
	while(1)
	{
		SET_BIT(PORTB , PORTB_PB1);
		_delay_ms(1000);
		
		CLEAR_BIT(PORTB , PORTB_PB1);
		_delay_ms(1000);
	}
	return;
}

/**
 * @brief  This the entry point of the power manager application
 * 
 * @return int 0 if everything is good and another value if there's an error 
 */
int main(void)
{
	testApp();
	
	while(1)
	{
	}
	return 0;
}