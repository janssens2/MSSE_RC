/*
 * main.c
 *
 * Created: 4/3/2015 10:21:24 AM
 * Author: Mike Weispfenning
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <pololu/orangutan.h>

#include "headers/timers.h"
#include "headers/motor.h"
#include "headers/pid.h"
#include "headers/serial.h"

void release_pid_task();
void print_lcd_task();
void pid_worker( SPid *myPid );

volatile bool g_release_pid_task = false;

int main()
{
	clear();
	lcd_init_printf();
	serial_init();
	
	timer_one_set_to_ten_milliseconds( &release_pid_task );
	timer_two_set_to_fast_pwm( NULL, NULL );
	timer_three_set_to_one_hundred_milliseconds( &print_lcd_task );
	setupMotors();
	
	initPIDs();
	//delay_ms(1000);
	
	sei();
	
	// local pointers to global variables outside of our scope.
	SPid *m1;
	SPid *m2;
	getMotorPid( &m1, 1 );
	debug_print(DEBUG_VERBOSE, "M1-%d(%d) %p(%p)", m1->currentTorque, m1->targetRef, m1->setMyMotorSpeed, &setMyMotor1Speed);
	getMotorPid( &m2, 2 );
	debug_print(DEBUG_VERBOSE, "M2-%d(%d) %p(%p)", m2->currentTorque, m2->targetRef, m2->setMyMotorSpeed, &setMyMotor2Speed);
	
	while(true)
	{
		serial_check();
		check_for_new_bytes_received();

		if (g_release_pid_task)
		{
			g_release_pid_task = false;
			// do pid things here...
			//m1->currentTorque = m1->setMyMotorSpeed( m1->targetRef );
			//m2->currentTorque = m2->setMyMotorSpeed( m2->targetRef );
			pid_worker( m1 );
			pid_worker( m2 );
		}
	}
}

void pid_worker( SPid *myPid )
{
	int32_t myError;
	if ( myPid->myMode == MOTOR_MODE_SPEED )
	{
		myPid->currentRef = myPid->currentVelocity;
	}
	else if ( myPid->myMode == MOTOR_MODE_POSITION )
	{
		myPid->currentRef = getMotorEncoderCounts( myPid->myMotorId );
	}
	else
	{
		// whoops... really bad stuff happening here
		debug_print(DEBUG_ERROR, "pid_worker, should not be here");
	}
	
	myError = myPid->targetRef - myPid->currentRef;
	int16_t myNewSpeed = updatePID( myPid, myError, myPid->currentRef );
	myNewSpeed = ( myPid->myMode == MOTOR_MODE_SPEED ) ? myNewSpeed + myPid->currentTorque : myNewSpeed;
	myPid->currentTorque = myPid->setMyMotorSpeed( myNewSpeed );
	
	debug_print(DEBUG_INFO, "(%d) Pm=%.5ld T=%.3d Pr=%.5ld Kp=%.3f Ki=%.5f Kd=%.3f", myPid->myMotorId,
																				 myPid->currentRef % 100000,
																				 myPid->currentTorque % 1000,
																				 myPid->targetRef % 100000,
																				 myPid->pGain,
																				 myPid->iGain,
																				 myPid->dGain);
	debug_print(DEBUG_INFO, "(%d) New=%.3d encoder=%.9ld err=%.5ld spd=%.5ld", myPid->myMotorId, 
																		   myNewSpeed % 1000,
																		   getMotorEncoderCounts( myPid->myMotorId ) % 1000000000,
																		   myError,
																		   myPid->currentVelocity % 100000 );
}

void release_pid_task()
{
	g_release_pid_task = true;
	updateVelocity();
}

void print_lcd_task()
{
	clear();

	lcd_goto_xy(0, 0);
	printf( "M1 = %ld", getM1EncoderCounts() );
	//SPid *m1 = NULL;
	//getMotorPid( &m1, 1 );
	//printf( "M1fp = %p", m1->setMyMotorSpeed );
	

	lcd_goto_xy(0, 1);
	printf( "M2 = %ld", getM2EncoderCounts() );
	//printf( "M1f  = %p", &setMyMotor1Speed );
}