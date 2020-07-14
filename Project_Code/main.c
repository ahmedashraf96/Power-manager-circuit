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

#include "Functionality.h"

/************************************************************************/
/*                    Application entry point                           */
/************************************************************************/

/**
 * @brief  This the entry point of the power manager application
 * 
 * @return int 0 if everything is good and another value if there's an error 
 */
int main(void)
{
	/*Initializing the power manager circuit*/
	attiny4_init();
	
	while(1)
	{
		/*The main operation of the power manager circuit*/
		mainApplication();
	}
	return 0;
}