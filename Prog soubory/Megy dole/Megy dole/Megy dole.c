/*
 * Megy_dole.c
 *
 * Created: 26.1.2013 11:23:50
 *  Author: jirka1
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include "servo.h"

void main()
{
	servoStart();
	sei();    
	for(int i = 0; i < N_SERVOS; i++) 
		servoTime[i] = US2TIMER1(SERVO_MID);
	while(1)
    {
		uint16_t time;
		uint8_t w;      
		// Test update of servo 0
		for(time = 1000; time <= 2000; time += 50)
		{
			servoSet(0, time);
			for(w = 0; w < 25; w++)
			{
				_delay_ms(10);
			}
		}
	}
}



ISR(TIMER1_COMPA_vect)
{
	static uint16_t nextStart;
	static uint8_t servo;
	static bool outputHigh = true;
	uint16_t currentTime = OCR1A;
	uint8_t mask = servoOutMask[servo];
	
	if (outputHigh) {
		SERVO_PORT |= mask;
		// Set the end time for the servo pulse
		OCR1A = currentTime + servoTime[servo];
		nextStart = currentTime + US2TIMER1(SERVO_TIME_DIV);
	} else {
		SERVO_PORT &= ~mask;
		if (++servo == N_SERVOS) {
			servo = 0;
		}
		OCR1A = nextStart;
	}
	outputHigh = !outputHigh;
}