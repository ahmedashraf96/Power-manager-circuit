/**
 * @file    Functionality.c
 * @author  Ahmed Ashraf (ahmedashrafelnaqeeb@gmail.com)
 * @brief   This file contains the interfacing functions logic implementation for the power manager application
 * @version 1.0
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
#define IO_PINS_DIR_INITIALIZATION      (0x08)
#define IO_LOW_LEVEL					(0)
#define IO_HIGH_LEVEL					(1)
#define POWER_DOWN_MODE_SELECTION		(0x04)
#define SYSTEM_OFF_STATUS				(0)
#define SYSTEM_ON_STATUS				(1)
#define NO_VOLTAGE_PRESENT				(0)
#define NO_RESIDUAL_CHARGE 				(1)
#define TWO_SEC_DELAY                   (2000)
#define ONE_MS_DELAY                    (2000)
#define ONE_SECOND                      (20)
#define TWO_SECONDS                     (40)
#define THREE_SECONDS                   (60)
#define TEN_SECONDS                     (200)

		
/************************************************************************/
/*                        Important system variables                    */
/************************************************************************/

/*Variable used to carry the system current status (ON or OFF)*/
u8_t  gu8_systemStatus = 0;

/*VAriable used in voltage checking trials*/
u8_t  gu8_voltageCheckTrials = 0;

/*Variable used in counting switch time being pressed*/
u16_t gu16_switchCounter = 0;

/*Variable used in voltage checking operation*/
u16_t gu16_checkCounter = 0;

/************************************************************************/
/*                Power manager functions implementation                */
/************************************************************************/

void attiny4_init(void)
{	
	/**
	  *External interrupt initialization section
	  */
	
	/*Disable external interrupt0 (EXTI0)*/
	EIMSK = EXTI0_ENABLE;
	
	/*Selecting low level as interrupt trigger*/
	EICRA = EXTI0_LOW_LEVEL_TRIGGER;
	
	/*Clear EXTI0 flag*/
	SET_BIT(EIFR , EIFR_INTF0);	

	/*Enable global interrupts*/
	SET_BIT(SREG , SREG_IBIT);

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
	 * PB0 -> Input
	 * PB1 -> Input
	 * PB2 -> Input
	 * PB3 -> Output
	 */
	DDRB = IO_PINS_DIR_INITIALIZATION;
	
	/*Set PB3 to logic zero*/
	CLEAR_BIT(PORTB , PORTB_PB3);

	/*Activate pull-up resistor for PB0*/
	SET_BIT(PORTB , PORTB_PB0);
	
	/*Activate pull-up resistor for PB2*/
	SET_BIT(PORTB , PORTB_PB2);	
	
	/**
	 * Activating power down mode
	 */
	
	/*Select the power down mode*/
	SMCR = POWER_DOWN_MODE_SELECTION;
	
	/*Sleep enable*/
	SET_BIT(SMCR , SMCR_SE);
	
	/*Execute sleep instruction*/
	__asm__ __volatile__ ( "sleep" "\n\t" :: );	
	
	return;
}

void mainApplication(void)
{
    /*Applying the state machine of the system*/

    /*Checking if there's a residual charge or not in the battery and if the switch pressed for more than 10 seconds*/
	if( (GET_BIT(PINB , PINB_PB0) == NO_RESIDUAL_CHARGE) || (gu16_switchCounter > TEN_SECONDS) )
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
		/*Set PB3 to high level*/
		SET_BIT(PORTB , PORTB_PB3);
		
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
		/*Set PB3 to low level*/
		CLEAR_BIT(PORTB , PORTB_PB3);

		/*Report that the system is in OFF mode*/
		gu8_systemStatus = SYSTEM_OFF_STATUS;		
	}

    /*Checking after powering ON by 3 seconds that there's a voltage present or not and applying two powering up trials 
      if there's no voltage present*/
	else if( (gu16_checkCounter == THREE_SECONDS) && (GET_BIT(PINB , PINB_PB1) == NO_VOLTAGE_PRESENT) && (gu8_voltageCheckTrials < 2) )
	{	
		/*Variable used in delay operations*/
		u16_t au16_delayVariable = TWO_SEC_DELAY;
			 
		/*Disable all interrupts*/
		CLEAR_BIT(SREG , SREG_IBIT);
				
		/*Set PB3 to low level*/
		CLEAR_BIT(PORTB , PORTB_PB3);
		
		/*Delay for two seconds*/
		while(au16_delayVariable--)
		{
			/*Variable used in for looping*/
			u16_t i = 0;
			
			/*Software delay for 1ms approximately*/
			for (i = 0 ; i < ONE_MS_DELAY ; i++);
		}
			
		/*Set PB3 to high level*/
		SET_BIT(PORTB , PORTB_PB3);
		
		/*Reset the voltage checking counter*/
		gu16_checkCounter = 0;
		
		/*Increase voltage checking trials counter*/
		gu8_voltageCheckTrials++;
		
		/*Enable all interrupts*/
		SET_BIT(SREG , SREG_IBIT);
	}

    /*Any other state happens the system will initialize and power down*/
	else
	{
		/*Initialize the system again and enter power down mode*/
		attiny4_init();
	}

	return;
}

/************************************************************************/
/*							 System ISRs                                */
/************************************************************************/

void EXTI0_ISR(void)
{
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
				
		/*Reset the switch counter*/
		gu16_switchCounter = 0;
		
		/*Reset the voltage checking counter*/
		gu16_checkCounter = 0;

		/*Report the system is in OFF mode*/
		gu8_systemStatus = SYSTEM_OFF_STATUS;
	}
	else if(EICRA == EXTI0_FALLING_EDGE_TRIGGER)
	{
		/*Reset the switch counter*/
		gu16_switchCounter = 0;
		
		/*Report the system is in ON mode*/
		gu8_systemStatus = SYSTEM_ON_STATUS;
	}
}

void OCR0A_ISR(void)
{
    /*Checking if the switch is pressed or not*/
	if(GET_BIT(PINB , PINB_PB2) == IO_LOW_LEVEL)
	{
        /*Increase the switch time counter if it's pressed*/
		gu16_switchCounter++;
	}

    /*Checking if the switch is released and there's no voltage present*/
	else if( (GET_BIT(PINB , PINB_PB2) == IO_HIGH_LEVEL) && (GET_BIT(PINB , PINB_PB1) == NO_VOLTAGE_PRESENT) )
	{
        /*Increase the voltage presence checking time counter*/
		gu16_checkCounter++;
	}
	else
	{
		/*Do nothing*/
	}
}