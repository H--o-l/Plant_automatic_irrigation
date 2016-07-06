/*============================ Include ============================*/

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>

/*============================ Macro ============================*/

#define msleep(X) delay(X)

#define DEBUG
#ifndef DEBUG
#define PRINTF(...);
#define setupSerial();
#else
#define setupSerial(); {Serial.begin(57600); printf_begin();}
#define PRINTF printf
#endif

/*============================ Global variable ============================*/

// Set up nRF24L01 radio on SPI pin for CE, CSN
RF24 radio(9,10);
#define MOISTURE_SENSOR_PIN A0

/*============================ Local Function interface ============================*/

static void setupNRF24(void);
static void send(char * iData);
static void received(char* oData, long iMsTimeOut);
static int getMoistureValue(void);

/*============================ Function implementation ============================*/

/*------------------------------- Main functions -------------------------------*/

void setup(void){
  setupSerial();
  PRINTF("Basic nrF24\n\r");
  setupNRF24();
}

void loop(void){
  char lInBuffer[32];
  char lOutBuffer[32];

  printf("moisture=%d\n", getMoistureValue());
  // 
  // radio.powerUp();
  // received(lInBuffer, -1);
  // if(strcmp(lInBuffer, "1") == 0){
  //   sprintf(lOutBuffer,"%d",12);
  //   send((char*)lOutBuffer);
  // }
  // radio.powerDown();
  
}

/*------------------------------- nRF24 -------------------------------*/

static void setupNRF24(void){
  radio.begin();
  radio.enableDynamicPayloads();
  radio.setRetries(20,10);
  radio.openWritingPipe(0xF0F0F0F0E2LL);
  radio.openReadingPipe(1,0x7365727632LL);
  radio.startListening();
  radio.printDetails();
  msleep(100);
  radio.powerDown(); 
}

static void send(char* iData){
  if(strlen(iData) <= 32){
    radio.stopListening();
    if(!radio.write(iData, strlen(iData))){
      PRINTF("Radio.write failed : %s\n\r", iData);
    }
    radio.startListening();
  }else{
    PRINTF("Data to long  : %s\n\r", iData);
  }
}

static void received(char* oData, long iMsTimeOut){
  long lTimeReference;
  long lTime;

  lTimeReference = millis();
  lTime = lTimeReference;

  //msleep(iMsTimeOut); /* TODO */
  while(  ((lTime - lTimeReference) < iMsTimeOut)
        ||(iMsTimeOut == -1)){
    if(radio.available()){
      uint8_t len = radio.getDynamicPayloadSize();
      if(len <= 32){
        radio.read(oData, len);
        oData[len] = 0;
      }else{
        PRINTF("Error in received function\n\r");
        oData[0] = 0;
      }
      return;
    }
    lTime = millis(); 
  }
  //PRINTF("Received time out %10ld %10ld %10ld\n\r", lTimeReference, lTime, lTime - lTimeReference);
  oData[0] = 0;
}

/*------------------------------- Moisture sensor -------------------------------*/

static int getMoistureValue(){
  return analogRead(MOISTURE_SENSOR_PIN);
}
