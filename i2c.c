

#ifndef F_CPU
#define F_CPU 3333333
#endif


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "globals.h"
#include "shapes.h"

uint8_t topBar[] = {255,1,1,25,33,249,33,25,1,1,25,249,1,1,1,255,1,131,69,65,25,37,195,195,37,25,1,69,131,1,1,255,1,249,137,233,137,201,137,233,137,201,137,233,137,249,1,255,1,1,1,1,13,13,129,97,113,57,29,13,1,1,1,255};
uint8_t cursor[] = {34,50,250,50,34};
uint8_t attentionCircle[] = {0,192,64,32,160,32,64,192,0,7,24,16,32,43,32,16,24,7};
uint8_t apple[] = {120,204,132,134,133,204,120};
uint8_t heart[] = {12,18,34,68,34,18,12};
uint8_t badX[] = {65,34,20,8,20,34,65};
uint8_t light[] = {0,65,34,0,12,30,127,97,18,12};   //0 is buffer value, next 6 are on, last 3 are off
uint8_t stats[] = {6,8,62,8,6,0,6,62,0,0,127,65,28,34,89,85,81,85,89,34,28,0,0,127,65,30,16,0,14,16,14,0,30,16,0,0,127,65};
uint8_t forwardDuck[] = {0,0,0,240,8,148,68,68,68,148,8,240,0,0,0,0,7,24,96,128,97,33,97,128,96,24,7,0};
uint8_t duck[] = {160,80,80,80,8,20,4,4,4,20,8,16,224,0,0,1,1,15,16,96,128,96,38,104,134,96,17,14};
uint8_t gameRound[] = {124,20,20,104,0,112,80,112,0,112,64,112,0,112,16,112,0,112,80,124,0,0,0,124};
uint8_t z[] = {200,168,152,0,100,84,76,0,50,42,38};
uint8_t dead[] = {62,10,14,0,62,42,34,0,2,62,2,0,0,0,62,34,28,0,34,62,34,0,62,42,34,0,62,34,28};


//Qwiic Cable
//Black = GND
//Red = 3.3V
//Blue = SDA
//Yellow = SCL

//Control Byte
// Co D/C 0 0 0 0 0 0
//if Co is 0, another control byte will not be expected
//if Co is 1, another control byte will b e expected
//if D/C is 0, following transmission byte is a command
//if D/C is 1, following transmission byte is data

void initTWI() {
    // TODO Initialize the TWI peripheral
    
    //HOST
    PORTA.DIR |= PIN2_bm;       //sda
    PORTA.DIR |= PIN3_bm;       //scl
    PORTA.PIN2CTRL |= PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL |= PORT_PULLUPEN_bm;
    TWI0.MBAUD = 10;            //baud rate
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;          //bus to idle
    
    TWI0.DBGCTRL = TWI_DBGRUN_bm;
    
    TWI0.MCTRLA |= TWI_ENABLE_bm;           //enable
    
}


void entireDisplayOn() {
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte
    TWI0.MDATA = 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //send set display on command
    TWI0.MDATA = 0xA5; //0xA5;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}



void setAddressingMode(uint8_t mode) {
    // 0 = horizontal
    // 1 = vertical
    // 2 = page 
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    //control byte for command
    TWI0.MDATA = 0;  //0b10000000
    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    //send page addressing mode command
    TWI0.MDATA = 0x20; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = mode;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    
}

void setColumnAddress(uint8_t start, uint8_t end) {
     //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    //control byte for command
    TWI0.MDATA = 0;  
    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    //send set column address command
    TWI0.MDATA = 0x21; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = start+32;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = end+32;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}

void setPageAddress(uint8_t start, uint8_t end) {
     //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    //control byte for command
    TWI0.MDATA = 0;  
    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    //send set page address command 
    TWI0.MDATA = 0x22; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = start;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = end;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}

void writeTopBar() {
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    //control byte for all data
    TWI0.MDATA = 64; //0b01000000
    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    for(uint8_t x = 0; x < 64; x++) {
        TWI0.MDATA = topBar[x];
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    TWI0.MDATA = 3;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    for(uint8_t y = 0; y < 14; y++) {
        TWI0.MDATA = 2;
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    TWI0.MDATA = 3;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    for(uint8_t y = 0; y < 15; y++) {
        TWI0.MDATA = 2;
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    TWI0.MDATA = 3;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    for(uint8_t y = 0; y < 15; y++) {
        TWI0.MDATA = 2;
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    TWI0.MDATA = 3;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    for(uint8_t y = 0; y < 15; y++) {
        TWI0.MDATA = 2;
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    TWI0.MDATA = 3;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}



void initScreen() {
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    //control byte for command
    TWI0.MDATA = 0;  
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //set display clock div
    TWI0.MDATA = 0xD5; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0x80;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //set multiplex
    TWI0.MDATA = 0xA8; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0x2F;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //set display offset
    TWI0.MDATA = 0xD3; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0x0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //set start line
    TWI0.MDATA = 0b01000000; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
        
    //charge pump
    TWI0.MDATA = 0x8D; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0x14;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //normal display
    TWI0.MDATA = 0xA6; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //display all on resume
    TWI0.MDATA = 0xA4; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
        
    //set segment re-map
    TWI0.MDATA = 0xA0; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //set com scan direction
    TWI0.MDATA = 0xC0; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //set com pins hardware configuration
    TWI0.MDATA = 0xDA; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0x12; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //set contrast
    TWI0.MDATA = 0x81; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0x8F; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //set pre-charge
    TWI0.MDATA = 0xD9; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0xF1; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //set v com deselect
    TWI0.MDATA = 0xDB; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0x40; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //deactivate scroll
    TWI0.MDATA = 0x2E;  //0b10000000
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //display on
    TWI0.MDATA = 0xAF; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}



void clearAll() {
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for command
    TWI0.MDATA = 0;  
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //horizontal addressing
    TWI0.MDATA = 0x20; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //column start address
    TWI0.MDATA = 0x21;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 127;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //page start address
    TWI0.MDATA = 0x22;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 7;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 0b01000000;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //send all 0s
    for(uint8_t y = 0; y < 8; y++){
        for(uint8_t x = 0; x < 128; x++) {
            TWI0.MDATA = 0;
            while (!(TWI0.MSTATUS & TWI_WIF_bm));
        }
    }
}


void drawCursor(uint8_t pos) {
    setPageAddress(1, 5);
    setColumnAddress((pos-1) * 16 + 5, 63);
    
    
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 64;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    for(uint8_t x = 0; x < 5; x++) {
        TWI0.MDATA = cursor[x];
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}

void eraseCursor(uint8_t pos) {
    setPageAddress(1, 5);
    setColumnAddress((pos-1)* 16 + 5, 63);
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 64;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    for(uint8_t x = 0; x < 5; x++) {
        TWI0.MDATA = 2;
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}



void draw(uint8_t *shape, uint8_t length, uint8_t startCol, uint8_t endCol, uint8_t startPage, uint8_t endPage) {
    setPageAddress(startPage, endPage);
    setColumnAddress(startCol, endCol);
    
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 64;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    for(uint8_t x = 0; x < length; x++) {
        TWI0.MDATA = shape[x];
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}

void drawReverse(uint8_t *shape, uint8_t length, uint8_t split, uint8_t startCol, uint8_t endCol, uint8_t startPage, uint8_t endPage) {
    setPageAddress(startPage, endPage);
    setColumnAddress(startCol, endCol);
    
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 64;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    
    uint8_t splitLen = length / split;      
    uint8_t curSplit = 0;                   
    for(uint8_t x = 0; x < split; x++) {
        uint8_t offset = curSplit * splitLen;   
        for(uint8_t y = splitLen - 1 + offset ; y > offset; y--) {
            TWI0.MDATA = shape[y];
            while (!(TWI0.MSTATUS & TWI_WIF_bm));
        }
        TWI0.MDATA = shape[offset];
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
        curSplit += 1;
    }
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}

void erase(uint8_t startCol, uint8_t endCol, uint8_t startPage, uint8_t endPage, uint8_t len) {
    setPageAddress(startPage, endPage);
    setColumnAddress(startCol, endCol);
    
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 64;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //send 0 bytes
    for(uint8_t x = 0; x < len; x++) {
            TWI0.MDATA = 0;
            while (!(TWI0.MSTATUS & TWI_WIF_bm));
        }
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}

void drawLight(uint8_t on) {
    setPageAddress(3, 5);
    if(on) {
        setColumnAddress(51, 63);   
    }
    else {
        setColumnAddress(54, 63);  
    }
    
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 64;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    if(on) {
        for(uint8_t x = 1; x < 7; x++) {
            TWI0.MDATA = light[x];
            while (!(TWI0.MSTATUS & TWI_WIF_bm));
        }
        for(uint8_t y = 6; y > 0; y--) {
            TWI0.MDATA = light[y];
            while (!(TWI0.MSTATUS & TWI_WIF_bm));
        }
    }
    else {
        for(uint8_t x = 9; x > 6; x--) {
            TWI0.MDATA = light[x];
            while (!(TWI0.MSTATUS & TWI_WIF_bm));
        }
        for(uint8_t y = 7; y < 10; y++) {
            TWI0.MDATA = light[y];
            while (!(TWI0.MSTATUS & TWI_WIF_bm));
        }
    }
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}

void drawStatsScreen() {
    
    //hunger
    setPageAddress(1, 5);
    setColumnAddress(4, 63);
    
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 64;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    for(uint8_t x = 0; x < 12; x++) {
        TWI0.MDATA = stats[x];
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    for(uint8_t y = 0; y < hunger; y++) {
        TWI0.MDATA = 93;   //01011101
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
        TWI0.MDATA = 93;
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    for(uint8_t z = hunger*2; z < 41; z++) {
        TWI0.MDATA = 65;
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    TWI0.MDATA = 127;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    
    
    //happiness
    setPageAddress(3, 5);
    setColumnAddress(3, 63);
    
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 64;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    for(uint8_t x = 12; x < 25; x++) {
        TWI0.MDATA = stats[x];
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    for(uint8_t y = 0; y < happiness; y++) {
        TWI0.MDATA = 93;
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
        TWI0.MDATA = 93;
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    for(uint8_t z = happiness*2; z < 41; z++) {
        TWI0.MDATA = 65;
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    TWI0.MDATA = 127;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    
    
    //level
    setPageAddress(5, 5);
    setColumnAddress(3, 63);
    
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 64;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    for(uint8_t x = 25; x < 38; x++) {
        TWI0.MDATA = stats[x];
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    for(uint8_t y = 0; y < level; y++) {
        TWI0.MDATA = 93;
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    for(uint8_t z = level; z < 41; z++) {
        TWI0.MDATA = 65;
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    TWI0.MDATA = 127;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}


void clearMain() {
    setPageAddress(0, 5);
    setColumnAddress(0, 63);
    
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 64;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    for(uint8_t x = 0; x < 6; x++) {
        for(uint8_t y = 0; y < 64; y++) {
            TWI0.MDATA = 0;
            while (!(TWI0.MSTATUS & TWI_WIF_bm));
        }
    }
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}

void setupGame() {
    
    //Game round and points
    setPageAddress(0, 5);
    setColumnAddress(19, 63);  
    
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 64;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    for(uint8_t x = 0; x < 24; x++) {
        TWI0.MDATA = gameRound[x];
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    
    
    //A and B
    setPageAddress(2, 5);
    setColumnAddress(9, 63);
    
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 64;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    TWI0.MDATA = 0b00111100; //A 1
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0b00001010; //A 2
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0b00001010; //A 3
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0b00111100; //A 4
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    
    setColumnAddress(50, 63);
    
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 64;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    TWI0.MDATA = 0b00111110; //B 1
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0b00101010; //B 2
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0b00101010; //B 3
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = 0b00010100; //B 4
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    
    
    
    //Forward Duck
    setPageAddress(3, 5);
    setColumnAddress(25, 38);
    
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 64;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    for(uint8_t x = 0; x < 28; x++) {
        TWI0.MDATA = forwardDuck[x];
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    
}

void addGameTally(uint8_t startCol) {
    
    setPageAddress(0, 5);
    setColumnAddress(startCol, 63);  
    
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 64;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    TWI0.MDATA = 0b01111100; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}

void upShift(uint8_t *shape, uint8_t length, uint8_t split, uint8_t startCol, uint8_t endCol, uint8_t startPage, uint8_t endPage) {
    setPageAddress(startPage, endPage);
    setColumnAddress(startCol, endCol);  
    
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for data
    TWI0.MDATA = 64;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    
    uint8_t splitLen = length / split;      //28 / 2 = 14
    uint8_t curSplit = 0;                   
    
    for(uint8_t x = 0; x < split; x++) {
        uint8_t offset = curSplit * splitLen;
        for(uint8_t y = offset; y < offset + splitLen; y++) {
            if(x < (split - 1)) {
                if(shape[y + splitLen] & 1) {
                    TWI0.MDATA = shape[y] >> 1 | 128;
                }
                else {
                    TWI0.MDATA = shape[y] >> 1;
                }
                while (!(TWI0.MSTATUS & TWI_WIF_bm));
            }
            else {
                TWI0.MDATA = shape[y] >> 1;
                while (!(TWI0.MSTATUS & TWI_WIF_bm));
            }
        }
        curSplit += 1;
    }
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}

void setContrast(uint8_t contrast) {
    //initiate transaction, wait for acknowledge
    TWI0.MADDR = (0x3D << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //control byte for command
    TWI0.MDATA = 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //set contrast
    TWI0.MDATA = 0x81; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    TWI0.MDATA = contrast; 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    //end transaction
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}


/* Stats
    01100000 00010000 01111100 00010000 01100000 00000000 01100000 01111100 00000000 00000000 11111110 10000010
    00111000 01000100 10011010 10101010 10001010 10101010 10011010 01000100 00111000 00000000 00000000 11111110 10000010
    01111000 00001000 00000000 01110000 00001000 01110000 00000000 01111000 00001000 00000000 00000000 11111110 10000010
  
 uint8_t stats[] = {6,8,62,8,6,0,6,62,0,0,127,65,28,34,89,85,81,85,89,34,28,0,0,127,65,30,16,0,14,16,14,0,30,16,0,0,127,65};
 len 38
*/


/* Top bar, doesn't include bottom border
 11111111 10000000 10000000 10011000 10000100 10011111 10000100 10011000 10000000 10000000 10011000 10011111 10000000 10000000 10000000 11111111
 10000000 11000001 10100010 10000010 10011000 10100100 11000011 11000011 10100100 10011000 10000000 10100010 11000001 10000000 10000000 11111111
 10000000 10011111 10010001 10010111 10010001 10010011 10010001 10010111 10010001 10010011 10010001 10010111 10010001 10011111 10000000 11111111
 10000000 10000000 10000000 10000000 10110000 10110000 10000001 10000110 10001110 10011100 10111000 10110000 10000000 10000000 10000000 11111111

 page 0
 */
//uint8_t topBar[] = {255,1,1,25,33,249,33,25,1,1,25,249,1,1,1,255,1,131,69,65,25,37,195,195,37,25,1,69,131,1,1,255,1,249,137,233,137,201,137,233,137,201,137,233,137,249,1,255,1,1,1,1,13,13,129,97,113,57,29,13,1,1,1,255};

/*
 top bar bottom border, cursor not included 
 10000000 X 64
*/

/*
 Attention circle 
  00000000 00000011 00000010 00000100 00000101 00000100 00000010 00000011 00000000
  11100000 00011000 00001000 00000100 11010100 00000100 00001000 00011000 11100000
 
 page 4
 col 53-61
*/
//uint8_t attentionCircle[] = {0,192,64,32,160,32,64,192,0,7,24,16,32,43,32,16,24,7};


/*
 Cursor
  01000100 010011000 01011111 01001100 01000100 
*/
//uint8_t cursor[] = {34,50,250,50,34};


/*
 Apple
    00011110 00110011 00100001 01100001 10100001 00110011 00011110
    
 uint8_t apple[] = {120,204,132,134,133,204,120};
 cols: 11-17
 page: 3
*/

/*
 BadX
    10000010 01000100 00101000 00010000 00101000 01000100 10000010
 
 uint8_t badX[] = {65,34,20,8,20,34,65};
 cols: 38-44
 page: 3
*/

/*
 Heart
    00110000 01001000 01000100 00100010 01000100 01001000 00110000
 
 uint8_t heart[] = {12,18,34,68,34,18,12};
 cols: 38-44
 page: 3
 * 
*/

/*
 Light                                              x
    10000010 01000100 00000000 00110000 01111000 11111110 //first 6 light on
    10000110 01001000 00110000 //last 3 light off
  
 uint8_t light[] = {65,34,0,12,30,127,97,18,12}
 length 9
 cols: 51-62
 page: 2
  
*/


/* Forward Duck
    00000000 00000000 00000000 00001111 00010000 00101001 00100010 00100010 00100010 00101001 00010000 00001111 00000000 00000000
    00000000 00000000 11100000 00011000 00000110 00000001 10000110 10000100 10000110 00000001 00000110 00011000 11100000 00000000
 
 len 28
 uint8_t forwardDuck[] = {0,0,0,240,8,148,68,68,68,148,8,240,0,0,0,0,7,24,96,128,97,33,97,128,96,24,7,0};
*/

/* Regular Duck
    00000101 00001010 00001010 00001010 00010000 00101000 00100000 00100000 00100000 00101000 00010000 00001000 00000111 00000000
    00000000 10000000 10000000 11110000 00001000 00000110 00000001 00000110 01100100 00010110 01100001 00000110 10001000 01110000
   
 uint8_t duck[] = {160,80,80,80,8,20,4,4,4,20,8,16,224,0,0,1,1,15,16,96,128,96,38,104,134,96,17,14};
 length 28
*/

/* Round
    00111110 00101000 00101000 00010110 00000000 00001110 00001010 00001110 00000000 00001110 00000010 00001110 00000000 00001110 00001000 00001110 00000000 00001110 00001010 00111110 00000000 00000000 00000000 00111110
 
 uint8_t gameRound[] = {124,20,20,104,0,112,80,112,0,112,64,112,0,112,16,112,0,112,80,124,0,0,0,124};
 
 length 24
 page 0
 col: 19-42
*/

/* ZZZ
    00010011 00010101 00011001 00000000 00100110 00101010 00110010 00000000 01001100 01010100 01100100
    
 page 2
 length 11
 col: 29-39
 
 */

/* Died
    01111100 01010000 01110000 00000000 01111100 01010100 01000100 00000000 01000000 01111100 01000000 
    00000000 00000000 00000000
    01111100 01000100 00111000 00000000 01000100 01111100 01000100 00000000 01111100 01010100 01000100 00000000 01111100 01000100 00111000
 
  uint8_t dead[] = {62,10,14,0,62,42,34,0,2,62,2,0,0,0,62,34,28,0,34,62,34,0,62,42,34,0,62,34,28};
  length 29
  col 19
 */