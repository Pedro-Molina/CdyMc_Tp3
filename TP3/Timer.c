/*
 * CFile1.c
 *
 * Created: 12/6/2021 20:19:15
 *  Author: molin
 */ 
#include <avr/io.h>
#include "avr/interrupt.h"
#include "serialPort.h"
#include <avr/io.h>
#include <string.h>
#include "Timer.h"

void Timer1_Init(void){

	TCCR1A=  (1<< COM1A0);                    //COM1A  = Toggle
	TCCR1B=(1<<WGM12) | (1<<CS11);            //CTC preescaldor fclock/8

}
