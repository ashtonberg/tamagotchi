#ifndef GLOBALS_H
#define GLOBALS_H

#include <xc.h> // include processor files - each processor file is guarded.  

extern volatile uint8_t feeding;
extern volatile uint8_t cursorPos;
extern volatile uint8_t homeScreen;
extern volatile uint8_t lightOn;
extern volatile uint8_t sleeping;
extern volatile uint8_t hunger;
extern volatile int8_t happiness;
extern volatile uint8_t level;
extern volatile uint8_t statsScreen;
extern volatile uint8_t game;
extern volatile uint8_t guesses;
extern volatile uint8_t correctGuesses;
extern volatile uint8_t waitGame;

extern volatile uint8_t seconds;
extern volatile uint8_t note;
extern volatile uint8_t alerts;
extern volatile uint8_t drawAlert;
extern volatile uint8_t hungerAlert;
extern volatile uint8_t happinessAlert;
extern volatile uint8_t sleeping;
extern volatile uint8_t sleepTimer;
extern volatile uint8_t bruh;

extern volatile uint8_t petPet;
extern volatile uint8_t sending;


//extern uint16_t lostwoods[];
//extern uint8_t notes[];


#endif