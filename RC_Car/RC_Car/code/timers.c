/*
 * timers.c
 *
 * Created: 4/11/2015 11:00:25 AM
 *  Author: Mike Weispfenning
 */

#include <pololu/orangutan.h>

void (*g_timer_one_interrupt_handler)();
void (*g_timer_three_interrupt_handler)();

void timer_one_set_to_ten_milliseconds(void (*timer_one_interrupt_handler)())
{
	g_timer_one_interrupt_handler = timer_one_interrupt_handler;

	TCCR1A = 0x80;
	TCCR1B = 0x1B;
	TCCR1C = 0x00;

	// Set ICR1 to 3,125
	ICR1 = 0x0C35;

	// Enable output compare A match interrupt
	TIMSK1 = 0x02;

	sei();
}

void timer_three_set_to_one_hundred_milliseconds(void (*timer_three_interrupt_handler)())
{
	g_timer_three_interrupt_handler = timer_three_interrupt_handler;

	TCCR3A = 0x80;
	TCCR3B = 0x1B;
	TCCR3C = 0x00;

	ICR3 = 0x7A12;

	TIMSK3 = 0x02;

	sei();
}

ISR (TIMER1_COMPA_vect)
{
	(*g_timer_one_interrupt_handler)();
}

ISR (TIMER3_COMPA_vect)
{
	(*g_timer_three_interrupt_handler)();
}