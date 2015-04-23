/*
 * lab9.c
 *
 * Created: 3/31/2015 12:30:18 PM
 *  Author: sethl
 */ 


#include <avr/io.h>
#include "IR.h"
#include "PING.h"
#include "USART.h"
#include "SERVO.h"
#include "util.h"
#include "lcd.h"
#include "stdio.h"
#include "math.h"
#define PINGTHRESHOLD 20
#define IRTHRESHOLD 100
// global variables to keep track of the number of objects found,
// location of those objects, and their width.
int numberOfObjects;
int objectsLocations[10];
int objectWidths[10];
void smallestObjectSweep()
{
	// clear the objects arrays to allow for reuse
	for(int n = 0; n < 10; n++) {
		objectsLocations[n] = 0;
		objectWidths[n] = 0;
	}
	numberOfObjects = 0;
	int degrees = 0;
	int irDist = 0;
	int seesObject = 0;
	int objectStartDegreePosition = 0;
	int objectEndDegreePosition = 0;
	int totalDegrees = 0;
	int degreeArray[15];
	int objectsFound = 0;
	int middleDegreeArray[15];
	int degreeIncrementer = 0;
	int distancesPing[15];
	int currentDistance = 0;
	//set servo to zero and wait before data collection
	move_servo(0);
	wait_ms(1000);
	// start moving servo and print readings
	
	for(; degrees <= 180; degrees+=2){ 
		move_servo(degrees);
		// get the distance from the IR Sensor
		irDist = adcConvert(ADC_read()); 
		if(irDist <= 100 && irDist >= 10 && seesObject == 0 && degrees != 0){
			//object flag set true
			seesObject = 1;
			//grab starting degree position
			objectStartDegreePosition = degrees;
			//start the averaging of the distance
			currentDistance = (int) ping_read();
		}
		else if ((irDist > 100 || irDist < 10) && seesObject == 1){
			//object flag set false
			seesObject = 0;
			//grab starting degree position
			objectEndDegreePosition = degrees;
			// calculate object angular width
			totalDegrees = objectEndDegreePosition - objectStartDegreePosition;
			if(totalDegrees <= 0) {
				// do nothing because this does not matter.
				// this is here to deal with any noise, we know if the totalDegrees is less than 5 it didn't actually see anything.
			}
			
			else {
				degreeArray[degreeIncrementer] = totalDegrees;
				middleDegreeArray[degreeIncrementer] = (objectStartDegreePosition + objectEndDegreePosition) / 2;
				// calculate the average distance for the current object
				distancesPing[degreeIncrementer] = currentDistance / ((totalDegrees/2) + 1);
				degreeIncrementer++;
				if (degreeIncrementer > 15) {
					lprintf("ERROR");
					wait_ms(10000);
				}
				objectsFound++;
				objectEndDegreePosition = 0;
				objectStartDegreePosition = 0;
				totalDegrees = 0;
			}
		}
		else{
			currentDistance += (int) ping_read();
		}
	}

	// print the number of objects detected
	char prt[50];
	// set the number of objects that have been found
	numberOfObjects = degreeIncrementer;
	for(int z = 0; z < degreeIncrementer; z++) {
		objectsLocations[z] = middleDegreeArray[z];
		objectWidths[z] = 2*distancesPing[z]*tan(.5*degreeArray[z]*M_PI/180);
	}
	sprintf(prt, "  NUMBER OF OBJECTS: %d\n\r", degreeIncrementer);
	serial_puts(prt);
	
	// print the angle, distance, and width of the objects
	for(int i = 0; i < degreeIncrementer; i++){
		int width = 2*distancesPing[i]*tan(.5*degreeArray[i]*M_PI/180);
		objectWidths[i] = 
		sprintf(prt, "  Angle: %d   Distance: %d   Width: %d\n\r", 
			middleDegreeArray[i], distancesPing[i], width);
		serial_puts(prt);
	}
	
	// print that the sweep is done
	serial_puts("DONE SWEEPING!\n\r\n\r");
	
	//return finalPosition;
//	move_servo(finalPosition);
//	lprintf("Final Position: %d\nFinal OBJ: %d", finalPosition, ++j);
}