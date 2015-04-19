/*
 * Lab_5.c
 *
 * Created: 2/17/2015 12:05:35 PM
 *  Author: sethl
 */ 


#include <avr/io.h>
#include "lcd.h"
#include "util.h"

// initializes serial comms
void serial_init(unsigned long baud){
	baud = (16000000L/8/baud) -1;
	//set baud rate
	UBRR0H = (unsigned char)(baud >> 8);
	UBRR0L = (unsigned char)baud;
	
	//enable double speed
	UCSR0A = 0b00000010;
	
	//set frame format: 8 data bits, 2 stop bit
	UCSR0C = 0b00001110;
	
	//enable receiver and transmitter
	UCSR0B = 0b00011000;
}

// places character in serial stream 
void serial_putc(char data){
	//wait for empty transmit buffer by checking the UDRE bit
	while((UCSR0A & 0b00100000) == 0);
		//Put data into transmit buffer, sends the data
		UDR0 = data;
}

//places string in serial stream
void serial_puts(char* str){
    while (*str) {
	  serial_putc(*str);
	  str++;
	}
}

// recieives transmitted serial chars
char serial_getc(){
	//wait for receive complete flag
	char button6String[] = "Yes";
	char button5String[] = "No";
	char button4String[] = "Blue, no green, Ahhhhh!!!";
	char button3String[] = "Hello world!";
	char button2String[] = "Go Cyclones!";
	char button1String[] = "CPRE 288";
	int currentButton = 0;
	while((UCSR0A & 0b10000000) == 0) {
		switch(read_push_buttons()) {
			case 6:  // 6th button is being pressed
				if(currentButton != 0)
					break;
				lprintf("%s", button6String);
				for(int j = 0; j < 4; j++) {
					serial_putc(button6String[j]);
				}
				serial_putc('\n');
				serial_putc('\r');
				currentButton = 1;
				break;
			case 5: // 5th button is being pressed
				if(currentButton != 0)
					break;
				lprintf("%s", button5String);
				for(int j = 0; j < 3; j++) {
					serial_putc(button5String[j]);
				}
				serial_putc('\n');
				serial_putc('\r');
				currentButton = 1;
				break;
			case 4: // 4th button is being pressed
				if(currentButton != 0)
					break;
				lprintf("%s", button4String);
				for(int j = 0; j < 26; j++) {
					serial_putc(button4String[j]);
				}
				serial_putc('\n');
				serial_putc('\r');
				currentButton = 1;
				break;
			case 3: // 3rd button is being pressed
				if(currentButton != 0)
					break;
				lprintf("%s", button3String);
				for(int j = 0; j < 12; j++) {
					serial_putc(button3String[j]);
				}
				serial_putc('\n');
				serial_putc('\r');
				currentButton = 1;
				break;
			case 2: // 2nd button is being pressed
				if(currentButton != 0)
					break;
				lprintf("%s", button2String);
				for(int j = 0; j < 13; j++) {
					serial_putc(button2String[j]);
				}
				serial_putc('\n');
				serial_putc('\r');
				currentButton = 1;
				break;
			case 1: // first button is being pressed
				if(currentButton != 0)
					break;
				lprintf("%s", button1String);
				for(int j = 0; j < 9; j++) {
					serial_putc(button1String[j]);
				}
				serial_putc('\n');
				serial_putc('\r');
				currentButton = 1;
				break;
			case 0: // no buttons are being pressed and handles if button is held down
				currentButton = 0;
				break;
		}
			
	}
	
	//reads data from the receive bugger, clears the receive buffer
	return UDR0;
}



/*
int main(void)
{
	lcd_init();
	init_push_buttons();
	serial_init(57600);
	
	int i = 0;
	char string[21];
	
	while(1) {
		char c = serial_getc();
		//serial_putc(c);
		lprintf("Received: [%c] %d",c,i);
		string[i] = c;
		if(i == 19 || c == '\r'){
			string[++i] = '\0';
			i = -1;
			lprintf("%s", string);
			serial_putc('\n');
		}
		i++;
	
	}
}
*/

