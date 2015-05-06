/*
 * pid.c
 *
 * Created: 4/16/2015 9:56:45 PM
 *  Author: janssens
 */ 

#include <pololu/orangutan.h>
#include <stdlib.h>

#include "../headers/pid.h"
#include "../headers/motor.h"
#include "../headers/serial_USB.h"

// some globals related to PID stuff
SPid *gM1Pid;
SPid *gM2Pid;

void getMotorPid( SPid **pid, uint8_t motorNum )
{
	switch (motorNum)
	{
		case 1:
			*pid = gM1Pid;
			break;
		case 2:
			*pid = gM2Pid;
			break;
		default:
			// error case
			break;
	}
}

void initPIDs( )
{
	gM1Pid = malloc( sizeof(SPid) );
	gM2Pid = malloc( sizeof(SPid) );
	//setupPID( gM1Pid, MOTOR_MODE_SPEED, 1, 0.0001, 0.001, 0.001, 1000.0, -1000.0, &setMyMotor1Speed );
	setupPID( gM1Pid, MOTOR_MODE_SPEED, 1, 0.0001, 0.002, 0.01, 10000.0, -10000.0, &setMyMotor1Speed );
	debug_print(DEBUG_ERROR, "M1 fp=%p f=%p", gM1Pid->setMyMotorSpeed, &setMyMotor1Speed );
	setupPID( gM2Pid, MOTOR_MODE_POSITION, 2, 0.0001, 0.125, 0.1, 1000.0, -1000.0, &setMyMotor2Speed );
	debug_print(DEBUG_ERROR, "M2 fp=%p f=%p", gM2Pid->setMyMotorSpeed, &setMyMotor2Speed );
}

void setupPID( SPid *pid, enum motor_mode_t myMode, uint8_t motorId, double iGain, double pGain, double dGain, double iMax, double iMin, int16_t (*setMyMotorSpeedFunc)( int16_t ) )
{
	pid->myMode = myMode;
	pid->iGain  = iGain;
	pid->pGain  = pGain;
	pid->dGain  = dGain;
	pid->iMax   = iMax;
	pid->iMin   = iMin;
	pid->setMyMotorSpeed = setMyMotorSpeedFunc;
	pid->myMotorId = motorId;
	
	pid->targetRef = 0;
	pid->currentRef = 0;
	pid->currentTorque = 0;
	pid->iState = 0;
	pid->dState = 0;
	pid->currentVelocity = 0;
	pid->maxLeft = CENTER_UNSET;
	pid->maxRight = CENTER_UNSET;
	pid->isCenterSet = 0;
	pid->centerRef = CENTER_UNSET;
}

int16_t updatePID( SPid *pid, int32_t error, int32_t position )
{
	double pTerm = 0.0;
	double dTerm = 0.0;
	double iTerm = 0.0;
	
	debug_print( DEBUG_VERBOSE, "args err=%ld position=%ld iState=%.1f dState=%.1f", error, position, pid->iState, pid->dState );
	
	pTerm = pid->pGain * error; // calculate proportional term

	pid->iState += error; // calculate integral state
	if ( pid->iState > pid->iMax )
	{
		pid->iState = pid->iMax;
	}
	else if ( pid->iState < pid->iMin )
	{
		pid->iState = pid->iMin;
	}
	
	iTerm = pid->iGain * pid->iState; // calculate the integral term
	
	//if ( pid->dState != 0.0 )
	//{
	dTerm = pid->dGain * (pid->dState - position);
	//}
	pid->dState = position;
	
	debug_print( DEBUG_VERBOSE, "p=%.1f d=%.1f i=%.1f", pTerm, dTerm, iTerm);
	
	return (int16_t) (pTerm + dTerm + iTerm);
}

void updateVelocity( )
{
	gM1Pid->currentVelocity = getMyMotor1Velocity();
	gM2Pid->currentVelocity = getMyMotor2Velocity();
}

uint8_t setPIDCenter( SPid *pid )
{
	if ( pid->isCenterSet )
	{
		return 0;
	}

	if ( pid->maxLeft == CENTER_UNSET || pid->maxRight == CENTER_UNSET )
	{
		//debug_print( DEBUG_ERROR, "You have not yet set center, not going to try to turn" );
		return 1;
	}

	int16_t center = (abs(pid->maxLeft) + abs(pid->maxRight)) / 2;

	pid->centerRef = center + pid->maxLeft;
	pid->isCenterSet = 1;

	return 0;
}

void setWheelDirection( SPid *pid, double tgtPercent )
{
	uint8_t rc = setPIDCenter( pid );
	if ( rc ) return;
	
	// left
	if ( tgtPercent < 0.0 )
	{
		pid->targetRef = (int16_t) ((abs(pid->maxLeft) + (pid->centerRef)) * tgtPercent);
	}
	// right
	else if ( tgtPercent > 0.0 )
	{
		pid->targetRef = (int16_t) (((pid->maxRight) + abs(pid->centerRef)) * tgtPercent);
	}
	else
	{
		pid->targetRef = (int16_t) (pid->centerRef);
	}
}