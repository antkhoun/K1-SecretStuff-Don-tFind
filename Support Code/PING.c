/*
 * Lab_7.c
 *
 * Created: 3/3/2015 1:12:00 PM
 *  Author: sethl
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "util.h"
#include "lcd.h"

volatile enum{LOW, HIGH, DONE}state;
volatile unsigned rising_time;
volatile unsigned falling_time;
volatile unsigned pulse_width;

// initializes timer 1 for sensor use
void timer_init(void) {
	// set up timer 1: WGM1 bits = 0000, CS = 010, set OCR1A, set TIMSK
	TCCR1A = 0b00000000;		// WGM1[1:0]=00
	TCCR1B = 0b00000010;		// WGM1[3:2]=00, CS=010 prescaler to 8
}

//sensor isr
ISR(TIMER1_CAPT_vect){
	switch (state){
		case LOW:
			rising_time = ICR1; // read rising time
			state = HIGH;
			TCCR1B &= 0b10111111; // changes to expect falling edge
			break;
			
		case HIGH:
			falling_time = ICR1; // read falling time
			pulse_width = falling_time - rising_time; // in clock cycles
			state = DONE;
			break;
		case DONE:
			break;
	}
}

// sends a start up pulse to ping sensor
void send_pulse(){
	DDRD |= 0x10;		// PD4 as output
	PORTD |= 0x10;		// PD4 as high
	wait_ms(1);
	PORTD &= 0xEF;		// PD4 as low
	DDRD &= 0xEF;		// PD4 as input
}

// reads data from the ping sensor
unsigned ping_read(){
	TIMSK &= 0b1101111;			// disable IC interrupt
	send_pulse();
	state = LOW;				//set state to low
	TIFR &= 0b11011111;			// clear IC flag
	TIMSK |= 0b00100000;		// enable IC interrupt
	TCCR1B |= 0b01000000;		//Set to capture rising edge
	while(state != DONE);		 //wait until done
	return ((pulse_width*.0000005)*34300)/2; // calculate distance based on speed of sound
}

/*
int main(void){ 
	lcd_init(); // initialize lcd
	timer_init(); //initialize timer
	
    while(1){
	   lprintf("Pulse Width: %u\n Distance: %u",pulse_width,ping_read()); // print distance
	   wait_ms(500);
    }
}
*/
