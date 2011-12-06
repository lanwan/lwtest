
#include "ds18b20.h"

#include <util/delay.h>

/************************************************************************
 * DS18B20 Handle                                                       *
 ************************************************************************/
#define DQ_OUT  (DS18B20_DDR |= _BV(DS18B20_DQ))      //Change DS18B20_DQ Bus to Output Data
#define DQ_IN   (DS18B20_DDR &= ~_BV(DS18B20_DQ))     //Change DS18B20_DQ Bus to Input Data
#define DQ_ON   (DS18B20_PORT |= _BV(DS18B20_DQ))     //Change DS18B20_DQ Bus High
#define DQ_OFF  (DS18B20_PORT &= ~_BV(DS18B20_DQ))    //Change DS18B20_DQ Bus Low
#define DQ_PIN  (DS18B20_PIN & _BV(DS18B20_DQ))       //Read DS18B20_DQ Bus H/L

static void ds18b20_reset(void);
static uint8_t ds18b20_read(void);
static void ds18b20_write(uint8_t dat);

static void ds18b20_reset(void)
{
    /* Bus set 1 and delay 1us */
    DQ_OUT;
    DQ_ON;
    _delay_us(1);

    /* Bus set 0 and delay 480us */
    DQ_OFF;
    _delay_us(480);

    /* Bus set 1 and wait Bus ready and released Bus */
    DQ_ON;
    DQ_IN;
    while(!(DQ_PIN));
    DQ_OUT;
    DQ_ON;			// release Bus
    _delay_us(150);
}

static void ds18b20_write(uint8_t dat)
{
    uint8_t i;
    for(i=0; i<8; i++)
    {
        /* Bus set 0 */
        DQ_OUT;
        DQ_OFF;

        /* Write 1 bit data */
        if( dat&0x01 )
        {
            DQ_ON;
        }
        else
        {
            DQ_OFF;
        }

        /* Bus set 1 and delay 30us */
        _delay_us(30);
        DQ_ON;

        /* shift right data 1 bit */
        dat >>= 1;
    }
    DQ_ON;  // release Bus
}

static uint8_t ds18b20_read(void)
{
    uint8_t i,dat = 0;
    for(i = 0; i<8; i++)
    {
        /* Set Bus high and low and delay 1us */
        DQ_OUT;
        dat >>= 1;
        DQ_ON;
        DQ_OFF;
        _delay_us( 1 );

        /* Bus set high and wait respond data */
        DQ_ON;
        DQ_IN;
        if(DQ_PIN)
        {
            dat |= _BV( 7 );
        }
        else
        {
            dat &= ~_BV( 7 );
        }

        /* wait 30us */
        _delay_us(30);
    }
    DQ_OUT;
    return dat;
}

uint16_t ds18b20_get_temp(void)
{
    uint8_t temp_low = 0, temp_high = 0;
    uint16_t temp  =  0;
    float t = 0;
    ds18b20_reset();		//reset
    ds18b20_write( 0xcc );	//skip rom
    ds18b20_write( 0x44 );	//start temperature transform
    _delay_ms( 90 );		//delay 90ms
    ds18b20_reset();		//reset
    ds18b20_write( 0xcc );	//skiprom
    ds18b20_write( 0xbe );	//read register
    temp_low = ds18b20_read();		//read temperature low byte
    temp_high = ds18b20_read();		//read temperature hit byte
    temp = (temp_high << 8) | temp_low;
    t = temp * 0.0625;
    temp = t * 10 + 0.5;
    return temp;
}

