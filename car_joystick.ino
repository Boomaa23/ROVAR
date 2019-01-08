/* JoyStick module transceiver code
 - CONNECTIONS: nRF24L01 Modules See:
   1 - GND
   2 - VCC 3.3V !!! NOT 5V
   3 - CE to Arduino pin 9
   4 - CSN to Arduino pin 10
   5 - SCK to Arduino pin 13
   6 - MOSI to Arduino pin 11
   7 - MISO to Arduino pin 12
   8 - UNUSED

   Analog Joystick module
   GND    to Arduino GND
   VCC    to Arduino +5V
   X Pot  to Arduino A0
   Y Pot  to Arduino A1
/*-----( Import needed libraries )-----*/

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define CE_PIN   9
#define CSN_PIN 10
#define xAxis A0     // A0 for Arduino UNO
#define yAxis A1     // A1 for Arduino UNO

// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe

/*-----( Declare objects )-----*/
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

/*-----( Declare Variables )-----*/
int joystick[2];  // 2 element array holding Joystick reading


void setup() {
  pinMode(xAxis, INPUT);
  pinMode(yAxis, INPUT);
  
  delay(500);
  Serial.begin(9600);
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(110);
  radio.openWritingPipe(pipe);
  Serial.print("starting writing pipe \n");
}

void loop() {
  
  joystick[0] = analogRead(xAxis);
  joystick[1] = analogRead(yAxis);

  radio.write(&joystick, sizeof(joystick));
  Serial.print("X = ");
  Serial.print(analogRead(xAxis));
  Serial.print(" Y = ");  
  Serial.print(analogRead(yAxis));
  Serial.print("\n");
}
