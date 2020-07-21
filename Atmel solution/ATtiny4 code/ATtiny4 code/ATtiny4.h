/**
 * @file    ATtiny4.h
 * @author  Ahmed Ashraf (ahmedashrafelnaqeeb@gmail.com)
 * @brief   This header file contains the important definitions for ATtiny4 MCU
 * @version 1.2
 * @date    2020-07-13
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef __MAIN_H__
#define __MAIN_H__

/************************************************************************/
/*           Typedefs to create project standard data types             */
/************************************************************************/

/*8bits std data type*/
typedef unsigned char u8_t;

/*16bits std data type*/
typedef unsigned short u16_t;

/************************************************************************/
/*                 ATtiny4 registers bit manipulation                   */
/************************************************************************/

#define SET_BIT(REG,BIT)		REG |= (1<<BIT)         /*Sets the bit value to 1*/
#define CLEAR_BIT(REG,BIT)		REG &= ~(1<<BIT)        /*Clears the bit value to 0*/
#define GET_BIT(REG,BIT)		((REG >> BIT) & 0x01)   /*Get the bit value*/

/************************************************************************/
/*                         ATtiny4 used registers                       */
/************************************************************************/

/*Status register*/
#define SREG   (*(volatile u8_t*)(0x3F))

/*DIO Registers*/
#define PUEB   (*(volatile u8_t*)(0x03))
#define PORTB  (*(volatile u8_t*)(0x02))
#define DDRB   (*(volatile u8_t*)(0x01))
#define PINB   (*(volatile u8_t*)(0x00))

/*Timer0 registers*/
#define TCCR0  (*(volatile u16_t*)(0x2D))
#define TCNT0  (*(volatile u16_t*)(0x28))
#define OCR0A  (*(volatile u16_t*)(0x26))
#define TIMSK0 (*(volatile u8_t*)(0x2B))

/*CLK control registers*/
#define CLKMSR (*(volatile u8_t*)(0x37))
#define CLKPSR (*(volatile u8_t*)(0x36))

/*Configuration Change Protection Register*/
#define CCP    (*(volatile u8_t*)(0x3C))

/************************************************************************/
/*               Important register bits                                */
/************************************************************************/

/*Interrupt bit*/
#define SREG_IBIT   (7)

/*PORTB register*/
#define PORTB_PB0	(0)
#define PORTB_PB1	(1)
#define PORTB_PB2	(2)
#define PORTB_PB3	(3)

/*DDRB register*/
#define DDRB_PB0	(0)
#define DDRB_PB1	(1)
#define DDRB_PB2	(2)
#define DDRB_PB3	(3)

/*PINB register*/
#define PINB_PB0	(0)
#define PINB_PB1	(1)
#define PINB_PB2	(2)
#define PINB_PB3	(3)

#endif /*__MAIN_H__*/