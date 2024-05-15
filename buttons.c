/*
 * File:   buttons.c
 * Author: ashtonberg
 *
 */

#ifndef F_CPU
#define F_CPU 3333333
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "globals.h"
#include "shapes.h"
#include "i2c.h"

#define BLE_RADIO_PROMPT "CMD> "



volatile uint8_t guesses = 3;
volatile uint8_t correctGuesses = 0;
volatile uint8_t waitGame = 0;

volatile uint8_t adcOn = 0;

//ISR for all buttons
ISR(PORTA_PORT_vect) {
    if(PORTA.INTFLAGS & PIN4_bm) {  //green button
        if(homeScreen) {
            if(cursorPos > 0) {
                eraseCursor(cursorPos);
            }
            cursorPos += 1;

            if(cursorPos > 4) {
                cursorPos = 0;
            }
            else {
                drawCursor(cursorPos);
            }
        }
        else if(game) {
            int correct = rand() % 2;
            if(correct == 0) {
                //turn duck left
                //draw heart
                //increment top tally
                draw(duck, 28, 25, 38, 3, 5);
                correctGuesses += 1;
                draw(heart, 7, 29, 63, 2, 5);
            }
            else {
                //turn duck right
                //draw X
                drawReverse(duck, 28, 2, 25, 38, 3, 5);
                draw(badX, 7, 29, 63, 2, 5);

            }
            guesses -= 1;
            PORTA.PIN4CTRL &= ~PORT_ISC_RISING_gc;
            PORTA.PIN5CTRL &= ~PORT_ISC_RISING_gc;
            waitGame = 2;
            
        }

        PORTA.INTFLAGS = PIN4_bm;
    }
    else if(PORTA.INTFLAGS & PIN5_bm) { //blue button
        
        if(homeScreen) {
            if(cursorPos == 1) {    //feed
                feeding = 4;
                //draw(43, 95, 3, 5, 2);          //draw apple
                draw(apple, 7, 8, 63, 3, 5);
                //disable buttons
                PORTA.PIN4CTRL &= ~PORT_ISC_RISING_gc;
                PORTA.PIN5CTRL &= ~PORT_ISC_RISING_gc;
            }
            else if(cursorPos == 2) {   //light control
                if(lightOn) {
                    lightOn = 0;
                    erase(51, 63, 3, 5, 12);
                    drawLight(0);
                }
                else{
                    lightOn = 1;
                    drawLight(1);
                }
            }
            else if(cursorPos == 3) {
                //stats
                clearMain();
                drawStatsScreen();
                homeScreen = 0;
                statsScreen = 1;
                cursorPos = 0;
            }
            else if(cursorPos == 4) {
                //game
                if(sleeping == 0) {
                    homeScreen = 0;
                    game = 1;
                    cursorPos = 0;
                    RTC.INTCTRL |=  RTC_CMP_bm;
                    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
                    //TCB0.CTRLA |= TCB_ENABLE_bm;
                    clearMain();
                    //draw game screen
                    erase(25, 38, 3, 5, 28);
                    setupGame();
                }
            }
        }
        else if(statsScreen) {
            clearMain();
            statsScreen = 0;
            homeScreen = 1;
            writeTopBar();
            drawLight(lightOn);
            draw(duck, 28, 20, 33, 3, 5);
            if(sleeping) {
                draw(z, 11, 29, 63, 2, 5);
            }
            if(hungerAlert || happinessAlert) {
                draw(attentionCircle, 18, 53, 61, 4, 5);
            }
        }
        else if(game) {
            int correct = rand() % 2;
            if(correct == 1) {
                //turn duck right
                //draw heart
                //increment top tally
                drawReverse(duck, 28, 2, 25, 38, 3, 5);
                draw(heart, 7, 29, 63, 2, 5);
                correctGuesses += 1;
            }
            else {
                //turn duck left
                //draw X
                draw(duck, 28, 25, 38, 3, 5);
                draw(badX, 7, 29, 63, 2, 5);
            }
            guesses -= 1;
            PORTA.PIN4CTRL &= ~PORT_ISC_RISING_gc;
            PORTA.PIN5CTRL &= ~PORT_ISC_RISING_gc;
            waitGame = 2;
           
        }
        PORTA.INTFLAGS = PIN5_bm;
    }
    else if(PORTA.INTFLAGS & PIN6_bm) {
        if(PORTA.IN & PIN6_bm) {
            ADC0.CTRLA &= ~ADC_ENABLE_bm;   //disable adc
        }
        else {
            ADC0.CTRLA &= ~ADC_ENABLE_bm;   //disable adc
            ADC0.CTRLA |= ADC_ENABLE_bm;    //enable adc
            ADC0.COMMAND |= ADC_STCONV_bm;  //start conversion
        }
        
        PORTA.INTFLAGS = PIN6_bm;
    }
}
