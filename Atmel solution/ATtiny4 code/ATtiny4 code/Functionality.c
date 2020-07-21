/**
 * @file    Functionality.c
 * @author  Ahmed Ashraf (ahmedashrafelnaqeeb@gmail.com)
 * @brief   This file contains the interfacing functions logic implementation for the power manager application
 * @version 1.1
 * @date    2020-07-13
 * 
 * @copyright Copyright (c) 2020
 * 
 */

/************************************************************************/
/*                         Includes                                     */
/************************************************************************/

/*Operation code include files*/
#include "ATtiny4.h"
#include "Functionality.h"

/*AVR ATtiny4 utilities include files*/
#define   F_CPU 31250UL
#include "util/delay.h"
#include "avr/sleep.h"

/************************************************************************/
/*					       Important macros                             */
/************************************************************************/

#define TIMER0_CTC_MODE_SELECTION		(0x0008)
#define TIMER0_50MS_TICK				(1563)
#define TIMER0_PRESCALER_1				(0x0001)
#define TIMER0_OCR0A_INT_EN				(0x02)
#define IO_PINS_DIR_INITIALIZATION      (0x01)
#define IO_LOW_LEVEL					(0)
#define IO_HIGH_LEVEL					(1)
#define IO_PB2_PULLUP_ENABLE            (0x04)
#define IO_PB0_LL_PB2_PUP_ACTIVE        (0x04)
#define IO_PB0_HL_PB2_PUP_ACTIVE        (0x05)
#define SYSTEM_OFF_STATUS				(0xAA)
#define SYSTEM_ON_STATUS				(0x55)
#define ONE_SECOND                      (20)
#define TWO_SECONDS                     (40)
#define TEN_SECONDS                     (200)
#define INTERNAL_OSC_SELECT_8MHZ        (0x00)
#define ENABLE_CHANGE_FOR_IO_REG        (0xD8)
#define MAIN_CLK_PRESCALING_BY_256      (0x08)
		
/************************************************************************/
/*                        Important system variables                    */
/************************************************************************/

/*Variable used in counting switch time being pressed*/
u16_t gu16_switchCounter = 0;

/*Variable used to carry the system current status (ON or OFF) [The variable will keep its value till the power goes OFF]*/
u8_t  gu8_systemStatus __attribute__((section(".noinit")));

/************************************************************************/
/*                Power manager functions implementation                */
/************************************************************************/

void attiny4_init(void)
{ 
	/**
	  * Adjusting the MCU CLK section
	  */
	
	/*Select the internal oscillator of the MCU with 8MHz*/
	CLKMSR = INTERNAL_OSC_SELECT_8MHZ;
	
	/*Enable writing to the CLKPSR register*/
	CCP = ENABLE_CHANGE_FOR_IO_REG;
	
	/*Enable the pre-scaler of the main CLK by 256 which gives 31.25 KHz*/
	CLKPSR = MAIN_CLK_PRESCALING_BY_256;
	

	/**
      *	DIO initialization section
	  */
	
	/**
	 * IO Pins initialization by:
	 * PB0 -> Output
	 * PB1 -> Input
	 * PB2 -> Input
	 * PB3 -> Input
	 */
	DDRB = IO_PINS_DIR_INITIALIZATION;
	
	/*Enabling the pull up resistor for PB2*/
	PUEB = IO_PB2_PULLUP_ENABLE;
		
	/*Check the current state of the system to turn it OFF or ON*/
	if( gu8_systemStatus == SYSTEM_ON_STATUS )
	{
		/*If the system is already ON then set PB0 to +5v voltage level with activating PB2 pull up resistor*/
		PORTB = IO_PB0_HL_PB2_PUP_ACTIVE;
	} 
	else if( gu8_systemStatus == SYSTEM_OFF_STATUS )
	{
		/*If the system is already ON then set PB0 to 0v voltage level with activating PB2 pull up resistor*/
		PORTB = IO_PB0_LL_PB2_PUP_ACTIVE;
	}
	else
	{
		/*Report that the system is in OFF mode*/
		gu8_systemStatus = SYSTEM_OFF_STATUS;
	}

			
	/**
      *	Timer initialization section
	  */
	
	/*Selecting CTC mode with OCR0A*/
	TCCR0 = TIMER0_CTC_MODE_SELECTION;
	
	/*Clearing timer/counter register*/
	TCNT0 = 0;
	
	/*Adjusting TIMER0 to fire CTC interrupt every 50ms for 8MHz frequency and pre-scaler by 8*/
	OCR0A = TIMER0_50MS_TICK;
	
	/*Enable CTC mode interrupt*/
	TIMSK0 = TIMER0_OCR0A_INT_EN;
		
				
	/*Enable global interrupts*/
	SET_BIT(SREG , SREG_IBIT);
	
	return;
}

void mainApplication(void)
{
	/*Check if the switch over PB2 is pressed or not*/
	if( GET_BIT(PINB , PINB_PB2) == IO_LOW_LEVEL )
	{	
		/*If the switch is pressed for more than one second and the system is in OFF mode then go to ON mode*/	
		if( (gu16_switchCounter > ONE_SECOND && gu16_switchCounter < TWO_SECONDS) && (gu8_systemStatus == SYSTEM_OFF_STATUS) )
		{			
			/*Report that the system is in ON mode*/
			gu8_systemStatus = SYSTEM_ON_STATUS;

			/*Set the switch counter to two seconds count*/
			gu16_switchCounter = TWO_SECONDS;
			
			/*Activate PB0*/
			SET_BIT(PORTB , PORTB_PB0);
		}

		/*If the switch is pressed for more than one second and the system is in ON mode then go to OFF mode*/
		else if( ((gu16_switchCounter > ONE_SECOND && gu16_switchCounter < TWO_SECONDS) && (gu8_systemStatus == SYSTEM_ON_STATUS)) || (gu16_switchCounter >= TEN_SECONDS) )
		{
			/*Report that the system is in OFF mode*/
			gu8_systemStatus = SYSTEM_OFF_STATUS;
			
			/*De-activate PB0*/
			CLEAR_BIT(PORTB , PORTB_PB0);
	
			/*Disable the timer*/
			TCCR0 = 0;
		
			/*Select the idle mode*/
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			
			/*Sleep enable*/
			sleep_enable();
			
			/*Execute sleep instruction*/
			sleep_cpu();
		}
		
		/*If the switch counter is reset then enable the timer and increase the switch counter by 1*/
		else if( gu16_switchCounter == 0 )
		{
			/*Turn ON the timer to measure the switch pressing time*/
			TCCR0 |= TIMER0_PRESCALER_1;
			
			/*Increase the switch counter by 1*/
			gu16_switchCounter++;
		}
		
		/*If nothing happens then enter IDLE mode until the timer fires its interrupt*/
		else
		{
			/*Select the idle mode*/
			set_sleep_mode(SLEEP_MODE_IDLE);
	
			/*Sleep enable*/
			sleep_enable();
	
			/*Execute sleep instruction*/
			sleep_cpu();			
		}
	}
	else if( GET_BIT(PINB , PINB_PB2) == IO_HIGH_LEVEL )
	{
		/*Delay to make sure the bouncing has gone*/
		_delay_ms(50);
		
		/*Disable the timer*/
		TCCR0 = 0;
		
		/*Select the power down mode*/
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			
		/*Sleep enable*/
		sleep_enable();
			
		/*Execute sleep instruction*/
		sleep_cpu();
	}
	else
	{
		/*Do nothing*/
	}		
	
	return;
}

/************************************************************************/
/*							 System ISRs                                */
/************************************************************************/

/*ISR for OCR0A that happens when timer compare match happens*/
void OCR0A_ISR(void)
{
	/*Increase the switch counter by 1*/
	gu16_switchCounter++;
}