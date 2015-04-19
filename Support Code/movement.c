/*
 * movement.c
 *
 * Created: 1/27/2015 12:51:19 PM
 *  Author: sethl
 */ 


#include "movement.h"
#include <avr/io.h>
#include "open_interface.h"


void moveBackward(oi_t* sensor, int centimeters){
	int millimeters = centimeters * -10;
	int sum = 0;
	oi_set_wheels(-200, -200); // move backwards
	
	while (sum > millimeters) {
		oi_update(sensor);
		sum += sensor->distance;
	}
	
	oi_set_wheels(0, 0); // stop
	oi_free(sensor);
}


void moveFoward(oi_t* sensor, int centimeters){
	int millimeters = centimeters * 10;
	int sum = 0;
	oi_set_wheels(200, 200); // move forward;
	
	while (sum < millimeters) {
		oi_update(sensor);
		sum += sensor->distance;
	}
	
	oi_set_wheels(0, 0); // stop
	oi_free(sensor);
}

void turnClockwise(oi_t* sensor, int degrees) {
	int sum = 12;
	if (degrees > 0) {
		oi_set_wheels(-200,200); // turn degrees;
		
		while (sum < degrees) {
			oi_update(sensor);
			sum -= sensor->angle;
		}
		
		oi_set_wheels(0, 0); // stop
		oi_free(sensor);
	}
	else {
		oi_set_wheels(200,-200); // turn degrees;
		while (sum > degrees) {
			oi_update(sensor);
			sum -= sensor->angle;
		}
		
		oi_set_wheels(0, 0); // stop
		oi_free(sensor);
	}
	
}