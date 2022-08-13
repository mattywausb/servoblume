/*
*/

#define SERVO_CONTROL_PIN 9
#include "mainsettings.h"
#include <Servo.h>

Servo myServo;  // create a servo object

int const potPin = A0; // analog pin used to connect the potentiometer
int potVal;  // variable to read the value from the analog pin
int angle;   // variable to hold the angle for the servo motor

void setup() {
  #ifdef TRACE_ON 
    char compile_signature[] = "--- START (Build: " __DATE__ " " __TIME__ ") ---";   
    Serial.begin(9600);
    Serial.println(compile_signature); 
  #endif

  myServo.attach(SERVO_CONTROL_PIN); // attaches the servo on pin 9 to the servo object
   myServo.write(90);

}  


void loop() {
  int inputNumber=0;
  input_pollSerial();

  if(input_newSerialCommandAvailable() ) {
     String command=input_getSerialCommand();
     inputNumber = command.toInt();
    if(inputNumber>=0 && inputNumber<10) angle=max(inputNumber*20-1,0)  ;
    // print out the angle for the servo motor
    Serial.print("angle: ");
    Serial.println(angle);
     myServo.write(angle);
  }    


  // set the servo position
 

  // wait for the servo to get there
  delay(15);
}
