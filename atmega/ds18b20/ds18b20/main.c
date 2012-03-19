/*
 * ds18b20.c
 *
 * Created: 2011/12/6 18:36:11
 *  Author: win7
 */ 

#include <avr/io.h>

#define F_CPU 14745600UL

#include <util/delay.h>


#include <stdio.h>
#include <avr/io.h>

#include "ds18b20.h"

#if defined (__AVR_ATmega128__)
	#define USR UCSR0A
	#define UCR UCSR0B
	#define UDR UDR0
	#define UBRR UBRR0L
	#define USART_RX USART0_RX_vect
#endif

#if defined (__AVR_ATmega644__) || (defined (__AVR_ATmega644P__) && !USART_USE1)
	#define USR UCSR0A
	#define UCR UCSR0B
	#define UBRR UBRR0L
	#define EICR EICRB
	#define TXEN TXEN0
	#define RXEN RXEN0
	#define RXCIE RXCIE0
	#define UDR UDR0
	#define UDRE UDRE0
	#define USART_RX USART0_RX_vect
#endif

#if defined (__AVR_ATmega644P__) && USART_USE1
	#define USR UCSR1A
	#define UCR UCSR1B
	#define UBRR UBRR1L
	#define EICR EICRB
	#define TXEN TXEN1
	#define RXEN RXEN1
	#define RXCIE RXCIE1
	#define UDR UDR1
	#define UDRE UDRE1
	#define USART_RX USART1_RX_vect
#endif

#if defined (__AVR_ATmega32__)
	#define USR UCSRA
	#define UCR UCSRB
	#define UBRR UBRRL
	#define EICR EICRB
	#define USART_RX USART_RXC_vect
#endif

#if defined (__AVR_ATmega8__)
	#define USR UCSRA
	#define UCR UCSRB
	#define UBRR UBRRL
#endif

#if defined (__AVR_ATmega88__)
	#define USR UCSR0A
	#define UCR UCSR0B
	#define UBRR UBRR0L
	#define TXEN TXEN0
	#define UDR UDR0
	#define UDRE UDRE0
#endif
	
void avr_uart_init(int16_t ubrr_val) {
#if defined (__AVR_ATmega644P__)	
	UCR =(1 << TXEN | 1 << RXEN | 1<< RXCIE);
	// 0 = Parity Mode Disabled
	// 1 = Parity Mode Enabled, Even Parity
	// 2 = Parity Mode Enabled, Odd Parity
	//UCSRC = 0x06 + ((parity+1)<<4);
	//UCSRC |= (1<<USBS);
	UBRR=(F_CPU / (ubrr_val * 16L) - 1);
#else		
    /* 设置波特率 */
    UBRRH = (unsigned char)(ubrr_val>>8);
    UBRRL = (unsigned char)ubrr_val;

    /* 接收器与发送器使能 */
    UCSRB = (1<<RXEN)|(1<<TXEN);

    /* 设置帧格式: 8个数据位, 1个停止位*/
    UCSRC = (1<<URSEL)|(3<<UCSZ0);
#endif	
}
int avr_uart_putchar(char c, FILE *unused)
{
#if defined (__AVR_ATmega644P__)	
	loop_until_bit_is_set(USR, UDRE);
#else
	loop_until_bit_is_set(UCSRA, UDRE);
#endif	
	UDR = c;
	return 0;
}


static FILE mystdout = FDEV_SETUP_STREAM(avr_uart_putchar, NULL, _FDEV_SETUP_WRITE);
static void init_c_language(void)
{
	stdout = &mystdout;
}

static int SmsEncode(char* str)
{
	
}

int main(void)
{	
	avr_uart_init(19200);
	init_c_language();
	while(1){
		_delay_ms(1000);
		_delay_ms(1000);
		_delay_ms(1000);
		printf("%d\r\n", ds18b20_get_temp());
	}		
}


