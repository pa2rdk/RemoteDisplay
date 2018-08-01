#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2018-04-29 16:06:38

#include "Arduino.h"
#include <TFT_ST7735.h>
#include <SPI.h>
#include <wire.h>
#include <EEPROM.h>

void drawGPSBox() ;
void initScreen() ;
void setup(void) ;
void requestEvent() ;
void receiveEvent() ;
void processData();
void printFreqInfo() ;
void loop();
void loadRepeater(int i) ;
void isr_2();
void isr_3();
void saveConfig() ;
void loadConfig() ;

#include "RemoteDisplay.ino"


#endif
