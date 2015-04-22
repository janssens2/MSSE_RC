/*
 * motor.c
 *
 * Created: 4/16/2015 9:10:47 PM
 *  Author: janssens
 */ 

#include <pololu/orangutan.h>
#include <stdlib.h>

#include "../headers/motor.h"
#include "../headers/serial.h"

// some global vars to keep track of the motor stuff
volatile int32_t gM1EncoderCounts	= 0;
volatile int32_t gM2EncoderCounts	= 0;
volatile int32_t gM1PrvEncoderCounts= 0;
volatile int32_t gM2PrvEncoderCounts= 0;
volatile int32_t gM1Speed[SPEED_COUNT_SIZE] = {0};
volatile int32_t gM2Speed[SPEED_COUNT_SIZE] = {0};
volatile uint8_t gM1SpeedIdx        = 0;
volatile uint8_t gM2SpeedIdx        = 0;
volatile unsigned char gLastM1A_val = 0;
volatile unsigned char gLastM1B_val = 0;
volatile unsigned char gLastM2A_val = 0;
volatile unsigned char gLastM2B_val = 0;
volatile unsigned char gErrorM1		= 0;
volatile unsigned char gErrorM2		= 0;

int32_t getMotorEncoderCounts( int8_t motorNum )
{
	switch ( motorNum )
	{
		case 1:
			return gM1EncoderCounts;
			break;
		case 2:
			return gM2EncoderCounts;
			break;
		default:
			// error condition
			return 0;
			break;
	}
}

int32_t getM1EncoderCounts( )
{
	return getMotorEncoderCounts( 1 );
}

int32_t getM2EncoderCounts( )
{
	return getMotorEncoderCounts( 2 );
}

void setupMotor2( )
{
	// set port to 0 for both PWM
	PORTD &= ~(1 << (PORTD6));
	//PORTD &= ~(1 << (PORTD7));
	
	PORTC &= ~(1 << (PORTC6));
	//PORTC &= ~(1 << (PORTC7));
	
	// set data direction to 0 for both PWM
	DDRD |= (1 << DDD6);
	//DDRD |= (1 << DDD7);
	
	DDRC |= (1 << DDC6);
	//DDRC |= (1 << DDC7);
	
	setMyMotor1Speed( 0 );
}

void setupMotor1( )
{
	// set port to 0 for both PWM
	//PORTD &= ~(1 << (PORTD6));
	PORTD &= ~(1 << (PORTD7));
	
	//PORTC &= ~(1 << (PORTC6));
	PORTC &= ~(1 << (PORTC7));
	
	// set data direction to 0 for both PWM
	//DDRD |= (1 << DDD6);
	DDRD |= (1 << DDD7);
	
	//DDRC |= (1 << DDC6);
	DDRC |= (1 << DDC7);
	
	setMyMotor2Speed( 0 );
}

void setupMotor1PinChange( )
{
	// IO_A0 and IO_A1 are where the motor is connected for pin change interrupts
	// This equates to PCINT00 and PCINT01 which are labeled A0/A1 on the board
	PCMSK0 |= (1 << (31 - IO_A0)) | (1 << (31 - IO_A1)); //0x03
}

void setupMotor2PinChange( )
{
	// IO_A2 and IO_A3 are where the motor is connected for pin change interrupts
	// This equates to PCINT02 and PCINT03 which are labeled A2/A3 on the board
	PCMSK0 |= (1 << (31 - IO_A2)) | (1 << (31 - IO_A3)); // 0x0C
}


void enableMotorPinChangeInterrupt( )
{
	PCICR |= (1 << PCIE0);
}

void setupMotors( )
{
	setupMotor1();
	setupMotor2();
	setupMotor1PinChange();
	setupMotor2PinChange();
	enableMotorPinChangeInterrupt();
}

int16_t setMyMotor1Speed( int16_t speed )
{
	int16_t currentTorque;
	
	// determine if we are in reverse mode or not
	uint8_t reverse = ( speed >= 0 ) ? 0 : 1;
	
	// get a positive value of speed
	speed = abs( speed );
	
	// limit the max of speed to 0xFF
	speed = ( speed > 0xFF ) ? 0xFF : speed;

	currentTorque = (reverse) ? speed * -1 : speed;
	
	debug_print(DEBUG_VERBOSE, "setting M1 speed to (%d)%d", reverse, speed);
	
	if ( speed == 0 )
	{
		// 0% duty cycle
		TCCR2A &= ~( 1 << COM2A1 );
	}
	else
	{
		TCCR2A |= ( 1 << COM2A1 );
		
		if ( reverse )
		{
			PORTC |= (1 << (PORTC7));
			OCR2A = speed;
		}
		else
		{
			PORTC &= ~(1 << (PORTC7));
			OCR2A = speed;
		}
	}
	
	return currentTorque;
}

int16_t setMyMotor2Speed( int16_t speed )
{
	int16_t currentTorque;
	
	// determine if we are in reverse mode or not
	uint8_t reverse = ( speed >= 0 ) ? 0 : 1;
	
	// get a positive value of speed
	speed = abs( speed );
	
	// limit the max of speed to 0xFF
	speed = ( speed > 0xFF ) ? 0xFF : speed;

	currentTorque = (reverse) ? speed * -1 : speed;

	debug_print(DEBUG_VERBOSE, "setting M2 speed to (%d)%d", reverse, speed);
	
	if ( speed == 0 )
	{
		// 0% duty cycle
		TCCR2A &= ~( 1 << COM2B1 );
	}
	else
	{
		TCCR2A |= ( 1 << COM2B1 );
		
		if ( reverse )
		{
			PORTC |= (1 << (PORTC6));
			OCR2B = speed;
		}
		else
		{
			PORTC &= ~(1 << (PORTC6));
			OCR2B = speed;
		}
	}
	
	return currentTorque;
}

int32_t getMyMotor1Velocity( )
{
	int32_t tmpEncoderCounts = gM1EncoderCounts;
	if ( gM1SpeedIdx >= SPEED_COUNT_SIZE )
	{
		gM1SpeedIdx = 0;
	}
	gM1Speed[gM1SpeedIdx] = (tmpEncoderCounts - gM1PrvEncoderCounts);
	
	gM1SpeedIdx++;
	
	gM1PrvEncoderCounts = tmpEncoderCounts;
	
	double tmpAverageSpeed = 0.0;
	for ( uint8_t idx=0; idx < SPEED_COUNT_SIZE; idx++ )
	{
		tmpAverageSpeed += gM1Speed[idx];
	}
	tmpAverageSpeed = (tmpAverageSpeed / SPEED_COUNT_SIZE) * 100;
	
	return (int32_t) tmpAverageSpeed;
}

int32_t getMyMotor2Velocity( )
{
	int32_t tmpEncoderCounts = gM2EncoderCounts;
	if ( gM2SpeedIdx >= SPEED_COUNT_SIZE )
	{
		gM2SpeedIdx = 0;
	}
	gM2Speed[gM2SpeedIdx] = (tmpEncoderCounts - gM2PrvEncoderCounts);
	
	gM2SpeedIdx++;
	
	gM2PrvEncoderCounts = tmpEncoderCounts;
	
	double tmpAverageSpeed = 0.0;
	for ( uint8_t idx=0; idx < SPEED_COUNT_SIZE; idx++ )
	{
		tmpAverageSpeed += gM2Speed[idx];
	}
	tmpAverageSpeed = (tmpAverageSpeed / SPEED_COUNT_SIZE) * 100;
	
	return (int32_t) tmpAverageSpeed;
}

ISR(PCINT0_vect)
{
	// ISR method to calculate the number of encoder counts based
	// on direction
	
	// a 1 or 0 allows us to get to 48 encoder checks for a single
	// revolution of the encoder magnet.
	unsigned char m1a_val = (PINA >> (31 - IO_A0)) & 0x1;
	unsigned char m2a_val = (PINA >> (31 - IO_A2)) & 0x1;
	
	unsigned char m1b_val = (PINA >> (31 - IO_A1)) & 0x1;
	unsigned char m2b_val = (PINA >> (31 - IO_A3)) & 0x1;

	// determine if we need to plus or minus based on current value
	// and the last value of the other encoder
	unsigned char plus_m1 = m1b_val ^ gLastM1A_val;
	unsigned char minus_m1 = m1a_val ^ gLastM1B_val;
	
	unsigned char plus_m2 = m2b_val ^ gLastM2A_val;
	unsigned char minus_m2 = m2a_val ^ gLastM2B_val;
	
	if ( plus_m1 )
	{
		gM1EncoderCounts += 1;
		
	}
	if ( minus_m1 )
	{
		gM1EncoderCounts -= 1;
	}
	
	if ( plus_m2 )
	{
		gM2EncoderCounts += 1;
	}
	if ( minus_m2 )
	{
		gM2EncoderCounts -= 1;
	}
	
	if ( m1a_val != gLastM1A_val && m1b_val != gLastM1B_val )
	{
		gErrorM1 = 1;
	}
	
	if ( m2a_val != gLastM2A_val && m2b_val != gLastM2B_val )
	{
		gErrorM2 = 1;
	}
	
	gLastM1A_val = m1a_val;
	gLastM1B_val = m1b_val;
	
	gLastM2A_val = m2a_val;
	gLastM2B_val = m2b_val;
}