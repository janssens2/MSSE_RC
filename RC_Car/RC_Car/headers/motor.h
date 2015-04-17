/*
 * motor.h
 *
 * Created: 4/16/2015 9:12:19 PM
 *  Author: janssens
 */ 


#ifndef MOTOR_H_
#define MOTOR_H_

void setupMotor2( );
void setupMotor1( );
void setupMotorPinChange( );
int16_t setMyMotor2Speed( int16_t speed );
int16_t setMyMotor1Speed( int16_t speed );

#endif /* MOTOR_H_ */