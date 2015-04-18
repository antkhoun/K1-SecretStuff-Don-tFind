/*
 * FinalProject.c
 *
 * Created: 4/12/2015 1:31:01 PM
 *  Author: sethl (Team?)
 */ 


// README.txt - Anthony Khounlo
// This is my updated version of the code we worked on the other day. 
// I just made changes to the FinalProject.c, not to any other files. 
// I mainly put most of the code we had and put into function to make it cleaner. 
// I also add quite a bit more stuff to it such as manual movement and states. 
// WARNING: I have not tried this with the robot or anything so there's a great chance that this could be broken!!!

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

#define WHITE_MAX 100
#define WHITE_MIN 51
#define BLACK_MAX 50
#define BLACK_MIN 0

// declare an enum for the states of the iRobot, called state
typedef enum {L_BUMPER, R_BUMPER,L_CLIFF, FL_CLIFF, FR_CLIFF, R_CLIFF, 
	WHITE_TAPE, BLACK_TAPE, FREE_MOVEMENT, MULTIPLE_ERRORS} state;

void initialize(); // initialize the timers, etc.
void clearScreen(); // pseudo clear screen for putty
void printHeader(); // print the header
void printData(); // print the data under the header
void keyboardInput(char c); // takes in keyboard input and actions the iRobot
void updateState(state st);
int errorDetection(); // detects if there's an obstacle the iRobot has come to
void moveFowardUpdate(oi_t* sensor, int centimeters);
void function_4_14(); // what we had in the while loop on 4/14/16 as a function

// a string for printing sensor data
char dataString[250];
// Initialize Open Interface and sensor data
oi_t *sensor_data;
// the state of the iRobot
state iState = FREE_MOVEMENT;

/**
 *
 * blah blah blah
 *
 */
int main(void)
{
	// initialize all the timer and registers
	initialize();

	// clear screen on putty
	clearScreen();

	// print the headers
	printHeader();

	while(1){
		
		// move the iRobot with the putty/keyboard, wasd controls
		//keyboardInput(serial_getc());
		
		// error detection that will print to putty if one occurs
		//errorDetection();
		
		oi_update(sensor_data);
		printData();
		

	}
}

/**
 *
 * Function to initialize the timers, registers, etc. for the ir, ping, servo, lcd
 */
void initialize()
{
	// Initialize Serial Communication
	serial_init(57600);
	// initialize timers for use by ping sensor and servo
	timer3_init(); 
	timer_init();
	// initialize ADC for use by IR sensor
	ADC_init(); 
	// Initialize Open Interface and sensor data
	sensor_data = oi_alloc();
	oi_init(sensor_data);
	// initialize LCD
	lcd_init();
}

/**
 *
 * Function to clear the screen on putty
 */
void clearScreen()
{
	// prints a bunch of spaces to "clear" the screen
	for(int i =0; i < 75; i++) {
		serial_putc('\n');
	}
	serial_putc('\r');
}

/**
 *
 * Function to print heading for bumpers, cliff, and light detection
 */
void printHeader()
{
	// print the headers
	serial_puts("| Bump L | Bump R | Cliff L  | Cliff LF | Cliff RF | Cliff R | Cliff SL | Cliff SLF | Cliff SRF | Cliff SR |\n\r");
}

/**
 *
 * Function to print bumper, cliff, and light sensor data to putty 
 */
void printData()
{
	sprintf(dataString, "| %d      | %d      | %d        | %d        | %d        | %d       | %04d     | %04d      | %04d      | %04d     | \r",
		sensor_data->bumper_left, sensor_data->bumper_right, sensor_data->cliff_left, sensor_data->cliff_frontleft, sensor_data->cliff_frontright, 
		sensor_data->cliff_right, sensor_data->cliff_left_signal, sensor_data->cliff_frontleft_signal, sensor_data->cliff_frontright_signal,
		sensor_data->cliff_right_signal);

}

/**
 *
 * Function to control the iRobot depending on the character pressed, call it like keyboardInput(serial_getc());
 * @param c the character that would determine if the iRobot moves, uses serial_getc()
 */
void keyboardInput(char c)
{
	// move the iRobot forward, 10 cm
	if(c == 'w')
	{
		moveFowardUpdate(sensor_data, 10);
		wait_ms(50);
	}
	
	// move the iRobot backwards, 10 cm
	else if(c == 's')
	{
		moveBackward(sensor_data, 10);
		wait_ms(100);
	}

	// rotate the iRobot counter clockwise, 15 degrees
	else if(c == 'a')
	{
		turnClockwise(sensor_data, -15);
	}

	// rotate the iRobot clockwise, 15 degrees
	else if(c == 'd')
	{
		turnClockwise(sensor_data, 15);
	}

	// start sweeping for ir and sonar data
	else if(c == ' ')
	{
		//place holder for the sweeping
		int smallest = smallestObjectSweep();
		char prt[3];
		sprintf(prt, "%d", smallest);
		serial_puts(prt);
	}
	
	// emergency stop
	else if(c == '*')
	{
		oi_set_wheels(0, 0);
	}

}

/**
 *
 * Function that updates the state of the iRobot
 * @param st the state to update the iRobot to
 */
void updateState(state st)
{
	// if there's an update to an error state while there's already an error
	// update to MULTIPLE_ERRORS
	if(st != FREE_MOVEMENT && iState != FREE_MOVEMENT)
		iState = MULTIPLE_ERRORS;
	
	// else, switch to whatever is the desire state is
	else 
		iState = st;
	
	
}

/**
 *
 * Function that detects the error and updates the state of the iRobot
 * @return returns 1 if there's an error detected, else 0
 */
int errorDetection()
{
	
	// update the sensors
	oi_update(sensor_data);
	
	// if there's an error, set to 1
	int detection = 0;
	
	// header for errors
	//serial_puts("\n\rErrors Detected:\n\r");
	
	/// bumper errors ///
	
	// left bumper
	if(sensor_data->bumper_left == 1)
	{
		// update state
		updateState(L_BUMPER);
		// stop the iRobot
		oi_set_wheels(0, 0);
		// print out error to putty
		serial_puts("  LEFT BUMBER HIT!\n\r");
		// set detection to 1
		detection = 1;
	}
	
	// right bumper
	if(sensor_data->bumper_right == 1)
	{
		// update state
		updateState(R_BUMPER);
		// stop the iRobot
		oi_set_wheels(0, 0);
		// print out error to putty
		serial_puts("  RIGHT BUMBER HIT!\n\r");
		// set detection to 1
		detection = 1;
	}
	
	/// cliff errors ///
	
	// left cliff
	if(sensor_data->cliff_left == 1)
	{
		// update state
		updateState(L_CLIFF);
		// stop the iRobot
		oi_set_wheels(0, 0);
		// print out error to putty
		serial_puts("  LEFT CLIFF DETECTED!\n\r");
		// set detection to 1
		detection = 1;
	}
	
	// front left cliff
	if(sensor_data->cliff_frontleft == 1)
	{
		// update state
		updateState(FL_CLIFF);
		// stop the iRobot
		oi_set_wheels(0, 0);
		// print out error to putty
		serial_puts("  FRONT LEFT CLIFF DETECTED!\n\r");
		// set detection to 1
		detection = 1;
	}
	
	// front right cliff
	if(sensor_data->cliff_frontright == 1)
	{
		// update state
		updateState(FR_CLIFF);
		// stop the iRobot
		oi_set_wheels(0, 0);
		// print out error to putty
		serial_puts("  FRONT RIGHT CLIFF DETECTED!\n\r");
		// set detection to 1
		detection = 1;
	}
	
	// right cliff
	if(sensor_data->cliff_right == 1)
	{
		// update state
		updateState(R_CLIFF);
		// stop the iRobot
		oi_set_wheels(0, 0);
		// print out error to putty
		serial_puts("  RIGHT CLIFF DETECTED!\n\r");
		// set detection to 1
		detection = 1;
	}
	
	/// light errors, soon to be made ///
	
	
	// add a new line at the end
	//serial_puts("\n\r");
	
	if(detection != 0)
		serial_puts("\n\r");
	return detection;
}

/**
 *
 * Same function as moveForward but updates and check if there's an error
 *
 * @param sensor the desired sensor to read and update
 * @param centimeters the longest distance the iRobot will move
 */
void moveFowardUpdate(oi_t* sensor, int centimeters){
	int millimeters = centimeters * 10;
	int sum = 0;
	oi_set_wheels(150, 150); // move forward;
	
	while (sum < millimeters) {
		if(errorDetection() == 1)
		{
			break;
		}
		sum += sensor->distance;
	}
	
	oi_set_wheels(0, 0); // stop
	oi_free(sensor);
}

void moveBackwardUpdate(oi_t* sensor, int centimeters){
	int millimeters = centimeters * -10;
	int sum = 0;
	oi_set_wheels(-200, -200); // move backwards
	
	while (sum > millimeters) {
		if(errorDetection() == 1)
		{
			break;
		}
		sum += sensor->distance;
	}
	
	oi_set_wheels(0, 0); // stop
	oi_free(sensor);
}

/**
 *
 * Function that recreates what we have done in lab on 4/14/16
 *
 */
void function_4_14()
{
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
		printData();
		// print the built string
		serial_puts(dataString);
		wait_ms(100);
	}
}








