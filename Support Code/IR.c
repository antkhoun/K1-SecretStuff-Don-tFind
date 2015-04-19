/*
 * Lab_6.c
 *
 * Lab_6_Project_DaveComputer.c
 *
 * Created: 3/1/2015 5:13:33 PM
 *  Author: dlowry
 */ 

#include <avr/io.h>
#include "util.h"
#include "lcd.h"
#include <stdlib.h>
#include <math.h>

// inializes the adc
void ADC_init(){
	ADMUX = _BV(REFS1) | _BV(REFS0) | 0b00000010; // make sure initialized to correct port. REFS1 and 0 handle the voltage, 0b000000010 selects the channel.
	ADCSRA = _BV(ADEN) | (7<<ADPS0);
}

// reads the incoming adc data
unsigned int ADC_read(){ 
	ADCSRA |= _BV(ADSC);
	while (ADCSRA & _BV(ADSC)) {
	}
	return ADC;
}

// checks if 2 numbers are within 35 of eachother
int within35(double numberOne, int numberTwo) {
	if(abs((int)(numberOne - numberTwo)) < 35)
		return 1;
	else
		return 0;
}



// converts the adc values to meaningful distances
int adcConvert(int adcValue){
	
	// equation from Excel Quantization = 5391.9*(x^-0.761) x being the distance.
	int arrOfDistances[45];
	float arrOfQuantizationsFromDistancesEquation[45]; //build this and then compare with read quantized value.
	int distance = 9;
	int quantizedValue;
	
	for(int i = 0; i < 45; i++) {  // fill an array with the number of distances.
		arrOfDistances[i] = distance;
		distance++;
	}
	
	for(int j = 0; j < 45; j++) {
		arrOfQuantizationsFromDistancesEquation[j] = 5391.9*pow(arrOfDistances[j],-0.761); // fill an array with mapped quantization values
	}
	 
	while(1){
		int k = 0, l = 0, totalQuantization = 0;
		while(l < 5){   // Compute the average of 5 samples, each sample taken at 50ms
			quantizedValue = ADC_read();
			totalQuantization += quantizedValue;
			wait_ms(5);
			l++;
		}
		quantizedValue = totalQuantization/5;  // finish averaging compuatation and write the value into quantized value
		l = 0;
		while(1) {
			if(within35(arrOfQuantizationsFromDistancesEquation[k], quantizedValue)) // checks if we are within 35 of the mapped quantized value
				break;
			else if(k >45) { // Should never enter this case, but breaks out just in case
				break;
			}
			else
				k++;	
		}
		return arrOfDistances[k];
		//lprintf("Quantization: %u,\n Distance: %d",quantizedValue, arrOfDistances[k]);  //print Quantization and Distance Values
	}
}
