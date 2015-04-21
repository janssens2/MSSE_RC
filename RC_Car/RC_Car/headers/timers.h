/*
 * timers.h
 *
 * Created: 4/11/2015 10:58:46 AM
 *  Author: Mike Weispfenning
 */

#ifndef TIMERS_H_
#define TIMERS_H_

void timer_one_set_to_ten_milliseconds(void (*timer_one_interrupt_handler)());
void timer_two_set_to_fast_pwm( void (*timer_two_interrupt_handlerA)(), void (*timer_two_interrupt_handlerB)() );
void timer_three_set_to_one_hundred_milliseconds(void (*timer_three_interrupt_handler)());

#endif /* TIMERS_H_ */