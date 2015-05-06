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
#include "headers/nintendo_nunchuk.h"
#include "headers/timers.h"
#include "headers/serial.h"

void send_motion_control_data();
void release_send_data_task();
void print_motion_control_info();
void release_print_task();

volatile bool g_release_send_data_task = false;
volatile bool g_release_print_task = false;

int16_t count = 0;

serialCommand *g_serialCommand;

int main()
{
	clear();
	lcd_init_printf();
	
	
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
	g_serialCommand->x = motion_control_get_x_input();
	g_serialCommand->y = motion_control_get_y_input();
	g_serialCommand->c = nunchuck_get_button_c();
	g_serialCommand->z = nunchuck_get_button_z();
	serial_send_command(g_serialCommand);
	/****************/
	count++;
}

void print_motion_control_info()
{
	clear();

	uint8_t input_method = motion_control_get_input_method();
	if (input_method == MOTION_CONTROL_JOYSTICK)
	{
		//printf("[JS]  ACCEL");
		printf("count=%+4d", count);
	}
	else if (input_method == MOTION_CONTROL_ACCELEROMETER)
	{
		printf(" JS  [ACCEL]");
	}

	lcd_goto_xy(0, 1);

	//printf("X:%+4d Y:%+4d", motion_control_get_x_input_as_percentage(), motion_control_get_y_input_as_percentage());
	printf("X:%+4d Y:%+4d", motion_control_get_x_input(), motion_control_get_y_input());

	//set_m1_speed((motion_control_get_y_input_as_percentage() * 0xFF) / 100);
	

}
