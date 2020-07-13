/**
 * @file    Functionality.h
 * @author  Ahmed Ashraf (ahmedashrafelnaqeeb@gmail.com)
 * @brief   This header file contains power manager interfacing functions' prototypes
 * @version 1.0
 * @date    2020-07-12
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __FUNCTIONALITY_H__
#define __FUNCTIONALITY_H__

/************************************************************************/
/*                     ISRs macros                                      */
/************************************************************************/

#define EXTI0_ISR  __vector_1
#define OCR0A_ISR  __vector_5

/************************************************************************/
/*                  Power manager functions' prototypes                 */
/************************************************************************/

/**
 * @brief This function is responsible for initializing the ATtiny MCU and activate the power down mode
 * 
 */
void attiny4_init(void);

/**
 * @brief This function is responsible for applying the state machine of the power manager system and making a transition
 *        from state to another  
 * 
 */
void mainApplication(void);

/************************************************************************/
/*                           ISRs prototypes                            */
/************************************************************************/

/*External interrupt0 ISR prototype*/
void EXTI0_ISR(void) __attribute__((signal));

/*OCR0A match compare ISR prototype*/
void OCR0A_ISR(void) __attribute__((signal));

#endif /*__FUNCTIONALITY_H__*/