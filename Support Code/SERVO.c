/*
 * lab8.c
 *
 * Created: 3/23/2015 8:44:28 PM
 *  Author: sethl
 */ 


#include <avr/io.h>
#include "util.h"
#include "lcd.h"
unsigned pulse_interval = 40000;	//20 ms
unsigned mid_point = 3000;			//1.5ms
unsigned count = 800;
unsigned degree = 0;

// initializes timer 3 for servo use
void timer3_init(){
	OCR3A = pulse_interval - 1;		//number of cycles in the interval, setting the top
	OCR3B = mid_point - 1;			// move servo to the middle
	TCCR3A |= 0b00100011;			//set COM and WGM 
	TCCR3B |= 0b00011010;			//set WGM and CS
	TCCR3C = 0; //FOC Off
	DDRE |= _BV(4);
}

// moves the servo a set degree value
void move_servo(unsigned degree){
	unsigned pulse_width = (20*(unsigned long)degree)+950; // calculates pulse width
	OCR3B = pulse_width - 1;
	//wait_ms(600);//wait to force delay
}
/*
int main(void)
{
	lcd_init();
	timer3_init();
	int increment = 1; // switch to 0 makes it decrement, changes on button 4
	OCR3B = count;
	while(1) {
		switch(read_push_buttons()) {
			case 4:
			if(increment == 1)
			increment = 0;
			else
			increment = 1;
			break;
			case 3:
			if(increment == 1){
				count += 5;
				move_servo(degree+5);
				degree += 5;
			}
			else {
				count -= 5;
				move_servo(degree-5);
				degree -= 5;
			}
			break;
			case 2:
			if(increment == 1) {
				count += 2.5;
				move_servo(degree+2.5);
				degree += 2.5;
			}
			else {
				count -= 2.5;
				move_servo(degree-2.5);
				degree -= 2.5;
			}
			break;
			case 1:
			if(increment == 1) {
				count++;
				move_servo(degree+1);
				degree += 1;
			}
			else {
				count--;
				move_servo(degree-1);
				degree -= 1;
			}
			break;
		}
		if(degree <= 0){
			degree = 0;
		}
		if (degree>=180)
		{
			degree = 180;
		}
		lprintf("direction: %d,\n %d,\n degree %d",increment,count,degree);
	}
}
*/