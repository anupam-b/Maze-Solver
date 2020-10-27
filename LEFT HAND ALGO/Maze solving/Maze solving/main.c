/*
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

#define LED1 PE6  
#define LED2 PE7  

#define SW1 PINE0       
#define SW2 PINE1


#define SETBIT(ADDRESS, BIT)    (ADDRESS |= (1 << BIT))
#define CLEARBIT(ADDRESS, BIT)  (ADDRESS &= ~(1 << BIT))
#define CHECKBIT(ADDRESS, BIT)  (!(ADDRESS & (1 << BIT)))

char path[500];

int pathLength=0;

int Kp = 50;
int Ki = 0;
int Kd = 10;

int error = 0, P = 0, I = 0, D = 0, PID_value = 0;
int previous_error = 0, previous_I = 0;

int flag = 0;

int count;

void timer0_init()
{
	// set up timer with prescaler = 1024
	TCCR0B |= (1 << CS02)|(1 << CS00);
	// initialize counter select the bottom value
	TCNT0 = 0;
}

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
	DDRC |= (1<<OC3A)|(1<<OC3B);
}
void sensor(void)
{
	/*Sensor Array 	Error Value
		0 0 0 0 1		 4
		0 0 0 1 1		 3
		0 0 0 1 0		 2
		0 0 1 1 0		 1
		0 0 1 0 0		 0
		0 1 1 0 0		-1
		0 1 0 0 0		-2
		1 1 0 0 0		-3
		1 0 0 0 0		-4
		1 1 1 1 1		 103
		0 0 0 0 0		 102
		0 0 1 1 1        101
		1 1 1 0 0        100
		*/

		if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 0 )&&((CHECKBIT(PINB, PB2))== 0 )&&((CHECKBIT(PINB, PB3))== 0 )&&((CHECKBIT(PINB, PB4))== 1 ))        {error =  4;}
		else if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 0 )&&((CHECKBIT(PINB, PB2))== 0 )&&((CHECKBIT(PINB, PB3))== 1 )&&((CHECKBIT(PINB, PB4))== 1 ))   {error =  3;}
		else if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 0 )&&((CHECKBIT(PINB, PB2))== 0 )&&((CHECKBIT(PINB, PB3))== 1 )&&((CHECKBIT(PINB, PB4))== 0 ))   {error =  2;}
		else if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 0 )&&((CHECKBIT(PINB, PB2))== 1 )&&((CHECKBIT(PINB, PB3))== 1 )&&((CHECKBIT(PINB, PB4))== 0 ))   {error =  1;}
		else if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 0 )&&((CHECKBIT(PINB, PB2))== 1 )&&((CHECKBIT(PINB, PB3))== 0 )&&((CHECKBIT(PINB, PB4))== 0 ))   {error =  0;}
		else if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 1 )&&((CHECKBIT(PINB, PB2))== 1 )&&((CHECKBIT(PINB, PB3))== 0 )&&((CHECKBIT(PINB, PB4))== 0 ))   {error =- 1;}
		else if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 1 )&&((CHECKBIT(PINB, PB2))== 0 )&&((CHECKBIT(PINB, PB3))== 0 )&&((CHECKBIT(PINB, PB4))== 0 ))   {error = -2;}
		else if(((CHECKBIT(PINB, PB0))== 1 )&&((CHECKBIT(PINB, PB1))== 1 )&&((CHECKBIT(PINB, PB2))== 0 )&&((CHECKBIT(PINB, PB3))== 0 )&&((CHECKBIT(PINB, PB4))== 0 ))   {error = -3;}
		else if(((CHECKBIT(PINB, PB0))== 1 )&&((CHECKBIT(PINB, PB1))== 0 )&&((CHECKBIT(PINB, PB2))== 0 )&&((CHECKBIT(PINB, PB3))== 0 )&&((CHECKBIT(PINB, PB4))== 0 ))   {error = -4;}
		else if(((CHECKBIT(PINB, PB0))== 1 )&&((CHECKBIT(PINB, PB1))== 1 )&&((CHECKBIT(PINB, PB2))== 1 )&&((CHECKBIT(PINB, PB3))== 0 )&&((CHECKBIT(PINB, PB4))== 0 ))   {error = 100;} // Turn robot left side
		else if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 0 )&&((CHECKBIT(PINB, PB2))== 1 )&&((CHECKBIT(PINB, PB3))== 1 )&&((CHECKBIT(PINB, PB4))== 1 ))   {error = 101;} // Turn robot right side
		else if(((CHECKBIT(PINB, PB0))== 0 )&&((CHECKBIT(PINB, PB1))== 0 )&&((CHECKBIT(PINB, PB2))== 0 )&&((CHECKBIT(PINB, PB3))== 0 )&&((CHECKBIT(PINB, PB4))== 0 ))   {error = 102;} // Make U turn
		else if(((CHECKBIT(PINB, PB0))== 1 )&&((CHECKBIT(PINB, PB1))== 1 )&&((CHECKBIT(PINB, PB2))== 1 )&&((CHECKBIT(PINB, PB3))== 1 )&&((CHECKBIT(PINB, PB4))== 1 ))   {error = 103;} // Turn left side or stop
}

void fwd(void)
	{
		//MOVE FORWARD
		SETBIT(PORTC, M1_CW);
		CLEARBIT(PORTC, M1_CCW);
		SETBIT(PORTC, M2_CW);
		CLEARBIT(PORTC, M2_CCW);
	}

void left(void)
	{
		//LEFT TURN
		SETBIT(PORTC, M1_CW);
		CLEARBIT(PORTC, M1_CCW);
		CLEARBIT(PORTC, M2_CW);
		SETBIT(PORTC, M2_CCW);
	}

void right(void)
	{
		// RIGHT TURN
		CLEARBIT(PORTC, M1_CW);
		SETBIT(PORTC, M1_CCW);
		SETBIT(PORTC, M2_CW);
		CLEARBIT(PORTC, M2_CCW);
	}

void u_turn(void)
	{
		// U TURN
		CLEARBIT(PORTC, M1_CW);
		SETBIT(PORTC, M1_CCW);
		SETBIT(PORTC, M2_CW);
		CLEARBIT(PORTC, M2_CCW);
	}

void stop(void)
	{
		//STOP
		CLEARBIT(PORTC, M1_CW);
		CLEARBIT(PORTC, M1_CCW);
		CLEARBIT(PORTC, M2_CW);
		CLEARBIT(PORTC, M2_CCW);
	}
void extra_inch(void)
	{
		timer0_init();
			
		if(TCNT0 >= 125 )
		{
			count++;
			TCNT0 = 0;
		}
		if(count>=125)
		{
			//MOVE FORWARD
			SETBIT(PORTC, M1_CW);
			CLEARBIT(PORTC, M1_CCW);
			SETBIT(PORTC, M2_CW);
			CLEARBIT(PORTC, M2_CCW);
			TCNT0 = 0;         // Resetting the counter to Zero
			count = 0;
		}
			
	}
	
void simplifyPath(void);

void recIntersection(char direction)
{
	path[pathLength] = direction; // Store the intersection in the path variable.
	pathLength ++;
	simplifyPath(); // Simplify the learned path.
}

void simplifyPath(void)
{
	// only simplify the path if the second-to-last turn was a 'B'
	if(pathLength < 3 || path[pathLength-2] != 'B')
	return;

	int totalAngle = 0;
	int i;
	for(i=1;i<=3;i++)
	{
		switch(path[pathLength-i])
		{
			case 'R':
			totalAngle += 90;
			break;
			case 'L':
			totalAngle += 270;
			break;
			case 'B':
			totalAngle += 180;
			break;
		}
	}

	// Get the angle as a number between 0 and 360 degrees.
	totalAngle = totalAngle % 360;
  
	switch(totalAngle)
	{
		case 0:
		path[pathLength - 3] = 'S';
		break;
		case 90:
		path[pathLength - 3] = 'R';
		break;
		case 180:
		path[pathLength - 3] = 'B';
		break;
		case 270:
		path[pathLength - 3] = 'L';
		break;
	}

	// The path is now two steps shorter.
	pathLength -= 2;
	
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
	
	
    DDRC |= (1<<M1_CCW)|(1<<M1_CW)|(1<<M2_CCW)|(1<<M2_CW);
	DDRE |= (1<<DDE6)|(1<<DDE7);    
	PORTE |= (1<<PE0)|(1<<PE1);
   
    SETBIT(PORTC,OC3B);
    SETBIT(PORTC,OC3A);
	
	OCR3A = 0X0000;
	OCR3B = 0X0000;
	
	CLEARBIT(PORTE, LED1);
	CLEARBIT(PORTE, LED2);
	
    
    while (1) 
    {
		TCCR3A = (0<<COM3A0)|(0<<COM3A1)|(0<<COM3B0)|(0<<COM3B1);
	if ((CHECKBIT(PINE, SW1))){
		SETBIT(PORTE, LED1); 
		CLEARBIT(PORTE, LED2); 
		
		sensor();
		
		switch (error)
		{
			case 102:
					{do {
						u_turn();
						sensor();
				
							/*if (error == 0) {
								stop();
								delay(200);
							}*/
					} while (error != 0);
					recIntersection('B');}
					break;
			
			case 103:
					{extra_inch();
					sensor();
					if (error == 103) {		left();
											_delay_ms(200);
											do {
												sensor();
												left();
											} while (error != 0); 
											recIntersection('L');
										}
					else stop();}
					break;
			
			case 101:
				{	extra_inch();
					sensor();
					if (error == 102) {do {
											right();
											sensor();
												} while (error != 0);
												 recIntersection('R');
											}
					else recIntersection('S');}
					break;
			
			case 100:
					{do {
						left();
						sensor();
					} while (error != 0);
					recIntersection('L');}
					break;
			
			default:
					{calculate_pid();
					OCR3A = 0X8907+PID_value; 
					OCR3B = 0XB3FF-PID_value; //B3FF=46079=(65535*180)/256
					fwd();}
					break;
			
			}
		
		
		}
		
		if (!(CHECKBIT(PINE, SW2))){
		SETBIT(PORTE, LED2);     
		CLEARBIT(PORTE, LED1); 
		
		

		
		}
		
		
		
		
		
		
	}
}



