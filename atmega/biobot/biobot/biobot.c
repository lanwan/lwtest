/*
 * biobot.c
 *
 * Created: 2012/6/20 14:35:21
 * Author: steven.zdwang@gmail.com
 * Copyright (C) Lanwan electronic Inc.
 */

#include "biobot_config.h"
#include <avr/io.h>
#include "pt.h"

#define F_CPU 800000000

#define SERVO_PIN0  PC5
#define SERVO_PIN1  PC4
#define SERVO_PIN2  PC3
#define SERVO_PIN3  PC2
#define SERVO_PIN4  PC1
#define SERVO_PIN5  PC0

#define SERVO_PIN6  PB5
#define SERVO_PIN7  PB4
#define SERVO_PIN8  PB3
#define SERVO_PIN9  PB2
#define SERVO_PIN10  PB1
#define SERVO_PIN11  PB0

#define SERVO_PIN12  PD7
#define SERVO_PIN13  PD6
#define SERVO_PIN14  PD5
#define SERVO_PIN15  PD4

#define SERVO_PORT0     ((uint8_t*) 0x35)
#define SERVO_PORT1     ((uint8_t*) 0x35)
#define SERVO_PORT2     ((uint8_t*) 0x35)
#define SERVO_PORT3     ((uint8_t*) 0x35)
#define SERVO_PORT4     ((uint8_t*) 0x35)
#define SERVO_PORT5     ((uint8_t*) 0x35)

#define SERVO_PORT6     ((uint8_t*) 0x38)
#define SERVO_PORT7     ((uint8_t*) 0x38)
#define SERVO_PORT8     ((uint8_t*) 0x38)
#define SERVO_PORT9     ((uint8_t*) 0x38)
#define SERVO_PORT10    ((uint8_t*) 0x38)
#define SERVO_PORT11    ((uint8_t*) 0x38)

#define SERVO_PORT12    ((uint8_t*) 0x32)
#define SERVO_PORT13    ((uint8_t*) 0x32)
#define SERVO_PORT14    ((uint8_t*) 0x32)
#define SERVO_PORT15    ((uint8_t*) 0x32)



struct servo_t{	
    uint8_t enabled;
    uint8_t speed;	
	
    uint16_t start_pulse;
    uint16_t end_pulse;
	
    uint16_t current_pulse;	
	
    uint16_t pulse_slot_rtct;
    uint16_t pulse_slot_count;
    uint16_t pulse_slot_header;
    uint8_t pulse_slot;
};


#define PIN_COUNT                   16                              // 16 servo pin
#define PULSE_SLOT_CODE_TIME        27                              // us
#define PULSE_SLOT_TIME             100      // us
#define PULSE_SLOT_RELOAD_COUNT     (0xFF - PULSE_SLOT_TIME - PULSE_SLOT_CODE_TIME)     // us

#define MAIN_PULSE_PERIOD           20000                           // us
#define MAIN_PULSE_SLOT_CODE_TIME   0                               // us
#define MAIN_PULSE_SLOT_TIME        MAIN_PULSE_PERIOD/PIN_COUNT    // us
#define MAIN_SLOT_COUNT             (MAIN_PULSE_PERIOD / MAIN_PULSE_SLOT_TIME / PIN_COUNT)
#define MAIN_SLOT_RELOAD_COUNTER    (0xFFFF-1150)   // :=13000us, 1250us

static uint8_t volatile gServoIndex = 0;		
static int8_t volatile gActiveServoIndex0 = -1;
static int8_t volatile gActiveServoIndex1 = -1; 								

struct servo_t gServos[PIN_COUNT];

static uint8_t gServoPins[PIN_COUNT] = {
                                        SERVO_PIN0, SERVO_PIN1, SERVO_PIN2, SERVO_PIN3,
                                        SERVO_PIN4, SERVO_PIN5, SERVO_PIN6, SERVO_PIN7,
                                        SERVO_PIN8, SERVO_PIN9, SERVO_PIN10, SERVO_PIN11,
                                        SERVO_PIN12, SERVO_PIN13, SERVO_PIN14, SERVO_PIN15
                                        };

static uint8_t* gServoPorts[PIN_COUNT]={
                                        SERVO_PORT0, SERVO_PORT1, SERVO_PORT2, SERVO_PORT3,
                                        SERVO_PORT4, SERVO_PORT5, SERVO_PORT6, SERVO_PORT7,
                                        SERVO_PORT8, SERVO_PORT9, SERVO_PORT10, SERVO_PORT11,
                                        SERVO_PORT12, SERVO_PORT13, SERVO_PORT14, SERVO_PORT15
                                        };

FILE* gfile_shell = NULL;


#define DEBUG_PULSE_HIGH()    (PORTD |= _BV(PD0))
#define DEBUG_PULSE_LOW()     (PORTD &= ~_BV(PD0))
#define DEBUG_PULSE_HIGH_1()    (PORTD |= _BV(PD1))
#define DEBUG_PULSE_LOW_1()     (PORTD &= ~_BV(PD1))

/**
 * TIMER1(16bit)
 */
ISR( TIMER1_OVF_vect )
{	    			
    DEBUG_PULSE_HIGH();

    if( gServos[gServoIndex].enabled == 0 ) goto inc_index;
		
    // set current high pulse width
    gServos[gServoIndex].current_pulse = gServos[gServoIndex].end_pulse;
	
    //set timer1 or timer2
    if( gActiveServoIndex0 == -1 )
    {	
        gActiveServoIndex0 = gServoIndex;                              		                                   	
        gServos[gServoIndex].pulse_slot_header = gServos[gServoIndex].current_pulse % PULSE_SLOT_TIME + PULSE_SLOT_TIME;			
        gServos[gServoIndex].pulse_slot_count = gServos[gServoIndex].current_pulse / PULSE_SLOT_TIME - 1;                                    			

        // set high pulse
        DEBUG_PULSE_HIGH_1();
        *(gServoPorts[gServoIndex]) |= _BV(gServoPins[gServoIndex]);
	
        TIFR &= ~_BV(TOV0);
        TCNT0 = 0xFF - (gServos[gServoIndex].pulse_slot_header & 0xFF); // must be first
        TIMSK |= _BV(TOIE0);
		
		DEBUG_PULSE_LOW_1();
    }
    else if( gActiveServoIndex1 == -1 )
    {	
        gActiveServoIndex1 = gServoIndex;       		
        gServos[gServoIndex].pulse_slot_header = gServos[gServoIndex].current_pulse % PULSE_SLOT_TIME + PULSE_SLOT_TIME;			
        gServos[gServoIndex].pulse_slot_count = gServos[gServoIndex].current_pulse / PULSE_SLOT_TIME - 1;                                    			

        // set high pulse
        *(gServoPorts[gServoIndex]) |= _BV(gServoPins[gServoIndex]);
	
        TIFR &= ~_BV(TOV2);
        TCNT2 = 0xFF - (gServos[gServoIndex].pulse_slot_header & 0xFF); // must be first
        TIMSK |= _BV(TOIE2);                                                                                  	                                				
    }
    else{
        TIMSK = 0;
    }		

inc_index:		
	gServoIndex++;
	if( gServoIndex >= PIN_COUNT )  gServoIndex = PIN0;    	

// reload counter
    TCNT1 = MAIN_SLOT_RELOAD_COUNTER;

    DEBUG_PULSE_LOW();      	
}


/**
 * TIMER0(8bit)
 * T slot time=100us
 */
ISR( TIMER0_OVF_vect )
{
	DEBUG_PULSE_HIGH_1();
	DEBUG_PULSE_LOW_1();
	
	TIMSK &= ~_BV(TOIE0);	
    TIFR &= ~_BV(TOV0);			
	
	if( gServos[gActiveServoIndex0].pulse_slot_count == 0 ){
        // set low pulse
	    *(gServoPorts[gActiveServoIndex0]) &= ~_BV(gServoPins[gActiveServoIndex0]);	
        gActiveServoIndex0 = -1; // timer 1 idle status	  
		DEBUG_PULSE_HIGH_1();      	
		DEBUG_PULSE_LOW_1();
	}
	else{
        TCNT0 = PULSE_SLOT_RELOAD_COUNT; // must be first
        TIMSK |= _BV(TOIE0);
        gServos[gActiveServoIndex0].pulse_slot_count --;		
	}		
}

/**
 * TIMER2(8bit)
 * T slot time=100us
 */
ISR( TIMER2_OVF_vect )
{
    //DEBUG_PULSE_HIGH();

	TIMSK &= ~_BV(TOIE2);		
	TIFR &= ~_BV(TOV2);
	
	if( gServos[gActiveServoIndex1].pulse_slot_count == 0 ){
        // set low pulse
	    *(gServoPorts[gActiveServoIndex1]) &= ~_BV(gServoPins[gActiveServoIndex1]);	
        gActiveServoIndex1 = -1; // timer 2 idle status		

        //DEBUG_PULSE_LOW();	
	}
	else{
        TCNT2 = PULSE_SLOT_RELOAD_COUNT; // must be first
        TIMSK |= _BV(TOIE2);
        gServos[gActiveServoIndex1].pulse_slot_count --;		
	}       						
}

int main(void)
{
    cli();	
	
    // OUTPUT

    DDRC |= _BV(SERVO_PIN0) | _BV(SERVO_PIN1) | _BV(SERVO_PIN2) | _BV(SERVO_PIN3) | _BV(SERVO_PIN4) | _BV(SERVO_PIN5);
    DDRD |= _BV(SERVO_PIN12) | _BV(SERVO_PIN13) | _BV(SERVO_PIN14) | _BV(SERVO_PIN15);
    DDRB |= _BV(SERVO_PIN6) | _BV(SERVO_PIN7) | _BV(SERVO_PIN8) | _BV(SERVO_PIN9) | _BV(SERVO_PIN10) | _BV(SERVO_PIN11);

    DDRD |= _BV(PD0);

    // 500 -> 532us
    for( int i=0; i<PIN_COUNT; ++i ){
        gServos[i].enabled = 1;
        gServos[i].end_pulse = 500;
    }
    	
    // main clock T=250us
    // TIMER0 clk_io/8
    TCCR0 = _BV(CS01);

    // long clock T>1500us
    // TIMER1, clk_io/8
    TCCR1B |= _BV(CS11);
    TIMSK |= _BV(TOIE1);
    TCNT1 = MAIN_SLOT_RELOAD_COUNTER;
	
    // short clock T>=500us and T<=1500us
    // TIMER2, clk_io/8
    TCCR2 |= _BV(CS21);	
		
    sei();
	
    while(1)
    {		
    }

    return 0;
}




