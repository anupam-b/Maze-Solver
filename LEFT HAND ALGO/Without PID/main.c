/*
 * GccApplication1.c
 *
 * Created: 13-06-2019 07:49:22 PM
 * Author : ANUPAM
 */ 

#include<avr/io.h>

#define	F_CPU	1000000

#include<util/delay.h>


/*#define M1_EN   PC6 		//16 Bit PWM pin (OC3A)
#define M1_CW   PC0
#define M1_CCW  PC1

#define M2_EN   PC5 		//16 Bit PWM pin (OC3B)
#define M2_CW   PC2
#define M2_CCW  PC3*/

#define SETBIT(ADDRESS, BIT)    (ADDRESS |= (1 << BIT))
#define CLEARBIT(ADDRESS, BIT)  (ADDRESS &= ~(1 << BIT))
#define CHECKBIT(ADDRESS, BIT)  (!(ADDRESS & (1 << BIT)))
int main(void)
{
	DDRA &= ~(1<<PB0);
	DDRA &= ~(1<<PB1);
	DDRA &= ~(1<<PB2);
	DDRA &= ~(1<<PB3);
	DDRA &= ~(1<<PB4);
    DDRC |= (1<<PC1)|(1<<PC0)|(1<<PC3)|(1<<PC2)|(1<<PC6)|(1<<PC5);
   
    SETBIT(PORTC,PC5);
    SETBIT(PORTC,PC6);
    
    while (1) 
    {
		TCCR3A = (0<<COM3A0)|(0<<COM3A1)|(0<<COM3B0)|(0<<COM3B1);
		if(!(PINB & (1<<PB2)))
		{
			//MOVE FORWARD
			SETBIT(PORTC, PC0);
			CLEARBIT(PORTC, PC1);
			SETBIT(PORTC, PC2);
			CLEARBIT(PORTC, PC3);
		} 
		if(!(PINB & (1<<PB0)) && !(PINB & (1<<PB1)) && !(PINB & (1<<PB2)) && !(PINB & (1<<PB3)) && !(PINB & (1<<PB4)))
		{
			// U TURN
			CLEARBIT(PORTC, PC0);
			SETBIT(PORTC, PC1);
			SETBIT(PORTC, PC2);
			CLEARBIT(PORTC, PC3);
		}
		if((PINB & (1<<PB0)) && (PINB & (1<<PB1)) && (PINB & (1<<PB2)) && (PINB & (1<<PB3)) && (PINB & (1<<PB4)))
		{
			//LINE
			CLEARBIT(PORTC, PC0);
			CLEARBIT(PORTC, PC1);
			SETBIT(PORTC, PC2);
			CLEARBIT(PORTC, PC3);
		}
		if(!(PINB & (1<<PB0)) && !(PINB & (1<<PB1)) && (PINB & (1<<PB2)) && (PINB & (1<<PB3)) && (PINB & (1<<PB4)))
		{
			// RIGHT TURN
			CLEARBIT(PORTC, PC0);
			SETBIT(PORTC, PC1);
			CLEARBIT(PORTC, PC2);
			CLEARBIT(PORTC, PC3);
		}
		
		
	}
}

