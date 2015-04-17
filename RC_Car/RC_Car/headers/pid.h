/*
 * pid.h
 *
 * Created: 4/16/2015 9:56:57 PM
 *  Author: janssens
 */ 


#ifndef PID_H_
#define PID_H_

#define MOTOR_NUM_COUNT_360 2249

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
	int32_t targetRef;
	int32_t currentRef;
	int16_t currentTorque;
} SPid;

#endif /* PID_H_ */