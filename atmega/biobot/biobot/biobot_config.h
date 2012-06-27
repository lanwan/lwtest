/*
 * biobot_config.h
 *
 * Created: 2012/6/20 15:23:04
 *  Author: win7
 */ 


#ifndef BIOBOT_CONFIG_H_
#define BIOBOT_CONFIG_H_

#include <stdint.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

/**
 * MAROC
 */
#define STREQ(a, b) ( (*a) == (*b) && strcmp((a), (b)) == 0 )
#define SIZEOF(T)       (sizeof(T)/sizeof(T[0]))
#define TRUE    1
#define FALSE   0


extern FILE* gfile_shell;


#endif /* BIOBOT_CONFIG_H_ */