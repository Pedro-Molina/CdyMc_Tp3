/*
 * TP3.c
 *
 * Created: 8/6/2021 16:33:39
 * Author : molin
 */ 
#define  F_CPU 8000000UL
#include <avr/io.h>
#include "avr/interrupt.h"
#include <stdint.h>
#include "serialPort.h"
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static volatile unsigned char  bienvenida[200] =" Generador de señales digitales programable \0 Ingrese frecuencia entre 100 y 10000Hz \0 ON: para encender, OFF para apagar, RST para reiniciar \0";
static volatile uint8_t accion_flag = 0;
static  char buffer[20];
void realizar_accion();
uint8_t comparar_str(unsigned char * str1, unsigned char * str2, uint8_t longitud);
uint8_t str_to_int(unsigned char * str);
void Timer1_Init(void);
int main (void)
{

	//DDRB = 0xFF; //Port B is output
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);//activo recepcion y transmision
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);//tamanio info
	UBRR0L = 103;
	Timer1_Init();
	//SerialPort_TX_Interrupt_Enable();
	SerialPort_RX_Interrupt_Enable();
	sei();
	while(1)
	{
			if (accion_flag == 1)
			{
				accion_flag = 0;
				realizar_accion();
			}
	}
	return 0;
}

ISR (USART_RX_vect)
{
	
	static volatile uint8_t i = 0;
	unsigned char dato;
	
	dato = UDR0;
	
	if (dato == '\r')
	{
		buffer[i] = '\0';
		//SerialPort_TX_Interrupt_Enable();
		i=0;
		accion_flag = 1;
	}
	else
	{
		buffer[i] = dato;
		i++;
	}
	
	
}

ISR (USART_UDRE_vect)
{
	static volatile uint8_t i = 0;
	static volatile uint8_t j = 0;
	if (bienvenida[i] != '\0')
	{
		SerialPort_Wait_For_TX_Buffer_Free();
		SerialPort_Send_Data(bienvenida[i]);
		i++;
	}
	else
	{
		SerialPort_Send_Data('\r');
		SerialPort_Send_Data('\n');
		i++;
		j++;
		if (j == 3){
			SerialPort_TX_Interrupt_Disable();
			SerialPort_RX_Interrupt_Enable();
		}
	}
}

void realizar_accion()
{
		SerialPort_RX_Interrupt_Disable();
		char reset[4] = "rst";
		char on[3] = "on";
		char off[4] = "off";
		int nro = atoi(buffer);
		
		if ((100 <= nro ) && (nro <= 10000))
		{
			OCR1A = 8000000UL/nro - 1;
		}else if (!strcmp(buffer,reset))
			{
				DDRB = (0<<PB1); 
				SerialPort_Send_String("reset");
			}else if (!strcmp(buffer,on))
				{
					DDRB = (1<<PB1); 
					SerialPort_Send_String("prendido \n");
				}else if (!strcmp(buffer,off))
					{
						DDRB = (0<<PB1);  
						SerialPort_Send_String("apagado \n");
					}else 
						{
						SerialPort_Send_String("pija \r");
						}
		SerialPort_RX_Interrupt_Enable();
}

void Timer1_Init(void){
	DDRB = (1<<PB1);                        //PB1 Salida
	TCCR1A=  (1<< COM1A0);                    //COM1A  = Toggle
	TCCR1B=(1<<WGM12) | (1<<CS10);            //CTC no prescalar
	//TIMSK1=(1<<OCIE1A);                        //Habilita la interrupcion por captura
}

/*void Timer1_Init(void){
	DDRB = (1<<PB1);                        //PB1 Salida
	TCCR1A=  (1<< COM1A0);                    //COM1A  = Toggle
	TCCR1B=(1<<WGM12) | (1<<CS10);            //CTC no prescalar
	OCR1A = 8000000UL/note[pos++] - 1;            //pongo la frecuencia
	TIMSK1=(1<<OCIE1A);                        //Habilita la interrupcion por captura
}


int main(void)
{
	Timer1_Init();
	sei();
	while (1);
}

ISR (TIMER1_COMPA_vect)
{
	static unsigned short cant=0;
	if (++cant == 10){
		cant = 0;
		OCR1A = 8000000UL/note[pos++];            //pongo la nueva frecuencia
		if (pos == 12)
		pos = 0;
	}
}*/

