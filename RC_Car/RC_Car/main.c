/*
 * main.c
 *
 * Created: 4/3/2015 10:21:24 AM
 * Author: Mike Weispfenning
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pololu/orangutan.h>

#include "headers/timers.h"
#include "headers/motor.h"
#include "headers/pid.h"
#include "headers/serial_USB.h"
#include "headers/serial.h"
#include "headers/horn.h"

#define NUNCHUCK_ZERO_GOOD 4
#define C_BUTTON_PRESS_CENTER 1000 // set to 50 which is 50 * 10ms or 500ms wait to take action

void release_pid_task();
void print_lcd_task();
void pid_worker( SPid *myPid );
void center_task( SPid *pid, SCenter *center );

volatile bool g_release_pid_task = false;
volatile uint16_t g_release_centering_task = false;

int main()
{
	uint16_t cButton = 0;
	
	clear();
	lcd_init_printf();
	serial_init();
	init_serial_rx();
	
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
	
	SCenter *myCentering;
	myCentering = malloc(sizeof(SCenter));
	memset( myCentering, 0, sizeof(SCenter));
	
	serialCommand *jesse_command;
	getSerialCommand( &jesse_command );

	while(true)
	{
		serial_check();
		check_for_new_bytes_received();
		serial_receive_bytes();

		if (g_release_pid_task)
		{
			g_release_pid_task = false;
			if ( jesse_command->c == 1 && g_release_centering_task < C_BUTTON_PRESS_CENTER ) // maybe only do this if held for 100ms? 10 times?
			{
				g_release_centering_task++;
			}
			else if ( jesse_command->c == 1 && g_release_centering_task == C_BUTTON_PRESS_CENTER )
			{
				m1->setMyMotorSpeed( 0 );
				m2->setMyMotorSpeed( 0 );
				g_release_centering_task++;
				memset( myCentering, 0x0, sizeof(SCenter) );
			}
			else if ( jesse_command->c == 0 && g_release_centering_task <= C_BUTTON_PRESS_CENTER )
			{
				g_release_centering_task = 0;
			}
			
			// running a center task OR pid task.  NOT BOTH!!!
			if ( g_release_centering_task > C_BUTTON_PRESS_CENTER )
			{
				// always run the centering task when requested
				// maybe we should count a 1s hold of the button to do this?
				center_task( m2, myCentering );
				if ( g_release_centering_task == 0 )
				{
					setPIDCenter( m2 );
					m2->targetRef = m2->centerRef;
					debug_print( DEBUG_ERROR, "FINAL: lftEnc=%d, rgtEnc=%d, cent=%d", m2->maxLeft, m2->maxRight, m2->centerRef );
				}
			}
			else
			{
				// horn
				//if ( jesse_command->c == 1 )
				//{
					//cButton++;
				//}
				//else if ( cButton > 0 ) // enter here if cButton is > 0 and real cButton is not pressed anymore
				//{
					//// cycle through horns here
					//if ( cButton % 3 == 0 )
						//horn_honk( HORN_VERSION_SHORT );
					//else if ( cButton % 3 == 1 )
						//horn_honk( HORN_VERSION_LONG );
					//else if ( cButton % 3 == 2 )
						//horn_honk( HORN_VERSION_FULL );
					//cButton = 0;
				//}
				
				// direction
				if ( abs(jesse_command->x) > NUNCHUCK_ZERO_GOOD )
				{
					setWheelDirection( m2, ((double) jesse_command->x/100) );
				}
				else
				{
					setWheelDirection( m2, 0.0 );
				}
				
				// speed
				if ( abs(jesse_command->y) > NUNCHUCK_ZERO_GOOD )
				{
					m1->targetRef = (int16_t) (((double) jesse_command->y/100) * 10000);
				}
				else
				{
					m1->targetRef = 0;
				}
				debug_print( DEBUG_IINFO, "command received: x: %d(%d), y:%d(%d)", jesse_command->x, m2->targetRef, jesse_command->y, m1->targetRef );
				pid_worker( m1 );
				pid_worker( m2 );
			}
		}
	}
	debug_print( DEBUG_ERROR, "SOMEHOW WE FELL OUT OF THE CYCLIC EXECUTIVE" );
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

void center_task( SPid *pid, SCenter *center )
{
	int32_t tmpCounts = 0;

	// find right side first
	if ( center->findRight == 0 )
	{
		center->encCount = getMotorEncoderCounts( pid->myMotorId );
		center->prvCount = 0;
		center->findRight = 1;
	}
	else if ( center->findRight == 1 )
	{
		tmpCounts = getMotorEncoderCounts( pid->myMotorId );
		debug_print( DEBUG_INFO, "(%d)(%d) tq=%d (tmp,prv) (%ld,%ld) >= dif=%ld spd=%ld", center->findLeft, center->findRight, pid->currentTorque, tmpCounts, center->prvCount, (tmpCounts - center->prvCount), pid->currentVelocity );

		if ( center->findRight == 1 && ( ( (tmpCounts - center->prvCount) > 3 ) || tmpCounts < (center->encCount + 500) ) )
		{
			pid->currentTorque = pid->setMyMotorSpeed( 25 );
			center->prvCount = tmpCounts;
		}
		else if ( pid->currentTorque != 0 )
		{
			pid->currentTorque = pid->setMyMotorSpeed( 0 );
			center->findRight = 2;
			pid->maxRight = (int16_t) getMotorEncoderCounts( pid->myMotorId );
		}
	}
	else if ( center->findRight >= 2  && center->findRight < 12)
	{
		// wait here for 100ms minimum
		center->findRight++;
	}
	else if ( center->findLeft == 0 )
	{
		center->encCount = getMotorEncoderCounts( pid->myMotorId );
		center->prvCount = 0;
		center->findLeft = 1;
	}
	else if ( center->findLeft == 1 )
	{
		tmpCounts = getMotorEncoderCounts( pid->myMotorId );
		debug_print( DEBUG_INFO, "(%d)(%d) tq=%d (tmp,prv) (%ld,%ld) >= dif=%ld spd=%ld", center->findLeft, center->findRight, pid->currentTorque, tmpCounts, center->prvCount, (tmpCounts - center->prvCount), pid->currentVelocity );

		if ( center->findLeft == 1 && ( ( (tmpCounts - center->prvCount) < -3 ) || tmpCounts > (center->encCount - 500) ) )
		{
			pid->currentTorque = pid->setMyMotorSpeed( -25 );
			center->prvCount = tmpCounts;
		}
		else if ( pid->currentTorque != 0 )
		{
			pid->currentTorque = pid->setMyMotorSpeed( 0 );
			center->findLeft = 2;
			pid->maxLeft = (int16_t) getMotorEncoderCounts( pid->myMotorId );
		}
	}
	else
	{
		g_release_centering_task = 0;
	}
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