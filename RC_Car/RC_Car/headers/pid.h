/*
 * pid.h
 *
 * Created: 4/16/2015 9:56:57 PM
 *  Author: janssens
 */ 


#ifndef PID_H_
#define PID_H_

#define MOTOR_NUM_COUNT_360 2249

enum motor_mode_t {
	MOTOR_MODE_SPEED,
	MOTOR_MODE_POSITION
};

typedef struct
{
	double dState;
	double iState;
	double iMax;
	double iMin;
	
	double iGain; // integral gain
	double pGain; // proportional gain
	double dGain; // derivative gain
	
	enum motor_mode_t myMode;
	int16_t targetRef;
	int16_t currentRef;
	int16_t currentTorque;
	
	int16_t (*setMyMotorSpeed)( int16_t );
} SPid;

void initPIDs( );
void setupPID( SPid *pid, enum motor_mode_t myMode, double iGain, double pGain, double dGain, double iMax, double iMin, int16_t (*setMyMotorSpeedFunc)(int16_t) );
int16_t updatePID( SPid *pid, int32_t error, int32_t position );
void getMotorPid( SPid **pid, uint8_t motorNum );

#endif /* PID_H_ */