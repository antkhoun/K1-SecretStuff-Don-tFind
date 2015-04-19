/*
 * movement.h
 *
 * Created: 1/27/2015 12:51:35 PM
 *  Author: sethl
 */ 

#include "open_interface.h"
#ifndef MOVEMENT_H_
#define MOVEMENT_H_

void moveBackward(oi_t* sensor, int centimeters);
void moveFoward(oi_t* sensor, int centimeters);
void turnClockwise(oi_t* sensor, int degrees);


#endif /* MOVEMENT_H_ */