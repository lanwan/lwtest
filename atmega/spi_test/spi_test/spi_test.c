/*
 * spi_test.c
 *
 * Created: 2012/1/5 22:28:05
 *  Author: win7
 */ 

#include <avr/io.h>

#define DDR_SPI		DDRB
#define DD_MOSI		PB5
#define DD_SCK		PB7	
#define DD_IRQ		INT0_vect
#define DbgPrint	printf
#define ROLER_SENDER		1	//sender or receiver

static void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

static void SPI_MasterTransmit(char cData)
{
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)))
	;
}

static void SPI_MasterWrite(const char* pData, uint16_t len)
{
	while(len--){
		SPI_MasterTransmit(*pData);	
		pData ++;		
	}
}

ISR(DD_IRQ)
{
	static uint8_t cData;
	cData = SPDR;
	DbgPrint("%c", cData);
}

int main(void)
{
	char test_str [] = "Hello, What's your Name?\0";
	SPI_MasterInit();
	
    while(1)
    {
		if( ROLER_SENDER ){
			SPI_MasterWrite(test_str, sizeof(test_str));
		}
	}
}						