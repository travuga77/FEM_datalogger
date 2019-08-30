/*
  SD card datalogger

  This example shows how to log data from three analog sensors
  to an SD card using the SD library.

  The circuit:
   analog sensors on analog ins 0, 1, and 2
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created  24 Nov 2010
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

//#define DEBUG


#include <SD.h>
#include <mcp_can.h>
#include <SPI.h>
#include <avr/wdt.h>


#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(9);                               // Set CS to pin 9
#define SD_CS_PIN 10
#define GREEN_LED_PIN   4
#define RED_LED_PIN     5
#define BUZZ_PIN        6
#define BUTTON_PIN      7
#define ERROR_MASK      1

File dataFile;
//unsigned int lReady, rReady;
//int batt_state;
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
//char msgString[128];                        // Array to store serial string
byte val = 0;
long previousMillis = 0;
bool job_flag=0, flagSD=1, name_flag=1;
String fileName;
int tr15=0, tr14=0, tr13=0, tr12=0, tr11=0, tr10=0, tr9=0, tr8=0, tr7=0, tr6=0, tr5=0, tr4=0, tr3=0, tr2=0, tr1=0, tr0=0;

int ReadBytesFrom(byte len, byte beg) {
  int canMsg;
  for ( int i = ( beg + len ); i > beg; i-- )
  {
    canMsg <<= 8;
    canMsg += rxBuf[ i - 1 ];
  }
  return canMsg;
}

void setup() {
  wdt_enable(WDTO_2S);
   File dataFile;

     dataFile.print("all digits you see between ':' char are readen bytes");
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Card failed, or not present");
    flagSD=0;
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input

  Serial.println("MCP2515 Library Receive Example...");
  
  pinMode(BUTTON_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, HIGH);
}

void loop() {  
  wdt_reset();
  if(digitalRead(BUTTON_PIN)==HIGH) {//если кнопка нажата ...
    if (millis()-previousMillis>50) {
       previousMillis = millis();    
       val++;
    }
  }
  else {
    val=0;
  }
  if(val>=5&&flagSD) {
    digitalWrite(BUZZ_PIN,1);
    delay(500);
    digitalWrite(BUZZ_PIN,0);
    digitalWrite(GREEN_LED_PIN,!digitalRead(GREEN_LED_PIN));
    digitalWrite(RED_LED_PIN,!digitalRead(RED_LED_PIN));
    if (job_flag==0) {
      job_flag=1;
      name_flag=1;
      unsigned int i = 1;
      
      while (name_flag) {
        fileName = String("data") + i + String(".csv");
        if (SD.exists(fileName)) {
          i++;
          continue;
        }
        else {
          dataFile = SD.open(fileName, FILE_WRITE);
          dataFile.println("Timestamp;SteerOut;PedalOut;alfa;send_motors;speed;speedf*100;speedr*100;speedRL;speedRR;slip*100;voltage_bms;current_acc_cont;current_left_motor;current_right_motor;racelogic_time;benchmark");
          Serial.println(fileName + " created!");
          name_flag=0;
        }
      }
    }
    else {
      job_flag=0;
      if (dataFile) { dataFile.close(); Serial.println("CLOSED");}
    }
    val=0;    
  }
 
  unsigned long timestamp;
  if (job_flag) {
    if (!digitalRead(CAN0_INT)) {                       // If CAN0_INT pin is low, read receive buffer
      CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s) 
    }
    switch (rxId) {
      case 0x134:
        tr0 = ReadBytesFrom(2,0);
        tr1 = ReadBytesFrom(2,2);
        tr2 = ReadBytesFrom(2,4);
        tr3 = ReadBytesFrom(2,6);
#ifdef DEBUG
        Serial.println("134 ok");
#endif
        rxId=0;
        break;      
      case 0x135:
        tr4 = ReadBytesFrom(2,0);
        tr5 = ReadBytesFrom(2,2);
        tr6 = ReadBytesFrom(2,4);
        tr7 = ReadBytesFrom(2,6);
#ifdef DEBUG
        Serial.println("135 ok");
#endif
        rxId=0;
        break;
      case 0x136:
        tr8 = ReadBytesFrom(2,0);
        tr9 = ReadBytesFrom(2,2);
        tr10 = ReadBytesFrom(2,4);
        tr11 = ReadBytesFrom(2,6);
#ifdef DEBUG
        Serial.println("136 ok");
#endif
        rxId=0;
        break;
      case 0x137:
        tr12 = ReadBytesFrom(2,0);
        tr13 = ReadBytesFrom(2,2);
        tr14 = ReadBytesFrom(2,4);
        tr15 = ReadBytesFrom(2,6);
#ifdef DEBUG
        Serial.println("137 ok");
#endif
        timestamp = millis();
        // if the file is available, write to it:
        if (dataFile&&GREEN_LED_PIN) {
          dataFile.print(timestamp);
          dataFile.print(";");
          dataFile.print(tr15);
          dataFile.print(";");
          dataFile.print(tr14);
          dataFile.print(";");
          dataFile.print(tr13);
          dataFile.print(";");
          dataFile.print(tr12);
          dataFile.print(";");
          dataFile.print(tr11);
          dataFile.print(";");
          dataFile.print(tr10);
          dataFile.print(";");
          dataFile.print(tr9);
          dataFile.print(";");
          dataFile.print(tr8);
          dataFile.print(";");
          dataFile.print(tr7);
          dataFile.print(";");
          dataFile.print(tr6);
          dataFile.print(";");
          dataFile.print(tr5);
          dataFile.print(";");
          dataFile.print(tr4);
          dataFile.print(";");
          dataFile.print(tr3);
          dataFile.print(";");
          dataFile.print(tr2);
          dataFile.print(";");
          dataFile.print(tr1);
          dataFile.print(";");
          dataFile.println(tr0);
#ifdef DEBUG
          Serial.print(timestamp);
          Serial.print(";");
          Serial.print(tr15);
          Serial.print(";");
          Serial.print(tr14);
          Serial.print(";");
          Serial.print(tr13);
          Serial.print(";");
          Serial.print(tr12);
          Serial.print(";");
          Serial.print(tr11);
          Serial.print(";");
          Serial.print(tr10);
          Serial.print(";");
          Serial.print(tr9);
          Serial.print(";");
          Serial.print(tr8);
          Serial.print(";");
          Serial.print(tr7);
          Serial.print(";");
          Serial.print(tr6);
          Serial.print(";");
          Serial.print(tr5);
          Serial.print(";");
          Serial.print(tr4);
          Serial.print(";");
          Serial.print(tr3);
          Serial.print(";");
          Serial.print(tr2);
          Serial.print(";");
          Serial.print(tr1);
          Serial.print(";");
          Serial.println(tr0);
#else
          Serial.println(timestamp);
#endif
        }
        // if the file isn't open, pop up an error:
        else {
           Serial.println("error opening " + fileName);
        }
        
        rxId=0;
        break;
      default:
        break;
    }
  }
}
