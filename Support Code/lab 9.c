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

//int main(void)
void smallestObjectSweep()
{
	// initialize lcd (clears screen)
	//lcd_init();
	// initialize timer for servo move
	//timer3_init(); 
	// initialize timer for ping sensor
	//timer_init();
	// initialize ADC for IR sensor 
	//ADC_init(); 
	// initialize Serial Communication
	//serial_init(57600); 
	
	int degrees = 0;
	int irDist = 0;
	//int sonarDist = 0;
	int seesObject = 0;
	int objectStartDegreePosition = 0;
	int objectEndDegreePosition = 0;
	int totalDegrees = 0;
	int degreeArray[15];
	//int lowestDegree;
	int objectsFound = 0;
	int middleDegreeArray[15];
	int degreeIncrementer = 0;
	//char dataLine[250];
	//char labelHeaders[]  = "Degrees       IR Distance (cm)       Sonar Distance (cm)"; //7 spaces between each
	
	int distancesPing[15];
	int currentDistance = 0;
	
	// print labels and get ready for data
	//serial_puts(labelHeaders);
	//serial_putc('\n');
	//serial_putc('\r');
	//set servo to zero and wait before data collection
	move_servo(0);
	wait_ms(1000);
	// start moving servo and print readings
	
	for(; degrees <= 180; degrees+=2){ 
		move_servo(degrees);
		// get the distance from the IR Sensor
		irDist = adcConvert(ADC_read()); 
		// get distance from the sonar
		//sonarDist = ping_read(); 
		//convert ints to string
		//sprintf(dataLine,"%d              %d                       %d",degrees,irDist,sonarDist);
		// print the data string and new line
		//serial_puts(dataLine); 
		//serial_putc('\n');
		//serial_putc('\r');
		
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
			if(totalDegrees < 5) {
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
		wait_ms(80); 
	}
	
// 	lowestDegree = degreeArray[0];
// 	for(int i = 0; i < degreeIncrementer; i++) {
// 		if(degreeArray[i] < lowestDegree)
// 			lowestDegree = degreeArray[i];
// 	}
// 	int j = 0;
// 	int finalPosition = 0;
// 	//lprintf("%d degreeIncrementer", degreeIncrementer);
// 	while(j < degreeIncrementer) {
// 		if(degreeArray[j] == lowestDegree) {
// 			finalPosition = middleDegreeArray[j];
// 			break;
// 		}
// 		j++;
// 	}

	// print the angle, distance, and width of the objects
	for(int i = 0; i < degreeIncrementer; i++){
		char prt[30];
		int width = 2*distancesPing[i]*tan(.5*degreeArray[i]*M_PI/180);
		sprintf(prt, "Angle: %03d   Distance: %03d   Width: %02d\n\r", 
			middleDegreeArray[i], distancesPing[i], width);
		serial_puts(prt);
	}
	
	//return finalPosition;
//	move_servo(finalPosition);
//	lprintf("Final Position: %d\nFinal OBJ: %d", finalPosition, ++j);
}
