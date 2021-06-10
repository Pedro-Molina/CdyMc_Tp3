/*
 * GccApplication7.c
 *
 * Created: 9/6/2021 10:06:45
 * Author : ignac
 */ 

#define  F_CPU 8000000UL

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "avr/interrupt.h"
#include "Uart.h"

void logicaDeAplicacion(char comando[]);
int comparador(char palabra[]);

void Timer1_Init(void);


char comando[10];

int main (void)
{

	UART_Init();
	Timer1_Init();

	sei();
	while(1)
	{
		if (UART_HayComando())
		{
			strcpy(comando,UART_GetComando());
			UART_Set_False_Command_Flag();
			logicaDeAplicacion(comando);
		}
	}
	return 0;
}



void logicaDeAplicacion(char comando[])
{
	
	int nro = atoi(comando);
	
	if ((100 <= nro ) && (nro <= 10000))
	{
		UART_Set_Frecuencia(nro);
	}
	else 
	{
		switch(comparador(comando))
		{
			case 0: UART_Off();break;
			case 1: UART_On();break;
			case 2: UART_Reset(); break;
			case 3: UART_Print_String("Comando invalido \r"); break;
		}	
		
	}
	
	
	
}
int comparador(char comando[])
{
	if(!strcmp(comando,"on"))
	{
		return 0;
	}
	if(!strcmp(comando,"off"))
	{
		return 1;
	}
	if(!strcmp(comando,"rst"))
	{
		return 2;
	}
	return 3;
}
void Timer1_Init(void){
	DDRB = (1<<PB1);                        //PB1 Salida
	TCCR1A=  (1<< COM1A0);                    //COM1A  = Toggle
	TCCR1B=(1<<WGM12) | (1<<CS10);            //CTC no prescalar
	//TIMSK1=(1<<OCIE1A);                        //Habilita la interrupcion por captura
}