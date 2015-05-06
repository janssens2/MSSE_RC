/*
 * timers.c
 *
 * Created: 4/11/2015 11:00:25 AM
 *  Author: Mike Weispfenning
 */

#include <pololu/orangutan.h>
#include <stdlib.h>

void (*g_timer_one_interrupt_handler)();
void (*g_timer_two_interrupt_handlerA)();
void (*g_timer_two_interrupt_handlerB)();
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

void timer_two_set_to_ten_milliseconds(void (*timer_two_interrupt_handler)())
{
	g_timer_two_interrupt_handlerA = timer_two_interrupt_handler;

	// COM2A = 0b10 Clear 0C2A on Compare Match (table 17-2)
	// COM2B = 0b00 Normal port operation
	// WGM   = 0b010 Set CTC mode (table 17-8)
	// CS    = 0b101 Prescaler of 1024 (table 17-9)
	TCCR2A = (1 << COM2A1) | (0 << COM2A0) | (1 << WGM21) | (0 << WGM20);
	TCCR2B = (0 << WGM22) | (1 << CS22) | (0 << CS21) | (1 << CS20);

	// ~10ms timer top
	OCR2A = 0xC2;

	TIMSK2 = (1 << OCIE2A);

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

ISR (TIMER2_COMPA_vect)
{
	if ( g_timer_two_interrupt_handlerA != NULL )
	(*g_timer_two_interrupt_handlerA)();
}

ISR (TIMER2_COMPB_vect)
{
	if ( g_timer_two_interrupt_handlerB != NULL )
	(*g_timer_two_interrupt_handlerB)();
}

ISR (TIMER3_COMPA_vect)
{
	(*g_timer_three_interrupt_handler)();
}