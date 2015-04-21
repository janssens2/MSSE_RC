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
void setupMotor1PinChange( );
void setupMotor2PinChange( );
void enableMotorPinChangeInterrupt( );
int32_t getMotorEncoderCounts( int8_t motorNum );
int32_t getM1EncoderCounts( );
int32_t getM2EncoderCounts( );
int16_t setMyMotor2Speed( int16_t speed );
int16_t setMyMotor1Speed( int16_t speed );
void setupMotors( );

#endif /* MOTOR_H_ */