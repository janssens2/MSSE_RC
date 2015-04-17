/*
 * motor.c
 *
 * Created: 4/16/2015 9:10:47 PM
 *  Author: janssens
 */ 

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

void setupMotorPinChange( )
{
	// IO_D2 and IO_D3 are where the motor is connected for pin change interrupts
	// This equates to PCINT26 and PCINT27 which are labeled D2/D3 on the board
	PCMSK3 |= (1 << IO_D2) | (1 << IO_D3);  //0x0c
	PCMSK3 |= (1 << IO_D0) | (1 << IO_D1);  //0x03
	// PCIE3 shifted into the pin change interrupt control register
	// Enables pcint pins 31:24 which are set via the PCMSK3 register
	PCICR  |= (1 << PCIE3);					//0x08
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
	
	OCR2A = speed;
	
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
		}
		else
		{
			PORTC &= ~(1 << (PORTC7));
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
	
	OCR2B = speed;
	
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
		}
		else
		{
			PORTC &= ~(1 << (PORTC6));
		}
	}
	
	return currentTorque;
}