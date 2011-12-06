#ifndef __DS18B20__H__
#define __DS18B20__H__

/************************************************************************
	ds18b20.h                                                            
	read temperature from ds18b20 1 wire sensor                          
	steven.zdwang@gmail.com
************************************************************************/
#include <stdint.h>
#include <avr/io.h>
/************************************************************************
 * DS18B20 GPIO,Please change it by your platform                       *
************************************************************************/
#define DS18B20_DDR     DDRC			//<!BSP-PORT
#define DS18B20_PORT    PORTC			//<!BSP-PORT
#define DS18B20_PIN     PINC			//<!BSP-PORT
#define DS18B20_DQ      PC7				//<!BSP-PORT

uint16_t ds18b20_get_temp(void);

#endif
