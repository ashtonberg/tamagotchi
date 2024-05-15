#ifndef F_CPU
#define F_CPU 3333333
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <stdio.h>
#include <stdlib.h>


#include "globals.h"
#include "shapes.h"
#include "i2c.h"
#include "ble.h"

#define BUF_SIZE 128
#define BLE_RADIO_PROMPT "CMD> "

uint16_t notes[] = {5674, 5055, 4771, 4251, 3787, 3374, 3184, 2837, 2527, 2251};
uint8_t lostwoods[] = {2,4,5,5,2,4,5,5,2,4,5,8,7,7,5,6,5,3,1,1,1,1,1,0,1,3,1,1,1,1,1,1,2,4,5,5,2,4,5,5,2,4,5,8,7,7,5,6,8,5,3,3,3,3,3,5,3,0,1,1,1,1,1,1,0,1,2,2,3,4,5,5,6,5,1,1,1,1,1,1,0,1,2,2,3,4,5,5,6,7,8,8,8,8,8,8,0,1,2,2,3,4,5,5,6,5,1,1,1,1,1,1,1,0,2,1,3,2,4,3,5,4,6,5,7,6,8,7,8,8,8,8,8,0,8,0,8,0,8,0,8,0};

//initialize RTC to keep track of time using internal 32kHz oscillator
//overflow (OVF) of .5 seconds
void initializeRTC() {
    //ccp_write_io() is used to write to a protected register
    
    //CONFIGURE CLOCK CLK_RTC
    
    //Make sure oscillator is disabled before configuration
    uint8_t temp;
    temp = CLKCTRL.OSC32KCTRLA;
    temp &= ~CLKCTRL_ENABLE_bm;
    ccp_write_io((void*)&CLKCTRL.OSC32KCTRLA, temp);
    
    //wait for status and OSC32KS to be zero
    while(CLKCTRL.MCLKSTATUS & CLKCTRL_OSC32KS_bm) {} 
    
    /* Oscillator clock selection */
    temp = CLKCTRL.OSC32KCTRLA;
    temp &= ~CLKCTRL_SEL_bm;
    ccp_write_io((void*)&CLKCTRL.OSC32KCTRLA, temp);
    
    /* Enable oscillator: */
    temp = CLKCTRL.OSC32KCTRLA;
    //temp |= CLKCTRL_RUNSTDBY_bm | CLKCTRL_ENABLE_bm;
     temp |= CLKCTRL_ENABLE_bm;
    ccp_write_io((void*)&CLKCTRL.OSC32KCTRLA, temp);
    
    //wait for registers to be synchronized
    while (RTC.STATUS > 0){}
    
    
    //CONFIGURE RTC
    
    RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;  //set internal 32kHz oscillator
    
    RTC.PER = 500;                 //set overflow value, 1 second
    RTC.CMP = 250;                  //set compare value, .25 seconds
    RTC.INTCTRL |=  RTC_OVF_bm;     //enable overflow interrupt
    
    
    //pre-scale, enable run in standby, enable
    RTC.CTRLA |= RTC_PRESCALER_DIV32_gc | RTC_RTCEN_bm;      

    
}



volatile uint8_t up = 0;

ISR(RTC_CNT_vect) {
    //overflow
    if (RTC.INTFLAGS & RTC_OVF_bm) {
        if(seconds == 120) {
            hunger -= 1;
            if(hunger < 11) {
                if(hunger == 10) {
                    hungerAlert = 1;
                    drawAlert = 1;
                    sending = 1;
                    usartWriteCommand("SHW,0072,01\r\n"); 
                }
                else if(hunger == 0) {
                    //dead
                    clearMain();
                    draw(dead, 29, 19, 63, 2, 5);
                    PORTA.PIN4CTRL &= ~PORT_ISC_RISING_gc;
                    PORTA.PIN5CTRL &= ~PORT_ISC_RISING_gc;
                    sending = 1;
                    usartWriteCommand("SHW,0072,03\r\n"); 
                    RTC.CTRLA &= ~RTC_RTCEN_bm;  
                }
                else {
                    happiness -= 1;
                    if(happiness <= 10 && happinessAlert == 0) {
                        happinessAlert = 1;
                        drawAlert = 1;
                        sending = 1;
                        usartWriteCommand("SHW,0072,02\r\n"); 
                        //usartReadUntil(buf,BLE_RADIO_PROMPT);
                    }
                }
            }
            
            level += 1;
            if(level == 40) {
                //evolve?
                level = 0;  //placeholder
            }
            
        
            
            if(sleeping) {
                if(lightOn) {
                    happiness -= 3;
                    if(happiness <= 10 && happinessAlert == 0) {
                        happinessAlert = 1;
                        drawAlert = 1;
                        sending = 1;
                        usartWriteCommand("SHW,0072,02\r\n");
                    }
                }
                sleepTimer = 0;
                sleeping = 0;
                lightOn = 1;
                drawLight(1);
                erase(29, 63, 2, 5, 11);
            }
            else {
                sleepTimer += 1;
                if(sleepTimer == 5) {   //falls asleep for one minute every 5 minutes, game disabled, feeding hurts happiness
                    sleeping = 1;
                    if(homeScreen) {
                        draw(z, 11, 29, 63, 2, 5);
                    }
                }
            }
            
            if(lightOn == 0) {
                happiness -= 1;
                if(happiness <= 10 && happinessAlert == 0) {
                        happinessAlert = 1;
                        draw(attentionCircle, 18, 53, 61, 4, 5);
                        sending = 1;
                        usartWriteCommand("SHW,0072,02\r\n"); 
                    }
            }
            
            
            seconds = 0;
        }
        
        if(petPet) {
                if(petPet == 5) {
                    draw(heart, 7, 4, 63, 5, 5);
                }
                else if(petPet == 1) {
                    erase(4, 63, 5, 5, 7);
                    happiness += 1;
                    if(happiness > 20) {
                        happiness = 20;
                    }
                    else if(happiness > 10 && happinessAlert) {
                        happinessAlert = 0;
                        if(hungerAlert == 0) {
                            erase(53, 61, 4, 5, 18);
                        }
                    }
                }
                petPet--;
        }
        
        
        if(homeScreen) {
            if(sleeping == 0) {
                if(up) {
                    draw(duck, 28, 20, 33, 3, 5);
                    up = 0;
                }
                else{
                    upShift(duck, 28, 2, 20, 33, 3, 5);
                    up = 1;
                }
            }
            if(drawAlert) {
                draw(attentionCircle, 18, 53, 61, 4, 5);
                drawAlert = 0;
            }
        }
        
        if(feeding) {
            if(feeding == 3) {
                erase(8, 63, 3, 5, 7);     //erase apple
                if(hunger == 20 || sleeping) {
                    //draw(70, 95, 3, 5, 3);       //draw badX
                    draw(badX, 7, 35, 63, 3, 5);
                    //subtract happiness
                    happiness -= 2;
                    if(happiness <= 10 && happinessAlert == 0) {
                        happinessAlert = 1;
                        drawAlert = 1;
                        sending = 1;
                        usartWriteCommand("SHW,0072,02\r\n"); 
                    }
                }
                else {
                    //draw(70, 95, 3, 5, 4);       //draw heart
                    draw(heart, 7, 35, 63, 3, 5);
                    hunger += 2;
                    if(hunger > 20) {
                        hunger = 20;
                    }
                    else if(hunger > 10 && hungerAlert) {
                        hungerAlert = 0;
                        if(happinessAlert == 0) {
                            erase(53, 61, 4, 5, 18);
                        }
                    }
                }
            }
            else if(feeding == 1) {
                erase(35, 63, 3, 5, 7);     //erase heart / badX
                PORTA.PIN4CTRL |= PORT_ISC_RISING_gc;
                PORTA.PIN5CTRL |= PORT_ISC_RISING_gc;
            }

            feeding -= 1;
        }
        else if(game) {
            uint8_t ind = lostwoods[note];
            TCA0.SINGLE.CMP0 = notes[ind];
            //TCA0.SINGLE.CMP0 = lostwoods[note];
            note++;
            if(note == 142) {
                note = 0;
            }
            if(waitGame) {
                waitGame -= 1;
                if(waitGame == 0) {                    
                    if(guesses == 0) {
                        happiness += correctGuesses * 2;
                        if(happiness > 20) {
                            happiness = 20;
                        }
                        else if(happiness > 10 && happinessAlert) {
                            happinessAlert = 0;
                            if(hungerAlert == 0) {
                                erase(53, 61, 4, 5, 18);
                            }
                        }
                        guesses = 3;
                        correctGuesses = 0;
                        game = 0;
                        note = 0;
                        TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
                        //TCB0.CTRLA &= ~ TCB_ENABLE_bm;
                        homeScreen = 1;
                        clearMain();
                        writeTopBar();
                        drawLight(lightOn);
                        //draw duck
                        draw(duck, 28, 20, 33, 3, 5);
                        if(hungerAlert || happinessAlert) {
                            draw(attentionCircle, 18, 53, 61, 4, 5);
                        }
                        if(sleeping) {
                            draw(z, 11, 29, 63, 2, 5);
                        }
                        RTC.INTCTRL &=  ~RTC_CMP_bm;
                    }
                    else {
                        //draw duck forward
                        draw(forwardDuck, 28, 25, 38, 3, 5);
                        addGameTally(48 - 2 * guesses);
                        erase(29,63, 2, 5, 7);
                    }
                    PORTA.PIN4CTRL |= PORT_ISC_RISING_gc;
                    PORTA.PIN5CTRL |= PORT_ISC_RISING_gc;
                    
                }
            }
            else {
                if(up) {
                    draw(forwardDuck, 28, 25, 38, 3, 5);
                    up = 0;
                }
                else {
                    upShift(forwardDuck, 28, 2, 25, 38, 3, 5);
                    up = 1;
                }
            }
        }
        
        if(happiness <= 0) {
            //dead
            clearMain();
            draw(dead, 29, 19, 63, 2, 5);
            PORTA.PIN4CTRL &= ~PORT_ISC_RISING_gc;
            PORTA.PIN5CTRL &= ~PORT_ISC_RISING_gc;
            sending = 1;
            usartWriteCommand("SHW,0072,03\r\n"); 
            //usartReadUntil(buf,BLE_RADIO_PROMPT);
            RTC.CTRLA &= ~RTC_RTCEN_bm;  
        }
        
        seconds++;
        
        //clear flag
        RTC.INTFLAGS = RTC_OVF_bm;
    }
    //compare
    else if (RTC.INTFLAGS & RTC_CMP_bm) {
        uint8_t ind = lostwoods[note];
        TCA0.SINGLE.CMP0 = notes[ind];
        //TCA0.SINGLE.CMP0 = lostwoods[note];
        note++;
        if(note == 142) {
                note = 0;
        }
        
        //clear flag
        RTC.INTFLAGS = RTC_CMP_bm;
        
    }
}


