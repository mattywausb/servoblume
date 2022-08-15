/*
*/

#include "mainsettings.h"
#include <Servo.h>
#include <TM1638plus.h>

#ifdef TRACE_ON
#define TRACE_MODES 
//#define TRACE_INPUT_HIGH 
#endif

#define SERVO_CONTROL_PIN 6

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
TM1638plus ledAndKeymodule( 3,4,5);  // the led+keys module is input and output, so core must own it


// Variable for Servo control and configuration
int g_servo_angle;   // variable to hold the g_servo_angle for the servo motor


struct showStep {
  int angle_start;
  int angle_stop;
  int duration; // in milliseconds
}; 

struct showStep showStepList[] = {{0,39,500},  // Start step
                                  {39,59,2000},
                                  {59,79,2000},
                                  {82,179,10000},
                                  {110,0,3500}}; 

#define SHOW_STEP_COUNT (sizeof(showStepList)/sizeof(showStepList[0]))

// Varibale for the show management
byte g_show_step =0;
unsigned long g_show_step_start_time=0;
unsigned long g_time_in_step=0;

enum EDIT_MODES {
  EDIT_ANGLE,
  EDIT_DURATION,
  RUN_TEST_TO_STEP
};

EDIT_MODES g_edit_mode=EDIT_ANGLE;

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
  g_servo_angle=90; // start at to middle position
  myServo.write(g_servo_angle);
  enter_SERIAL_MODE();

}  


void loop() {

  input_pollSerial();

  input_switches_scan_tick();

  switch(g_process_mode) {
    case SHOW_MODE: process_SHOW_MODE();break;
    //case SET_MODE: process_SET_MODE();break;
    case SERIAL_MODE:process_SERIAL_MODE();break;
    //case TEST_MODE:process_TEST_MODE();break;
   } // switch

   
}

/* ======== SHOW_MODE ======== */

void enter_SHOW_MODE()
{
   #ifdef TRACE_MODES
      Serial.print(F("#SHOW_MODE: "));
      Serial.print(freeMemory());
      Serial.print(F(" bytes free memory. "));
      Serial.print(millis()/1000);
      Serial.println(F(" seconds uptime"));
    #endif
    g_process_mode=SHOW_MODE;
    g_mode_start_time=millis();

    g_show_step=0;
    g_show_step_start_time=g_mode_start_time;
    g_time_in_step=0;
}

void process_SHOW_MODE()
{

  
   long angle1000s_per_ms=0;
    
   // Goto Serial Mode
   if(input_moduleButtonGotPressed(0)) {
    input_ignoreUntilRelease();
    enter_SERIAL_MODE();
    return;
   }

   // Step Foreward
   if(input_moduleButtonGotPressed(3)) {  // TODO Change to Final switch button
    if(++g_show_step >= SHOW_STEP_COUNT) g_show_step=0;
    g_show_step_start_time=millis();
   }

   // Determine and set current angle
   g_time_in_step=millis()-g_show_step_start_time;
   if(g_time_in_step<showStepList[g_show_step].duration) { // calculate intermediate angle
        angle1000s_per_ms=(((long)(showStepList[g_show_step].angle_start-showStepList[g_show_step].angle_stop))<<10)/showStepList[g_show_step].duration; // Multiply by 1024 to get good resolution
        g_servo_angle=showStepList[g_show_step].angle_start-(int)((angle1000s_per_ms*g_time_in_step)>>10);
   } else {
    g_servo_angle=showStepList[g_show_step].angle_stop;
    if(g_show_step==SHOW_STEP_COUNT-1) {
      g_show_step=0;
      g_show_step_start_time=millis();
      }
   }

   myServo.write(g_servo_angle);
   
  // display state 
  output_render_ShowScene();
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
   if(input_moduleButtonGotPressed(7)) {
    input_ignoreUntilRelease();
    enter_SHOW_MODE();
    return;
   }
   
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
  output_render_SerialScene();
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
