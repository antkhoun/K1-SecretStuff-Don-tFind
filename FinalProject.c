/*
 * FinalProject.c
 *
 * Created: 4/12/2015 1:31:01 PM
 *  Author: sethl (Team?)
 */ 


#include <avr/io.h>
#include <stdio.h>

#include "Support Code/USART.h"
#include "Support Code/IR.h"
#include "Support Code/PING.h"
#include "Support Code/SERVO.h"
#include "Support Code/movement.h"
#include "Support Code/lab 9.h"

#include "Support Code/util.h"
#include "Support Code/lcd.h"
#include "Support Code/open_interface.h"

int main(void)
{
	// Initialize Serial Communication
	serial_init(57600);
	// initialize timers for use by ping sensor and servo
	timer3_init(); 
	timer_init();
	// initialize ADC for use by IR sensor
	ADC_init(); 
	// Initialize Open Interface and sensor data
	oi_t *sensor_data = oi_alloc();
	oi_init(sensor_data);
	// initialize LCD
	lcd_init();
	// initialize a string for printing sensor data
	char dataString[250];
	// prints a bunch of spaces to "clear" the screen
	for(int i =0; i < 75; i++) {
		serial_putc('\n');
	}
	serial_putc('\r');
	// print the headers
	serial_puts("| Bump L | Bump R | Cliff L  | Cliff LF | Cliff RF | Cliff R | Cliff SL | Cliff SLF | Cliff SRF | Cliff SR |\n\r");
	while(1){
		// update sensor values
		oi_update(sensor_data);
		// check if robot is on cliff edge
		if(sensor_data->cliff_frontleft == 1 || sensor_data->cliff_frontright == 1 || sensor_data->cliff_right == 1 || sensor_data->cliff_left == 1){
			//Print to Console that Cliff has been detected
			serial_puts("DANGER! CLIFF! Rerouting...                                                                                               \r");
			// Stop and Wait
			oi_set_wheels(0,0);
			wait_ms(50);
			// Move backwards 15 cm and wait
			moveBackward(sensor_data,15);
			wait_ms(100);
			oi_set_wheels(0,0);
			// begin scan to determine where to turn once we've seen the cliff
			int degreesToTurn = smallestObjectSweep();
			lprintf("Sweep Result: %d",degreesToTurn);
			if(degreesToTurn < 90) {
				turnClockwise(sensor_data, 90-degreesToTurn);
			}
			else {
				turnClockwise(sensor_data,-1*(degreesToTurn - 90));	
			}
			
		}
		// if not near a cliff, just keep going, and print all sensor data
		else{
			oi_set_wheels(100,100);
			// build string with given sensor data
			sprintf(dataString, "| %d      | %d      | %d        | %d        | %d        | %d       | %04d     | %04d      | %04d      | %04d     | \r", sensor_data->bumper_left, sensor_data->bumper_right, sensor_data->cliff_left,
			sensor_data->cliff_frontleft, sensor_data->cliff_frontright,sensor_data->cliff_right, sensor_data->cliff_left_signal, sensor_data->cliff_frontleft_signal, sensor_data->cliff_frontright_signal,
			sensor_data->cliff_right_signal);
			// print the built string
			serial_puts(dataString);
			wait_ms(100);
		}

	}
}