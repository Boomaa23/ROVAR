/* JoyStick module receiver code
 - CONNECTIONS: nRF24L01 Modules See:
   1 - GND
   2 - VCC 3.3V !!! NOT 5V
   3 - CE to Arduino pin 6
   4 - CSN to Arduino pin 8
   5 - SCK to Arduino pin 13
   6 - MOSI to Arduino pin 11
   7 - MISO to Arduino pin 12
   8 - UNUSED
 */
//-----( Import needed libraries )-----
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
/*-----( Declare Constants and Pin Numbers )-----*/
#define CE_PIN 9
#define CSN_PIN 10

#define F1 2
#define B1 3
#define F2 4
#define B2 7
#define PWM1 6
#define PWM2 5

// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe

/*-----( Declare objects )-----*/
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
/*-----( Declare Variables )-----*/
int joystick[2];  // 2 element array holding Joystick readings
int speedRight = 0;
int speedLeft = 0;
int xAxis, yAxis, motorSpeedA, motorSpeedB;

void setup() {
  delay(500);
  Serial.begin(9600);
  Serial.println("Nrf24L01 Receiver Starting"); 
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(110);
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  
  pinMode(F1, OUTPUT);
  pinMode(F2, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);
}

void loop() {
  delay(50);
  radio.read(&joystick, sizeof(joystick) );
  xAxis = joystick[0];
  yAxis = joystick[1];
  
  // Y-axis used for forward and backward control
  if (yAxis < 470) {
  // Set Motor A backward
  digitalWrite(B1, HIGH);
  digitalWrite(F1, LOW);
  // Set Motor B backward
  digitalWrite(B2, HIGH);
  digitalWrite(F2, LOW);
  // Convert the declining Y-axis readings for going backward from 470 to 0 into 0 to 255 value for the PWM signal for increasing the motor speed
  motorSpeedA = map(yAxis, 470, 0, 0, 255);
  motorSpeedB = map(yAxis, 470, 0, 0, 255);
  }
  else if (yAxis > 550) {
  // Set Motor A forward
  digitalWrite(B1, LOW);
  digitalWrite(F1, HIGH);
  // Set Motor B forward
  digitalWrite(B2, LOW);
  digitalWrite(F2, HIGH);
  // Convert the increasing Y-axis readings for going forward from 550 to 1023 into 0 to 255 value for the PWM signal for increasing the motor speed
  motorSpeedA = map(yAxis, 550, 1023, 0, 255);
  motorSpeedB = map(yAxis, 550, 1023, 0, 255);
  }
  // If joystick stays in middle the motors are not moving
  else {
  motorSpeedA = 0;
  motorSpeedB = 0;
  }
  // X-axis used for left and right control
  if (xAxis < 470) {
    // Convert the declining X-axis readings from 470 to 0 into increasing 0 to 255 value
    int xMapped = map(xAxis, 470, 0, 0, 255);
    // Move to left - decrease left motor speed, increase right motor speed
    motorSpeedA = motorSpeedA + xMapped;
    motorSpeedB = motorSpeedB - xMapped;
    // Confine the range from 0 to 255
    if (motorSpeedA < 0) {
    motorSpeedA = 0;
    }
    if (motorSpeedB > 255) {
    motorSpeedB = 255;
    }
  }
  if (xAxis > 550) {
    // Convert the increasing X-axis readings from 550 to 1023 into 0 to 255 value
    int xMapped = map(xAxis, 550, 1023, 0, 255);
    // Move right - decrease right motor speed, increase left motor speed
    motorSpeedA = motorSpeedA - xMapped;
    motorSpeedB = motorSpeedB + xMapped;
    // Confine the range from 0 to 255
    if (motorSpeedA > 255) {
      motorSpeedA = 255;
    }
    if (motorSpeedB < 0) {
      motorSpeedB = 0;
    }
  }
  // Prevent buzzing at low speeds (Adjust according to your motors. My motors couldn't start moving if PWM value was below value of 70)
  if (motorSpeedA < 70) {
  motorSpeedA = 0;
  }
  if (motorSpeedB < 70) {
  motorSpeedB = 0;
  }
  analogWrite(PWM1, motorSpeedB); // Send PWM signal to motor A
  analogWrite(PWM2, motorSpeedA); // Send PWM signal to motor B
  Serial.print("Speed A: ");
  Serial.print(motorSpeedA);
  Serial.print("\n");
  Serial.print("Speed B: ");
  Serial.print(motorSpeedB);
  Serial.print("\n");
  Serial.print("\n");
}
