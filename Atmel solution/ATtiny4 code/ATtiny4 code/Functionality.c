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

#include "ATtiny4.h"
#include "Functionality.h"

#include "avr/sleep.h"
/************************************************************************/
/*					       Important macros                             */
/************************************************************************/

#define EXTI0_ENABLE					(0x01)
#define EXTI0_DISABLE					(0x00)
#define EXTI0_LOW_LEVEL_TRIGGER			(0x00)
#define EXTI0_FALLING_EDGE_TRIGGER		(0x02)
#define TIMER0_CTC_MODE_SELECTION		(0x0008)
#define TIMER0_50MS_TICK				(50000)
#define TIMER0_PRESCALER_8				(0x0002)
#define TIMER0_CLEAR_PRESCALER			(0xFFF8)
#define TIMER0_OCR0A_INT_EN				(0x02)
#define IO_PINS_DIR_INITIALIZATION      (0x01)
#define IO_LOW_LEVEL					(0)
#define IO_HIGH_LEVEL					(1)
#define IO_PB2_PULLUP_ENABLE            (0x04)
#define IO_PB2_PULLUP_ACTIVATE_PB0_LL   (0x04)
#define POWER_DOWN_MODE_SELECTION		(0x04)
#define SYSTEM_OFF_STATUS				(0)
#define SYSTEM_ON_STATUS				(1)
#define NO_VOLTAGE_PRESENT				(0)
#define NO_RESIDUAL_CHARGE 				(1)
#define VOLTAGE_CHECKING_TRIALS         (2)
#define TWO_SEC_DELAY                   (2000)
#define ONE_MS_DELAY                    (2000)
#define ONE_SECOND                      (20)
#define TWO_SECONDS                     (40)
#define THREE_SECONDS                   (60)
#define TEN_SECONDS                     (200)
#define INTERNAL_OSC_SELECT_8MZ         (0x00)
#define ENABLE_CHANGE_FOR_IO_REG        (0xD8)
#define MAIN_CLK_PRESCALING_BY_1        (0x00)
		
/************************************************************************/
/*                        Important system variables                    */
/************************************************************************/

/*Variable used in counting switch time being pressed*/
u16_t gu16_switchCounter = 0;

/*Variable used in voltage checking operation*/
u16_t gu16_checkCounter = 0;

/*Variable used in voltage checking trials*/
u8_t  gu8_voltageCheckTrials = 0;

/*Variable used to carry the system current status (ON or OFF)*/
u8_t  gu8_systemStatus = 0;

/************************************************************************/
/*                Power manager functions implementation                */
/************************************************************************/

void attiny4_init(void)
{
	/**
	  * Adjusting the MCU CLK section
	  */

	/*Disable global interrupts*/
	CLEAR_BIT(SREG , SREG_IBIT);
	
	/*Select the internal oscillator of the MCU with 8MHz*/
	CLKMSR = INTERNAL_OSC_SELECT_8MZ;
	
	/*Enable writing to the CLKPSR register*/
	CCP = ENABLE_CHANGE_FOR_IO_REG;
	
	/*Enable the pre-scaler of the main CLK by 1*/
	CLKPSR = MAIN_CLK_PRESCALING_BY_1;
	
			
	/**
	  * External interrupt initialization section
	  */
	
	/*Disable external interrupt0 (EXTI0)*/
	EIMSK = EXTI0_ENABLE;
	
	/*Selecting low level as interrupt trigger*/
	EICRA = EXTI0_LOW_LEVEL_TRIGGER;
	
	/*Clear EXTI0 flag*/
	SET_BIT(EIFR , EIFR_INTF0);	


	/**
      *	Timer initialization section
	  */
	
	/*Selecting CTC mode with OCR0A*/
	TCCR0 = TIMER0_CTC_MODE_SELECTION;
	
	/*Clearing timer/counter register*/
	TCNT0 = 0;
	
	/*Adjusting TIMER0 to fire CTC interrupt every 10ms for 8MHz frequency and prescaler by 8*/
	OCR0A = TIMER0_50MS_TICK;
	
	/*Enable CTC mode interrupt*/
	TIMSK0 = TIMER0_OCR0A_INT_EN;
		
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
	
	/*Activate the pull up resistor for PB2 and set PB0 voltage level to zero*/
	PORTB = IO_PB2_PULLUP_ACTIVATE_PB0_LL;
	
	/**
	 * Enabling all interrupts and activating power down mode
	 */
	
	/*Enable global interrupts*/
	SET_BIT(SREG , SREG_IBIT);

	/*Select the power down mode*/
	//SMCR = POWER_DOWN_MODE_SELECTION;
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	
	/*Sleep enable*/
	//SET_BIT(SMCR , SMCR_SE);
	sleep_enable();

	/*Execute sleep instruction*/
	//__asm__ __volatile__ ( "sleep" "\n\t" :: );	
	sleep_cpu();
	
	return;
}

void mainApplication(void)
{
    /*Applying the state machine of the system*/

    /*Checking if the switch pressed for more than 10 seconds*/
	if( gu16_switchCounter > TEN_SECONDS )
	{
		/*Variable used in delay operations*/
		u16_t au16_delayVariable = TWO_SEC_DELAY;
		
		/*Delay for two seconds*/
		while(au16_delayVariable--)
		{
			/*Variable used in for looping*/
			u16_t i = 0;
			
			/*Software delay for 1ms approximately*/
			for (i = 0 ; i < ONE_MS_DELAY ; i++);
		}
	
		/*Initialize the system again and enter power down mode*/
		attiny4_init();
	}

    /*Checking if the switch is pressed for (1~2) seconds and the system is already in the OFF state*/
	else if( (gu16_switchCounter >= ONE_SECOND && gu16_switchCounter <= TWO_SECONDS) && (gu8_systemStatus == SYSTEM_OFF_STATUS) )
	{
		/*Set PB0 to high level*/
		SET_BIT(PORTB , PORTB_PB0);
		
		/*Report that the system has become in ON mode*/
		gu8_systemStatus = SYSTEM_ON_STATUS;
		
		/*Reset the voltage checking counter*/
		gu16_checkCounter = 0;
		
		/*Reset voltage checking trials counter*/
		gu8_voltageCheckTrials = 0;
	}

    /*Checking if the switch is pressed for (1~2) seconds and the system is already in the ON state*/
	else if( (gu16_switchCounter >= ONE_SECOND && gu16_switchCounter <= TWO_SECONDS) && (gu8_systemStatus == SYSTEM_ON_STATUS) )
	{
		/*Report that the system is in OFF mode*/
		gu8_systemStatus = SYSTEM_OFF_STATUS;
		
		/*Initialize the system and enter power down mode*/
		attiny4_init();				
	}

    /*Checking after powering ON by 3 seconds that there's a voltage present or not and applying two powering up trials 
      if there's no voltage present*/
	else if( (gu16_checkCounter >= THREE_SECONDS) && (GET_BIT(PINB , PINB_PB1) == NO_VOLTAGE_PRESENT) && (gu8_voltageCheckTrials < VOLTAGE_CHECKING_TRIALS) )
	{	
		/*Variable used in delay operations*/
		u16_t au16_delayVariable = TWO_SEC_DELAY;
			 
		/*Disable all interrupts*/
		CLEAR_BIT(SREG , SREG_IBIT);
				
		/*Set PB0 to low level*/
		CLEAR_BIT(PORTB , PORTB_PB0);
		
		/*Delay for two seconds*/
		while(au16_delayVariable--)
		{
			/*Variable used in for looping*/
			u16_t i = 0;
			
			/*Software delay for 1ms approximately*/
			for (i = 0 ; i < ONE_MS_DELAY ; i++);
		}
			
		/*Set PB0 to high level*/
		SET_BIT(PORTB , PORTB_PB0);
		
		/*Reset the voltage checking counter*/
		gu16_checkCounter = 0;
		
		/*Increase voltage checking trials counter*/
		gu8_voltageCheckTrials++;
		
		/*Enable all interrupts*/
		SET_BIT(SREG , SREG_IBIT);
		
		/*Check if the system attempted two trials or not*/
		if(gu8_voltageCheckTrials == VOLTAGE_CHECKING_TRIALS)
		{
			/*If the trials reached two times initialize the system and enter power down mode*/
			attiny4_init();
		}
		else
		{
			/*Do nothing*/
		}	
	}
	
	/*Checking if the push button pressed accidentally for less than 1 second*/
	else if( (GET_BIT(PINB , PINB_PB2) == IO_HIGH_LEVEL) && (gu8_systemStatus == SYSTEM_OFF_STATUS) )
	{
		/*Initialize the system and enter power down mode*/
		attiny4_init();
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

/*ISR for EXTI0 that happens when pressing switch on PB2*/
void EXTI0_ISR(void)
{
	/*Reset the switch counter*/
	gu16_switchCounter = 0;

	/*Switching from OFF mode to ON mode*/
	if(EICRA == EXTI0_LOW_LEVEL_TRIGGER)
	{
		/*Convert EXTI0 mode into falling edge trigger*/
		EICRA = EXTI0_FALLING_EDGE_TRIGGER;
		
		/*Clear the timer counter*/
		TCNT0 = 0;

		/*Turn on the timer by pre-scaler 8*/
		TCCR0 &= TIMER0_CLEAR_PRESCALER;
		TCCR0 |= TIMER0_PRESCALER_8;
						
		/*Reset the voltage checking counter*/
		gu16_checkCounter = 0;

		/*Report that the system is in OFF mode*/
		gu8_systemStatus = SYSTEM_OFF_STATUS;
	}
	else if(EICRA == EXTI0_FALLING_EDGE_TRIGGER)
	{		
		/*Report that the system is in ON mode*/
		gu8_systemStatus = SYSTEM_ON_STATUS;
	}
}

/*ISR for OCR0A that happens when timer compare match happens*/
void OCR0A_ISR(void)
{
    /*Checking if the switch is pressed or not*/
	if( GET_BIT(PINB , PINB_PB2) == IO_LOW_LEVEL )
	{
        /*Increase the switch time counter if it's pressed*/
		gu16_switchCounter++;
	}

    /*Checking if there's no voltage present*/
	else if( GET_BIT(PINB , PINB_PB1) == NO_VOLTAGE_PRESENT )
	{
        /*Increase the voltage presence checking time counter*/
		gu16_checkCounter++;
	}
	else
	{
		/*Do nothing*/
	}
}