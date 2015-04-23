//

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
#include "Support Code/music.h"

#define GOAL_POST_WIDTH 5

#define WHITE_MIN 600
#define BLACK_MAX 190

// variables that keep track of the location of the array for the sensors
#define LEFT_BUMPER 0
#define RIGHT_BUMPER 1
#define CLIFF_LEFT 2
#define CLIFF_FRONT_LEFT 3
#define CLIFF_FRONT_RIGHT 4
#define CLIFF_RIGHT 5
#define CLIFF_LEFT_SIGNAL 6
#define CLIFF_FRONT_LEFT_SIGNAL 7
#define CLIFF_FRONT_RIGHT_SIGNAL 8
#define CLIFF_RIGHT_SIGNAL 9

void initialize(); // initialize the timers, etc.
void clearScreen(); // pseudo clear screen for putty
void printHeader(); // print the header
void printData(); // print the data under the header
void keyboardInput(char c); // takes in keyboard input and actions the iRobot
int errorDetection(); // detects if there's an obstacle the iRobot has come to
void printSensorStatus(int arr[]); // print out the sensors that are activated
void moveFowardUpdate(oi_t* sensor, int centimeters); //moves the iRobot forward
void turnClockwiseUpdate(oi_t* sensor, int degrees); // rotates the iRobot
void function_4_14(); // what we had in the while loop on 4/14/16 as a function
void turn_clockwise(oi_t *sensor, int degrees);
void turn_counter_clockwise(oi_t *sensor, int degrees);

// a string for printing sensor data
char dataString[250];
// Initialize Open Interface and sensor data
oi_t *sensor_data;
// array of the sensors
int sensorArray[10];
// to indicate whether the iRobot is in precision mode
int precision = 0;

int overBlackTape = 0;

int main(void) {
	initialize();

	clearScreen();

	// printHeader();
	int objectLocations[];
	while(overBlackTape == 0) {
		// if all bumpers are clear, move forward
		// do a scan to make sure I can move forward.
		smallestObjectSweep();
		if(seeGoalPost() == 1) {
			int i = 0;
			for(; i < numberOfObjects; i++) {
				if(objectWidths[i] < GOAL_POST_WIDTH) { // enter here if we see any goal post whatsoever.
					precision = 1; // we are getting close to the goal
					for(int j = (i+1); j < numberOfObjects; j++) { // check to see if can see another goal post
						if(objectWidths[j] < GOAL_POST_WIDTH) { // if we do we know we are just about to win. found 2nd post
							// grabs the middle angle between the two goal posts and that's where we will go.
							int middleOfTwoGoalPosts = (objectLocations[i] + objectLocations[j]) / 2;
							if(middleOfTwoGoalPosts < 90) {
								rotateClockWise(sensor_data, 90 - middleOfTwoGoalPosts);
							} else { 
								rotateCounterClockWise(sensor_data, middleOfTwoGoalPosts - 90);
							}
						} else { // can't find the second goal post
							if(objectLocations[i] > 90) { // turn and face directly at the small goal post
								rotateCounterClockWise(sensor_data, objectLocations[i] - 90);
							}
							else if(objectLocations[i] > 90) { // turn and face directly at the small goal post
								rotateClockWise(sensor_data, 90 - objectLocations[i]);
							}
							// return back to the very first start of the first while loop
							break; // not really sure how to do that.
						}
					}
				}
			}
		} else if(seeGoalPost() == 0 && numberOfObjects != 0) { // handle maneavuring the object detected

		}
		else {
			moveFowardUpdate(sensor_data,10);
		}
	}
}

int seeGoalPost() {
	for(int i = 0; i < numberOfObjects; i++) {
		if(objectWidths[i] < GOAL_POST_WIDTH)
			return 1;
	}
	return 0;

}