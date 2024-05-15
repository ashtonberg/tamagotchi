
#ifndef I2C_H
#define	I2C_H

#include <xc.h> // include processor files - each processor file is guarded.  

void initTWI();
void entireDisplayOn();
void setAddressingMode(uint8_t mode);
void setColumnAddress(uint8_t start, uint8_t end);
void setPageAddress(uint8_t start, uint8_t end);
void writeTopBar();
void initScreen();
void clearAll();
void drawCursor(uint8_t pos);
void eraseCursor(uint8_t pos);
void draw(uint8_t *shape, uint8_t length, uint8_t startCol, uint8_t endCol, uint8_t startPage, uint8_t endPage);
void drawReverse(uint8_t *shape, uint8_t length, uint8_t split, uint8_t startCol, uint8_t endCol, uint8_t startPage, uint8_t endPage);
void erase(uint8_t startCol, uint8_t endCol, uint8_t startPage, uint8_t endPage, uint8_t len);
void drawLight(uint8_t on);
void drawStatsScreen();
void clearMain();
void setupGame();
void addGameTally(uint8_t startCol);
void upShift(uint8_t *shape, uint8_t length, uint8_t split, uint8_t startCol, uint8_t endCol, uint8_t startPage, uint8_t endPage);
void setContrast(uint8_t contrast);

#endif	/* I2C_H */

