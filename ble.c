#ifndef F_CPU
#define F_CPU 3333333
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "ble.h"
#include "globals.h"

#define BUF_SIZE 128
#define BLE_RADIO_PROMPT "CMD> "

#define USART1_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

#define SAMPLES_PER_BIT 16
#define USART_BAUD_VALUE(BAUD_RATE) (uint16_t) ((F_CPU << 6) / (((float) SAMPLES_PER_BIT) * (BAUD_RATE)) + 0.5)

void usartInit() {
    // TODO Fill in with USART peripheral initialization code
    //8 bit mode, async mode
    PORTA.DIR &= ~PIN1_bm;
    PORTA.DIR |= PIN0_bm;
    
    USART0.BAUD = (uint16_t)USART_BAUD_VALUE(9600);
    
    USART0.CTRLA |= USART_RXCIE_bm;      //enable receive interrupts
    
    USART0.CTRLB |= USART_TXEN_bm |  USART_RXEN_bm | USART_RXMODE_NORMAL_gc; 
    USART0.CTRLC |= USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_0_bm;
    USART0.CTRLC |= USART_CHSIZE_8BIT_gc;  // Set character size to 8-bit
    
    USART0.CTRLC &= ~USART_SBMODE_bm;  // Set stop bit to 1-bit
}

void usartWriteChar(char c) {
    // TODO fill this in
    while (!(USART0.STATUS & USART_DREIF_bm))
    {
        ;
    }        
    USART0.TXDATAL = c;
}

void usartWriteCommand(const char *cmd) {
    for (uint8_t i = 0; cmd[i] != '\0'; i++) {
        usartWriteChar(cmd[i]);
    }
}

char usartReadChar() {
    // TODO fill this in and return correct value
    while (!(USART0.STATUS & USART_RXCIF_bm))
    {
        ;
    }
    return USART0.RXDATAL;
}

void usartReadUntil(char *dest, const char *end_str) {
    // Zero out dest memory so we always have null terminator at end
    memset(dest, 0, BUF_SIZE);
    uint8_t end_len = strlen(end_str);
    uint8_t bytes_read = 0;
    while (bytes_read < end_len || strcmp(dest + bytes_read - end_len, end_str) != 0 ) {
        dest[bytes_read] = usartReadChar();
        bytes_read++;
    }
}

uint8_t usartReadMessage(char *dest) {
    // Zero out dest memory so we always have null terminator at end
    memset(dest, 0, BUF_SIZE);
    uint8_t bytes_read = 0;
    
    usartReadChar();    //ignore first percent
    dest[0] = usartReadChar();  //read in first character of message

    while(dest[bytes_read] != '%') {
        bytes_read++;
        dest[bytes_read] = usartReadChar();
    }
    
    return bytes_read;
}



// Must be called after usartInit()
void bleInit(const char *name) {
    // Put BLE Radio in "Application Mode" by driving F3 high
    PORTF.DIRSET = PIN3_bm;
    PORTF.OUTSET = PIN3_bm;

    // Reset BLE Module - pull PD3 low, then back high after a delay
    PORTD.DIRSET = PIN3_bm | PIN2_bm;
    PORTD.OUTCLR = PIN3_bm;
    _delay_ms(10);
    PORTD.OUTSET = PIN3_bm;

    // The AVR-BLE hardware guide is wrong. Labels this as D3
    // Tell BLE module to expect data - set D2 low
    PORTD.OUTCLR = PIN2_bm;
    _delay_ms(200); // Give time for RN4870 to boot up

    char buf[BUF_SIZE];
    // Put RN4870 in Command Mode
    usartWriteCommand("$$$");
    usartReadUntil(buf, BLE_RADIO_PROMPT);
    
            
    // Change BLE device name to specified value
    // There can be some lag between updating name here and
    // seeing it in the LightBlue phone interface
    strcpy(buf, "S-,");
    strcat(buf, name);
    strcat(buf, "\r\n");
    usartWriteCommand(buf);
    usartReadUntil(buf, BLE_RADIO_PROMPT);


    // TODO 1: Send command to remove all previously declared BLE services
    usartWriteCommand("SS,00\r\n");
    usartReadUntil(buf,BLE_RADIO_PROMPT);
    
    // TODO 2: Add a new service. Feel free to use any ID you want from the
    // BLE assigned numbers document. Avoid the "generic" services.
    
    //Alert Notification Service
    usartWriteCommand("PS,1811\r\n");
    usartReadUntil(buf,BLE_RADIO_PROMPT);
    
    // TODO 3: Add a new characteristic to the service for your accelerometer
    // data. Pick any ID you want from the BLE assigned numbers document.
    // Avoid the "generic" characteristics.
    
    //ACS Data Out Notify
    //1A = notify, write, and read
    usartWriteCommand("PC,2B31,1A,02\r\n");
    usartReadUntil(buf,BLE_RADIO_PROMPT);
    
    // TODO 4: Set the characteristic's initial value to hex "00".
    usartWriteCommand("SHW,0072,05\r\n");
    usartReadUntil(buf,BLE_RADIO_PROMPT);   
    
}
//
//int main() {
//    usartInit();
//    bleInit("ashton");
//
//    char buf[BUF_SIZE];
//    _delay_ms(5000);
//    while (1) {
//        usartWriteCommand("SHW,0072,02\r\n"); 
//    
//        usartReadUntil(buf, BLE_RADIO_PROMPT);
//        PORTA.DIRSET = P9 IN7_bm;
//        _delay_ms(1000);
//    }
//}


char buff[BUF_SIZE];
ISR(USART0_RXC_vect) {
    USART0.CTRLA &= ~USART_RXCIE_bm;        //disable interrupts until done reading new message
    
    if(sending) {
        usartReadUntil(buff, BLE_RADIO_PROMPT);
        sending = 0;
    }
    else {
        uint8_t bytes = usartReadMessage(buff);

        if(bytes > 2) {
            if(buff[0] == 'W' && buff[1] == 'V') {      //received characteristic update
                char first = buff[bytes-2];
                char second = buff[bytes-1];

                if (first == 'A' && second == 'B') {    //value = "AB"
                    //pet the pet
                    petPet = 5;
                }
            }
        }
    }
    
    USART0.CTRLA |= USART_RXCIE_bm;        //re-enable interrupts

}
