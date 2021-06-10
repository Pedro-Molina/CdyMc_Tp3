#include <avr/io.h>
#include "avr/interrupt.h"
#include "serialPort.h"
#include <avr/io.h>

static volatile uint8_t comando_flag = 0;
static volatile unsigned char  bienvenida[200] =" Generador de señales digitales programable \0 Ingrese frecuencia entre 100 y 10000Hz \0 ON: para encender, OFF para apagar, RST para reiniciar \0";

static  char buffer[20];


void UART_Init()
{
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);//activo recepcion y transmision
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);//tamanio info
	UBRR0L = 103;
	SerialPort_TX_Interrupt_Enable();
	SerialPort_RX_Interrupt_Enable();
}
uint8_t UART_HayComando()
{
	return comando_flag;
}
void UART_Set_False_Command_Flag()
{
	comando_flag = 0;
}
char* UART_GetComando()
{
	SerialPort_RX_Interrupt_Disable();
	return buffer;
}
void UART_Set_Frecuencia(int f)
{
	OCR1A = 8000000UL/f - 1;
	SerialPort_RX_Interrupt_Enable();
}
void UART_On()
{
	DDRB = (0<<PB1);
	SerialPort_RX_Interrupt_Enable();
}
void UART_Off()
{
	DDRB = (1<<PB1);
	SerialPort_RX_Interrupt_Enable();
}
void UART_Reset()
{
	SerialPort_TX_Interrupt_Enable();
	SerialPort_RX_Interrupt_Enable();
}
void UART_Print_String(char str[])
{
	SerialPort_Send_String(str); //no usar esto?
	SerialPort_RX_Interrupt_Enable();
}
ISR (USART_RX_vect) //interrupción de recepción
{
	
	static volatile uint8_t i = 0;
	unsigned char dato;
	
	dato = UDR0;
	
	if (dato == '\r')
	{
		buffer[i] = '\0';
		i=0;
		comando_flag = 1;
		SerialPort_RX_Interrupt_Disable(); //deshabilito las interrupciones ya que tengo una sección critica por mis variables compartidas 'buffer' y 'acciónFlag'
	}
	else
	{
		buffer[i] = dato;
		i++;
	}
	
	
}
ISR (USART_UDRE_vect) //interrupción de transmisión
{
	static volatile uint8_t i = 0;
	static volatile uint8_t j = 0;
	if (bienvenida[i] != '\0')
	{
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
			i=0;j=0;
			SerialPort_TX_Interrupt_Disable();
			SerialPort_RX_Interrupt_Enable();
		}
		DDRB = (1<<PB1); //habilito por si estaba en off
	}
}