/*
 * File:   main.c
 * Author: ashtonberg
 *
 */


#ifndef F_CPU
#define F_CPU 3333333
#endif


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include "ble.h"
#include "shapes.h"
#include "i2c.h"
#include "rtc.h"

volatile uint8_t homeScreen = 1;
volatile uint8_t cursorPos = 0;  //current cursor position
volatile uint8_t feeding = 0;
volatile uint8_t lightOn = 1;
volatile uint8_t hunger = 11;
volatile int8_t happiness = 5;
volatile uint8_t level = 5;
volatile uint8_t statsScreen = 0;
volatile uint8_t game = 0;

volatile uint8_t seconds = 0;
volatile uint8_t note = 0;
volatile uint8_t alerts = 0;
volatile uint8_t drawAlert = 1;
volatile uint8_t hungerAlert = 0;
volatile uint8_t happinessAlert = 1;

volatile uint8_t petPet = 0;
volatile uint8_t sending = 0;

volatile uint8_t sleeping = 0;
volatile uint8_t sleepTimer = 4;

//uint16_t lostwoods[] = {4771,3787,3374,3374,4771,3787,3374,3374,4771,3787,3374,2527,2837,2837,3374,3184,3374,4251,5055,5055,5055,5055,5055,5674,5055,4251,5055,5055,5055,5055,5055,5055,4771,3787,3374,3374,4771,3787,3374,3374,4771,3787,3374,2527,2837,2837,3374,3184,2527,3374,4251,4251,4251,4251,4251,3374,4251,5674,5055,5055,5055,5055,5055,5055,5674,5055,4771,4771,4251,3787,3374,3374,3184,3374,5055,5055,5055,5055,5055,5055,5674,5055,4771,4771,4251,3787,3374,3374,3184,2837,2527,2527,2527,2527,2527,2527,5674,5055,4771,4771,4251,3787,3374,3374,3184,3374,5055,5055,5055,5055,5055,5055,5055,5674,4771,5055,4251,4771,3787,4251,3374,3787,3184,3374,2837,3184,2527,2837,2527,2527,2527,2527,2527,0,2527,0,2527,0,2527,0,2527,0};
//notes = [d,e,f,g,a,b,c,d2,e2,f2]
//uint16_t notes[] = {5674, 5055, 4771, 4251, 3787, 3374, 3184, 2837, 2527, 2251};
//uint8_t lostwoods[] = {2,4,5,5,2,4,5,5,2,4,5,8,7,7,5,6,5,3,1,1,1,1,1,0,1,3,1,1,1,1,1,1,2,4,5,5,2,4,5,5,2,4,5,8,7,7,5,6,8,5,3,3,3,3,3,5,3,0,1,1,1,1,1,1,0,1,2,2,3,4,5,5,6,5,1,1,1,1,1,1,0,1,2,2,3,4,5,5,6,7,8,8,8,8,8,8,0,1,2,2,3,4,5,5,6,5,1,1,1,1,1,1,1,0,2,1,3,2,4,3,5,4,6,5,7,6,8,7,8,8,8,8,8,0,8,0,8,0,8,0,8,0};


void initTCA0WAVE() {
    TCA0.SINGLE.CTRLESET = TCA_SINGLE_CMD_RESET_gc;     //reset peripheral
    
    PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTC_gc;          //send to port C
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_FRQ_gc;
    TCA0.SINGLE.EVCTRL &= ~(TCA_SINGLE_CNTEI_bm);
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc;

    TCA0.SINGLE.CMP0 = 0;
}


//ISR for potentiometer, updates screen contrast
ISR(ADC0_RESRDY_vect) {
    if (ADC0.INTFLAGS & ADC_RESRDY_bm) {
        uint8_t contrast = ADC0.RES * (255.0 / 1023.0);
        setContrast(contrast);
        ADC0.INTFLAGS = ADC_RESRDY_bm;
    }
}


//ADC initialization for potentiometer to set power level
void initADC() {    
    PORTD.PIN6CTRL &= ~PORT_ISC_gm;     //disable digital input buffer
    PORTD.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc;   
    PORTD.PIN6CTRL &= ~PORT_PULLUPEN_bm;    //disable pull-up resistor

    
    ADC0.CTRLC |= ADC_PRESC_DIV4_gc | ADC_REFSEL_VDDREF_gc;     //ADC internal reference voltage with prescale of 4
    ADC0.CTRLA |= ADC_FREERUN_bm | ADC_RESSEL_10BIT_gc; //freerun, 10 bit resolution
    ADC0.INTCTRL |= ADC_RESRDY_bm;  //set ADC peripheral to fire interrupt once a conversion is ready
    ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc;   //set pin D6 as input source for ADC
    
//    ADC0.CTRLA |= ADC_ENABLE_bm;    //enable adc
//    ADC0.COMMAND |= ADC_STCONV_bm;   //start adc conversion
}




int main(void) {
   
    initTWI();  
    initializeRTC();
    usartInit();
    bleInit("ashton");
    USART0.CTRLA |= USART_RXCIE_bm;     //enable USART interrupts after BLE initialization
    initTCA0WAVE();
    
    initADC();

    
    //buttons
    PORTA.DIR &= ~PIN4_bm;  //green
    PORTA.DIR &= ~PIN5_bm;  //blue
    PORTA.DIR &= ~PIN6_bm;  //yellow
    PORTA.PIN4CTRL |= PORT_PULLUPEN_bm | PORT_ISC_RISING_gc;    //green
    PORTA.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_RISING_gc;    //blue
    PORTA.PIN6CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;    //yellow
    
    
    
    //speaker to output
    PORTC.DIR |= PIN0_bm;

    
    _delay_ms(2000);    //give screen a second to boot up
        
    initScreen();
    clearAll();
    setColumnAddress(0, 63);
    setPageAddress(0, 5);
    
    writeTopBar();
    drawLight(1);
    
    
    sei();
    
    
    while (1){
    }
}

//https://musescore.com/usocolo123/lost-woods---violin-solo#comments
//[f,a,b,b,f,a,b,b]
//[f,a,b,e2,d2,d2,b,c] 
//[b,g,e,e,e,e,e,d]
//[e,g,e,e,e,e,e,e]
//
//[f,a,b,b,f,a,b,b]
//[f,a,b,e2,d2,d2,b,c]
//[e2,b,g,g,g,g,g,b]
//[g,d,e,e,e,e,e,e]
//
//[d,e,f,f,g,a,b,b]
//[c,b,e,e,e,e,e,e]
//[d,e,f,f,g,a,b,b]
//[c,d2,e2,e2,e2,e2,e2,e2]
//
//[d,e,f,f,g,a,b,b]
//[c,b,e,e,e,e,e,e]
//[e,d,f,e,g,f,a,g]
//[b,a,c,b,d2,c,e2,f2]
//
//[e2,e2,e2,e2,e2,e2,e2,e2]