/*
 * TP3.c
 *
 * Created: 8/6/2021 16:33:39
 * Author : molin
 */ 

#include <avr/io.h>
#include "avr/interrupt.h"
#include <stdint.h>
#include "serialPort.h"
#include <util/delay.h>
static volatile unsigned char  bienvenida[200] =" Generador de señales digitales programable \0 Ingrese frecuencia entre 100 y 10000Hz \0 ON: para encender, OFF para apagar, RST para reiniciar \0";
static volatile uint8_t accion_flag = 0;
static volatile unsigned char buffer[4];
void realizar_accion(unsigned char * accion);
uint8_t comparar_str(unsigned char * str1, unsigned char * str2, uint8_t longitud);
uint8_t str_to_int(unsigned char * str);
int main (void)
{

	//DDRB = 0xFF; //Port B is output
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);//activo recepcion y transmision
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);//tamanio info
	UBRR0L = 103;
	SerialPort_TX_Interrupt_Enable();
	sei();
	while(1)
	{
			if (accion_flag == 1)
			{
				accion_flag = 0;
				realizar_accion(buffer);
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
//usar srt comapare
//usar atoi
void realizar_accion(unsigned char * accion)
{
		SerialPort_RX_Interrupt_Disable();
		unsigned char reset[3] = "rst";
		unsigned char on[3] = "on";
		unsigned char off[3] = "off";
		uint8_t nro =0;
		
		if (comparar_str(reset,accion,(uint8_t)3))
		{
			SerialPort_Send_String("resteado \n");
		}else if (comparar_str(on,accion,(uint8_t)2))
			{
				SerialPort_Send_String("prendido \n");
			}
			else if (comparar_str(off,accion,(uint8_t)3))
			{
				SerialPort_Send_String("apagado \n");
			}else if (str_to_int(accion))
			{
				SerialPort_Send_String("es numero \n");
				nro = (str_to_int(accion));
				SerialPort_Send_uint8_t(nro);
			}else 
			{
				SerialPort_Send_String("pija \r");
				nro = (str_to_int(accion));
				SerialPort_Send_uint8_t(nro);
			}
		SerialPort_RX_Interrupt_Enable();
}
uint8_t comparar_str(unsigned char * str1, unsigned char * str2, uint8_t longitud)
{
	uint8_t condicion = 1;
	for (uint8_t i=0; i< longitud; i++)
	{
		if (str1[i]!=str2[i])
		{
			condicion = 0;
			break;
		}
	}
	return condicion;
}
uint8_t str_to_int(unsigned char * str)
{
	
	uint8_t valor = 0,i=0;
	while (str[i] != '\0')
	{
		if ((str[i] <= 57) && (str[i] >= 48) )
		{
			valor*=10;
			valor += (str[i] - 48);
			
		}else
		{
			valor = 0;
			return valor;
		}	
		i++;
	}
	if ((valor >= 100) && (valor <= 10000))
	{
		return valor;
	}else
		valor= 0;
		return valor;
}
