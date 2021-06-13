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
#include "Timer.h"
void logicaDeAplicacion(char comando[]);
int comparador(char palabra[]);

char comando[10];

int main (void)
{

	UART_Init();
	Timer1_Init();

	sei();
	while(1)
	{
		if (UART_HayComando())//chequea flag 1-> hay comando 0-> todavia no hay comando
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
			case 0: UART_On();break;
			case 1: UART_Off();break;
			case 2: UART_Reset(); break;
			case 3: UART_Print_Error(); break;
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
