/*
*/

#include "mainsettings.h"
#include <Servo.h>
#include <TM1638plus.h>

#define SERVO_CONTROL_PIN 9

enum PROCESS_MODES {
  SHOW_MODE, 
  SET_MODE,
  SERIAL_MODE,
  TEST_MODE
};

/* mode management data */
PROCESS_MODES g_process_mode = SERIAL_MODE; 
unsigned long g_mode_start_time=0;


Servo myServo;  // create a servo object
TM1638plus ledAndKeymodule(2, 3, 4);  // the led+keys module is input and output, so core must own it


int g_servo_angle;   // variable to hold the g_servo_angle for the servo motor

void setup() {
  #ifdef TRACE_ON 
    char compile_signature[] = "--- START (Build: " __DATE__ " " __TIME__ ") ---";   
    Serial.begin(9600);
    Serial.println(compile_signature); 
  #endif
  ledAndKeymodule.displayBegin();
  
  output_setup(&ledAndKeymodule);
  input_setup(&ledAndKeymodule); 

  myServo.attach(SERVO_CONTROL_PIN); // attaches the servo on pin 9 to the servo object
  myServo.write(90);
  enter_SERIAL_MODE();

}  


void loop() {

  input_pollSerial();

  input_switches_scan_tick() ;

  switch(g_process_mode) {
    //case SHOW_MODE: process_SHOW_MODE();break;
    //case SET_MODE: process_SET_MODE();break;
    case SERIAL_MODE:process_SERIAL_MODE();break;
    //case TEST_MODE:process_TEST_MODE();break;
   } // switch

   
}

/* ======== SERIAL_MODE ========*/

void enter_SERIAL_MODE()
{
   #ifdef TRACE_MODES
      Serial.print(F("#SERIAL_MODE: "));
      Serial.print(freeMemory());
      Serial.print(F(" bytes free memory. "));
      Serial.print(millis()/1000);
      Serial.println(F(" seconds uptime"));
    #endif
    g_process_mode=SERIAL_MODE;
    g_mode_start_time=millis();
}

void process_SERIAL_MODE()
{
    int inputNumber=0;
   // Manage button input

   // Manage serial input

   if(input_newSerialCommandAvailable() ) {
     String command=input_getSerialCommand();
     inputNumber = command.toInt();
    if(inputNumber>=0 && inputNumber<10) g_servo_angle=max(inputNumber*20-1,0)  ;
    // print out the g_servo_angle for the servo motor
    Serial.print("g_servo_angle: ");
    Serial.println(g_servo_angle);
     myServo.write(g_servo_angle);
  }   
   
   // Finally display state 

}


/* ******************** Memory Helper *************** */
 
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}
