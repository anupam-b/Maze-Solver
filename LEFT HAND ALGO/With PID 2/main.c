/*
 * GccApplication2.c
 * Author : ANUPAM
 */ 
#include<avr/io.h>

#define	F_CPU	1000000

#include<util/delay.h>


#define OC3A   PC6 		//16 Bit PWM pin (OC3A)
#define M1_CW   PC0
#define M1_CCW  PC1

#define OC3B   PC5 		//16 Bit PWM pin (OC3B)
#define M2_CW  PC2
#define M2_CCW  PC3

#define SETBIT(ADDRESS, BIT)    (ADDRESS |= (1 << BIT))
#define CLEARBIT(ADDRESS, BIT)  (ADDRESS &= ~(1 << BIT))
#define CHECKBIT(ADDRESS, BIT)  (!(ADDRESS & (1 << BIT)))

float Kp = 25;
float Ki = 0;
float Kd = 0;

float error = 0, P = 0, I = 0, D = 0, PID_value = 0;
float previous_error = 0, previous_I = 0;

void calculate_pid(void)
{
	P = error;
	I = I + previous_I;
	D = error - previous_error;

	PID_value = (Kp * P) + (Ki * I) + (Kd * D);

	previous_I = I;
	previous_error = error;
}
void PWM_Init(void)
{
	ICR3 = 0xFFFF;
	TCCR3A |= (1<<WGM31)|(1<<COM3A1)|(1<<COM3B1);
	TCCR3B |= (1<<WGM33)|(1<<CS30);
	DDRC |= (1<<PC6)|(1<<PC5);
}

int main(void)
{
	DDRA &= ~(1<<PB0);
	DDRA &= ~(1<<PB1);
	DDRA &= ~(1<<PB2);
	DDRA &= ~(1<<PB3);
	DDRA &= ~(1<<PB4);
	
	PWM_Init();
	calculate_pid();
	
    DDRC |= (1<<M1_CCW)|(1<<M1_CW)|(1<<M2_CCW)|(1<<M2_CW)/*|(1<<OC3A)|(1<<OC3B)*/;
   
    SETBIT(PORTC,OC3B);
    SETBIT(PORTC,OC3A);
	
	OCR3A = 0X0000;
	OCR3B = 0X0000;
	
    
    while (1) 
    {
		TCCR3A = (0<<COM3A0)|(0<<COM3A1)|(0<<COM3B0)|(0<<COM3B1);
		
		if     (((CHECKBIT(PINB, PB0))== 1 )&&((CHECKBIT(PINB, PB1))== 1 )&&((CHECKBIT(PINB, PB2))== 1 )&&((CHECKBIT(PINB, PB3))== 1 )&&((CHECKBIT(PINB, PB4))== 1 ))   {error = 0;}
		else if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 0 )&&((CHECKBIT(PINB, PB2))== 0 )&&((CHECKBIT(PINB, PB3))== 0 )&&((CHECKBIT(PINB, PB4))== 0 ))	{error = 0;}
		else if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 0 )&&((CHECKBIT(PINB, PB2))== 0 )&&((CHECKBIT(PINB, PB3))== 0 )&&((CHECKBIT(PINB, PB4))== 1 ))   {error = 4;}
		else if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 0 )&&((CHECKBIT(PINB, PB2))== 0 )&&((CHECKBIT(PINB, PB3))== 1 )&&((CHECKBIT(PINB, PB4))== 1 ))   {error = 3;}
		else if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 0 )&&((CHECKBIT(PINB, PB2))== 0 )&&((CHECKBIT(PINB, PB3))== 1 )&&((CHECKBIT(PINB, PB4))== 0 ))   {error = 2;}
		else if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 0 )&&((CHECKBIT(PINB, PB2))== 1 )&&((CHECKBIT(PINB, PB3))== 1 )&&((CHECKBIT(PINB, PB4))== 0 ))   {error = 1;}
		else if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 0 )&&((CHECKBIT(PINB, PB2))== 1 )&&((CHECKBIT(PINB, PB3))== 0 )&&((CHECKBIT(PINB, PB4))== 0 ))   {error = 0;}
		else if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 1 )&&((CHECKBIT(PINB, PB2))== 1 )&&((CHECKBIT(PINB, PB3))== 0 )&&((CHECKBIT(PINB, PB4))== 0 ))   {error =- 1;}
		else if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 1 )&&((CHECKBIT(PINB, PB2))== 0 )&&((CHECKBIT(PINB, PB3))== 0 )&&((CHECKBIT(PINB, PB4))== 0 ))   {error = -2;}
		else if(((CHECKBIT(PINB, PB0))== 1 )&&((CHECKBIT(PINB, PB1))== 1 )&&((CHECKBIT(PINB, PB2))== 0 )&&((CHECKBIT(PINB, PB3))== 0 )&&((CHECKBIT(PINB, PB4))== 0 ))   {error = -3;}
		else if(((CHECKBIT(PINB, PB0))== 1 )&&((CHECKBIT(PINB, PB1))== 0 )&&((CHECKBIT(PINB, PB2))== 0 )&&((CHECKBIT(PINB, PB3))== 0 )&&((CHECKBIT(PINB, PB4))== 0 ))   {error = -4;}
			
		OCR3A = 0X1111/*+PID_value*/;  //B3FF=46079=(65535*180)/256
		OCR3B = 0XEEEE/*-PID_value*/;
		SETBIT(PORTC, M1_CW);
		CLEARBIT(PORTC, M1_CCW);
		SETBIT(PORTC, M2_CW);
		CLEARBIT(PORTC, M2_CCW);
			
		
		
	}
}



