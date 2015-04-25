/*
 * FinalProject.c
 *
 * Created: 4/12/2015 1:31:01 PM
 *  Author: Seth, David, Caleb, Anthony (Team?)
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
#include "Support Code/music.h"
 
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
void start(); // keep the iRobot in idle mode until ready to start
void finish(); // set the iRobot into victory mode!
void courseMode(); // put the iRobot in course/manual mode
void clearScreen(); // pseudo clear screen for putty
void printHeader(); // print the header
void printData(); // print the data under the header
void keyboardInput(char c); // takes in keyboard input and actions the iRobot
int errorDetection(int isRotating); // detects if there's an obstacle the iRobot has come to
void printSensorStatus(int arr[]); // print out the sensors that are activated
void moveFowardUpdate(oi_t* sensor, int centimeters); //moves the iRobot forward
void turnClockwiseUpdate(oi_t* sensor, int degrees); // rotates the iRobot
void turn_clockwise(oi_t *sensor, int degrees); // turn iRobot clockwise
void turn_counter_clockwise(oi_t *sensor, int degrees); //turn iRobot counter clockwise

// a string for printing sensor data
char dataString[250];
// Initialize Open Interface and sensor data
oi_t *sensor_data;
// array of the sensors
int sensorArray[10];
// to indicate whether the iRobot is in precision mode
int precision = 0;
// variable to indicate when the course is finished
int isFinished = 0;

/**
 *
 * The main() function to control the iRobot through the course
 *
 */
int main(void)
{
	// initialize all the timer and registers
	initialize();

	// clear screen on putty
	clearScreen();

	// idle mode until we start
	start();

	// mode that will allow us to control the iRobot 
	courseMode();
	
	// victory!
	finish();
	
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
	load_songs();
	
}

/**
 *
 * Function to let the robot be in idle until we start
 */
void start()
{
	// stay in while loop until ready when 1 is entered
	while(1)
	{
		if(serial_getc() == '1')
			break;
	}
	
	// play introduction song (Lost Woods)
	oi_play_song(3);
	serial_puts("START!\n\r");
	serial_puts("------------------------------------------------------\n\r");
}

/**
 *
 * Function to stop the robot when finished with the course
 */
void finish()
{
	// blink led power light 3 times
	for(int i = 0; i < 3; i++){
		oi_set_leds(1, 0, 128, 255);
		wait_ms(500);
		oi_set_leds(0, 1, 128, 255);
		wait_ms(500);
	}
	
	// play victory song (Final Fantasy)
	oi_play_song(2);
	
	// print to putty victory message
	serial_puts("\n\rSWEET, SWEET, SWEET VICTORY\n\r");
	
}

/**
 *
 * Function to put the iRobot into manual control in putty
 * will be used to control the iRobot through the course
 */
void courseMode()
{
	// stay in this loop until course is finished
	while(isFinished == 0)
	{
		// move the iRobot with the putty/keyboard, wasd controls
		keyboardInput(serial_getc());
		// separate each command
		serial_puts("------------------------------------------------------\n\r");
	}
	
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
	serial_puts(dataString);

}

/**
 *
 * Function to control the iRobot depending on the character pressed, call it like keyboardInput(serial_getc());
 * @param c the character that would determine if the iRobot moves, uses serial_getc()
 */
void keyboardInput(char c)
{
	// toggle precision mode, if activated, move is 5 cm and 5 degrees
	if(c == 'T')
	{
		if(precision == 0){
			serial_puts("PRECISION ACTIVATED\n\r\n\r");
			precision = 1;
		}
		else{
			serial_puts("PRECISION DEACTIVATED\n\r\n\r");
			precision = 0;
		}
	}
	
	// move the iRobot forward, 5 or 10 cm
	else if(c == 'W')
	{
		if(precision)
		{
			serial_puts("MOVING FORWARD 5 CM\n\r\n\r");
			moveFowardUpdate(sensor_data, 5);
		}
		else
		{
			serial_puts("MOVING FORWARD 10 CM\n\r\n\r");
			moveFowardUpdate(sensor_data, 10);
		}
		wait_ms(100);
	}
	
	// move the iRobot backwards, 5 or 10 cm
	else if(c == 'S')
	{
		if(precision)
		{
			serial_puts("MOVING BACKWARD 5 CM\n\r\n\r");
			moveBackward(sensor_data, 5);
		}
		else
		{
			serial_puts("MOVING BACKWARD 10 CM\n\r\n\r");
			moveBackward(sensor_data, 10);
		}
		wait_ms(100);
	}

	// rotate the iRobot counter clockwise, 15 degrees
	else if(c == 'A')
	{
		serial_puts("TURNING COUNTER CLOCKWISE 15 DEGREES\n\r\n\r");
		turn_counter_clockwise(sensor_data, 15);
		wait_ms(100);
	}

	// rotate the iRobot clockwise, 15 degrees
	else if(c == 'D')
	{
		serial_puts("TURNING CLOCKWISE 15 DEGREEES\n\r\n\r");
		turn_clockwise(sensor_data, 15);
		wait_ms(100);
	}

	// start sweeping for ir and sonar data
	else if(c == ' ')
	{
		//oi_play_song(0);
		serial_puts("SWEEPING FOR OBJECTS\n\r");
		smallestObjectSweep();
		wait_ms(100);
	}
	
	// clear screen
	else if(c == '-')
	{
		clearScreen();
		wait_ms(100);
	}
	
	// finish command
	else if(c == 'f')
	{
		if(serial_getc() == 'i')
		{
			if(serial_getc() == 'n')
			{
				if(serial_getc() == 'i')
				{
					if(serial_getc() == 's')
					{
						if(serial_getc() == 'h')
						{
							isFinished = 1;
						}
					}
				}
			}
		}
	}
	
	// if any other key is pressed, nothing happens

}

/**
 *
 * Function that detects the error and updates the state of the iRobot
 * 
 * @param isRotating 0 if it's not rotating, 1 if it is
 * @return returns 1 if there's an error detected, 2 if it's only a light sensor, else 0
 */
int errorDetection(int isRotating)
{
	
	// update the sensors
	oi_update(sensor_data);
	
	// if there's an error, set to 1
	int detection = 0;
	
	// reset all sensors
	for(int i = 0; i < 10; i++)
		sensorArray[i] = 0;
	
	/// light errors///
	
	// left sensor to detect black tape
	if(sensor_data->cliff_left_signal < BLACK_MAX && sensor_data->cliff_left_signal > 20)
	{
		// stop the iRobot
		//oi_set_wheels(0, 0);
		// set detection to 1
		detection = 3;
		sensorArray[CLIFF_LEFT_SIGNAL] = 2;
		
	}
	
	// front left sensor to detect black tape
	if(sensor_data->cliff_frontleft_signal < BLACK_MAX && sensor_data->cliff_frontleft_signal > 20)
	{
		// stop the iRobot
		//oi_set_wheels(0, 0);
		// set detection to 1
		detection = 3;
		sensorArray[CLIFF_FRONT_LEFT_SIGNAL] = 2;
		
	}
	
	// front right sensor to detect black tape
	if(sensor_data->cliff_frontright_signal < BLACK_MAX && sensor_data->cliff_frontright_signal > 20)
	{
		// stop the iRobot
		//oi_set_wheels(0, 0);
		// set detection to 1
		detection = 3;
		sensorArray[CLIFF_FRONT_RIGHT_SIGNAL] = 2;
		
	}
	
	// right sensor to detect black tape
	else if(sensor_data->cliff_right_signal < BLACK_MAX && sensor_data->cliff_right_signal > 20)
	{
		// stop the iRobot
		//oi_set_wheels(0, 0);
		// set detection to 1
		detection = 3;
		sensorArray[CLIFF_RIGHT_SIGNAL] = 2;
		
	}
	
	// left sensor to detect white tape
	if(sensor_data->cliff_left_signal > WHITE_MIN)
	{
		// stop the iRobot
		//oi_set_wheels(0, 0);
		// set detection to 1
		detection = 2;
		sensorArray[CLIFF_LEFT_SIGNAL] = 1;
	}
	
	// front left sensor to detect white tape
	if(sensor_data->cliff_frontleft_signal > WHITE_MIN+200)
	{
		// stop the iRobot
		//oi_set_wheels(0, 0);
		// set detection to 1
		detection = 2;
		sensorArray[CLIFF_FRONT_LEFT_SIGNAL] = 1;
	}
	
	// front right sensor to detect white tape
	if(sensor_data->cliff_frontright_signal > WHITE_MIN)
	{
		// stop the iRobot
		//oi_set_wheels(0, 0);
		// set detection to 1
		detection = 2;
		sensorArray[CLIFF_FRONT_RIGHT_SIGNAL] = 1;
	}
	
	// right sensor to detect white tape
	if(sensor_data->cliff_right_signal > WHITE_MIN)
	{
		// stop the iRobot
		//oi_set_wheels(0, 0);
		// set detection to 1
		detection = 2;
		sensorArray[CLIFF_RIGHT_SIGNAL] = 1;
	}
	
	/// bumper errors ///
	
	// left bumper
	if(sensor_data->bumper_left == 1)
	{
		// stop the iRobot
		oi_set_wheels(0, 0);
		// set detection to 1
		detection = 1;
		sensorArray[LEFT_BUMPER] = 1;
		oi_play_song(0);
	}
	
	// right bumper
	if(sensor_data->bumper_right == 1)
	{
		// stop the iRobot
		oi_set_wheels(0, 0);
		// set detection to 1
		detection = 1;
		sensorArray[RIGHT_BUMPER] = 1;
		oi_play_song(0);
	}
	
	/// cliff errors ///
	
	// left cliff
	if(sensor_data->cliff_left == 1)
	{
		// stop the iRobot
		oi_set_wheels(0, 0);
		// set detection to 1
		detection = 1;
		sensorArray[CLIFF_LEFT] = 1;
	}
	
	// front left cliff
	if(sensor_data->cliff_frontleft == 1)
	{
		// stop the iRobot
		oi_set_wheels(0, 0);
		// set detection to 1
		detection = 1;
		sensorArray[CLIFF_FRONT_LEFT] = 1;
	}
	
	// front right cliff
	if(sensor_data->cliff_frontright == 1)
	{
		// stop the iRobot
		oi_set_wheels(0, 0);
		// set detection to 1
		detection = 1;
		sensorArray[CLIFF_FRONT_RIGHT] = 1;
	}
	
	// right cliff
	if(sensor_data->cliff_right == 1)
	{
		// stop the iRobot
		oi_set_wheels(0, 0);
		// set detection to 1
		detection = 1;
		sensorArray[CLIFF_RIGHT] = 1;
	}
	
		
	// if there's an error, print the status of the sensors
	if((detection != 0) && (detection != 3) && (isRotating == 0))
		printSensorStatus(sensorArray);
	
	return detection;
}

/**
*
* Function to loop through the sensor array and decide what to print
* Prints the total number of sensors activated as well as which specific sensors are activated
*/
void printSensorStatus(int arr[])
{
	// print the total number of sensors activated
	int numberSensors = 0;
	char totalString[40];
	for(int i = 0; i < 10; i++) {
		if(sensorArray[i] != 0) {
			numberSensors++;
		}
	}
	sprintf(totalString, "NUMBER OF SENSORS ACTIVATED: %d", numberSensors);
	serial_puts(totalString);
	serial_putc('\n');
	serial_putc('\r');
	//print individually each of the total issues
	if(sensorArray[LEFT_BUMPER] == 1)
	{
		serial_puts("  LEFT BUMPER HAS BEEN HIT\n\r");
	}
	if(sensorArray[RIGHT_BUMPER] == 1)
	{
		serial_puts("  RIGHT BUMPER HAS BEEN HIT\n\r");
	}
	if(sensorArray[CLIFF_LEFT] == 1)
	{
		serial_puts("  OVER LEFT CLIFF\n\r");
	}
	if(sensorArray[CLIFF_FRONT_LEFT] == 1)
	{
		serial_puts("  OVER FRONT LEFT CLIFF\n\r");
	}
	if(sensorArray[CLIFF_FRONT_RIGHT] == 1)
	{
		serial_puts("  OVER FRONT RIGHT CLIFF\n\r");
	}
	if(sensorArray[CLIFF_RIGHT] == 1)
	{
		serial_puts("  OVER RIGHT CLIFF\n\r");
	}
	if(sensorArray[CLIFF_LEFT_SIGNAL] != 0)
	{
		if (sensorArray[CLIFF_LEFT_SIGNAL] == 1) {
			serial_puts("  LEFT OVER WHITE TAPE\n\r");
		}
		else {
			serial_puts("  LEFT OVER BLACK TAPE\n\r");
		}
	}
	if(sensorArray[CLIFF_FRONT_LEFT_SIGNAL] != 0)
	{
		if (sensorArray[CLIFF_FRONT_LEFT_SIGNAL] == 1) {
			serial_puts("  LEFT FRONT OVER WHITE TAPE\n\r");
		}
		else {
			serial_puts("  LEFT FRONT OVER BLACK TAPE\n\r");
		}
	}
	if(sensorArray[CLIFF_FRONT_RIGHT_SIGNAL] != 0)
	{
		if (sensorArray[CLIFF_FRONT_RIGHT_SIGNAL] == 1)
		{
			serial_puts("  RIGHT FRONT OVER WHITE TAPE\n\r");
		}
		else
		{
			serial_puts("  RIGHT FRONT OVER BLACK TAPE\n\r");
		}
	}
	if(sensorArray[CLIFF_RIGHT_SIGNAL] != 0)
	{
		if (sensorArray[CLIFF_RIGHT_SIGNAL] == 1)
		{
			serial_puts("  RIGHT OVER WHITE TAPE\n\r");
		}
		else
		{
			serial_puts("  RIGHT OVER BLACK TAPE\n\r");
		}
	}
}

/**
 *
 * Same function as moveForward but updates and check if there's an error
 *
 * @param sensor the desired sensor to read and update
 * @param centimeters the longest distance the iRobot will move
 */
void moveFowardUpdate(oi_t* sensor, int centimeters){
	// if the current state of the iRobot has an error, return
	if(errorDetection(0) != 0 && errorDetection(0) != 3){
		serial_puts("CAN'T MOVE!\n\r\n\r");
		return;
	}
	
	int millimeters = centimeters * 10;
	int sum = 0;
	oi_set_wheels(150, 150); // move forward;
	
	while (sum < millimeters)
	{
		// check if there's an error detectiion while moving
		// if so, break out the loop and stop
		if(errorDetection(0) != 0 && errorDetection(0) != 3)
		{
			serial_puts("\n\rSTOPPED DUE TO SENSORS!\n\r");
			oi_set_wheels(0, 0);
			break;
		}
		sum += sensor->distance;
	}
	
	oi_set_wheels(0, 0); // stop
	// print the sensor status if there's just black tape
	if(errorDetection(0) == 3)
		printSensorStatus(sensorArray);
}

/**
 *
 * Same function as turnClockwise but updates and check if there's an error
 *
 * @param sensor the desired sensor to read and update
 * @param degrees the amount of rotation clockwise
 */
void turn_clockwise(oi_t *sensor, int degrees){
	// if the current state of the iRobot has an error, return
	if(errorDetection(1) == 1){
		serial_puts("CAN'T MOVE!\n\r\n\r");
		return;
	}
	
	int sum = 0;
	oi_set_wheels(-150, 150); // move forward; 150/500 speed
	while (sum+7 < degrees) 
	{
		if(errorDetection(1) == 1)
		{
			serial_puts("\n\rSTOPPED DUE TO SENSORS!\n\r");
			oi_set_wheels(0, 0);
			
			break;
		}
		wait_ms(10);
		sum -= sensor->angle;
		
	}
	
	oi_set_wheels(0, 0); // stop
	errorDetection(0);
}

/**
 *
 * Same function as turn_clockwise but moves counter clockwise
 *
 * @param sensor the desired sensor to read and update
 * @param degrees the amount of rotation clockwise
 */
void turn_counter_clockwise(oi_t *sensor, int degrees){
	// if the current state of the iRobot has an error, return
	if(errorDetection(1) == 1){
		serial_puts("CAN'T MOVE!\n\r\n\r");
		return;
	}
	
	int sum = 0;
	oi_set_wheels(150, -150); // move forward; 150/500 speed
	while (sum+7 < degrees) 
	{
		if(errorDetection(1) == 1)
		{
			serial_puts("\n\rSTOPPED DUE TO SENSORS!\n\r");
			oi_set_wheels(0, 0);
			
			break;
		}
		wait_ms(10);
		sum += sensor->angle;
		
	}
	oi_set_wheels(0, 0); // stop
	errorDetection(0);
}
