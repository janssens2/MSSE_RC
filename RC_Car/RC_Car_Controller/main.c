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

#include "headers/motion_control.h"
#include "headers/timers.h"
#include "headers/serial.h"

void send_motion_control_data();
void release_send_data_task();
void print_motion_control_info();
void release_print_task();

volatile bool g_release_send_data_task = false;
volatile bool g_release_print_task = false;

#ifdef DEBUG
extern serialMessage *g_serialMessage;
extern char tmpBuff[30];
#endif

serialCommand *g_serialCommand;

int main()
{
	clear();
	lcd_init_printf();
	
	motion_control_initialize();
	delay_ms(20);
	motion_control_home_inputs();
	
	init_serial_tx(&g_serialCommand);
	
	timer_two_set_to_ten_milliseconds(&release_send_data_task);
	timer_three_set_to_one_hundred_milliseconds(&release_print_task);

	while(true)
	{
		if (g_release_send_data_task)
		{
			g_release_send_data_task = false;
			send_motion_control_data();
		}
		
		if (g_release_print_task)
		{
			g_release_print_task = false;
			print_motion_control_info();
			//print_debug_info1();
		}
	}
}

void release_send_data_task()
{
	g_release_send_data_task = true;
}

void release_print_task()
{
	g_release_print_task = true;
}

void send_motion_control_data()
{
	/****************/
	g_serialCommand->x = motion_control_get_x_input_as_percentage();
	g_serialCommand->y = motion_control_get_y_input_as_percentage();
	g_serialCommand->c = motion_control_get_c_button_input();
	g_serialCommand->z = motion_control_get_z_button_input();
	serial_send_command(g_serialCommand);
	/****************/
}

void print_motion_control_info()
{
	clear();

	uint8_t input_method = motion_control_get_input_method();
	if (input_method == MOTION_CONTROL_JOYSTICK)
	{
		printf("[JS]  ACCEL C%dZ%d", motion_control_get_c_button_input(), motion_control_get_z_button_input());
	}
	else if (input_method == MOTION_CONTROL_ACCELEROMETER)
	{
		printf(" JS  [ACCEL]C%dZ%d", motion_control_get_c_button_input(), motion_control_get_z_button_input());
	}

	lcd_goto_xy(0, 1);

	//printf("X:%+4d Y:%+4d", motion_control_get_x_input_as_percentage(), motion_control_get_y_input_as_percentage());
	printf("X:%+4d Y:%+4d", motion_control_get_x_input(), motion_control_get_y_input());

	//set_m1_speed((motion_control_get_y_input_as_percentage() * 0xFF) / 100);
	

}

#ifdef DEBUG
void print_debug_info1()
{
	clear();
	lcd_goto_xy(0,0);
	//printf( "%s", (char *)g_serialMessage);
	printf( "%s", tmpBuff );
	//printf( "%+4d%+4d %1d%1d", g_serialCommand->x, g_serialCommand->y, g_serialCommand->c, g_serialCommand->z);
	//lcd_goto_xy(0,1);
	//printf( "%d", sizeof(serialMessage));
}
#endif
