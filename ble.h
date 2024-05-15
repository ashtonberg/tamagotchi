
/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef BLE_H
#define	BLE_H

#include <xc.h> // include processor files - each processor file is guarded.  

void bleInit(const char *name);
void usartInit();
void usartWriteChar(char c);
void usartWriteCommand(const char *cmd);
char usartReadChar();
void usartReadUntil(char *dest, const char *end_str);
uint8_t usartReadMessage(char *dest);


#endif

