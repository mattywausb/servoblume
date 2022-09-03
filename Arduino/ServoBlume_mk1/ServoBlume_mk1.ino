/*
*/

#include "mainsettings.h"
#include "EepromDB.h"
#include <Servo.h>
#include <TM1638plus.h>

#ifdef TRACE_ON
#define TRACE_MODES 
#define INCLUDE_SERIAL_MODE
//#define TRACE_INPUT_HIGH 
//#define TRACE_DB
#endif

#define SERVO_CONTROL_PIN 6

#define SWITCH_INDEX_FOR_STEP 0

#define BUTTON_INDEX_FOR_COMBO 0
#define BUTTON_INDEX_FOR_EDITMODE 5
#define BUTTON_INDEX_FOR_SERIAL 6
#define BUTTON_INDEX_FOR_LOAD 2
#define BUTTON_INDEX_FOR_SAVE 7

#define BUTTON_INDEX_FOR_STEP_FOREWARD 1
#define BUTTON_INDEX_FOR_STEP_BACKWARD 0
#define BUTTON_INDEX_FOR_ANGLE_START_PLUS 3
#define BUTTON_INDEX_FOR_ANGLE_START_MINUS 2
#define BUTTON_INDEX_FOR_ANGLE_STOP_PLUS 7
#define BUTTON_INDEX_FOR_ANGLE_STOP_MINUS 6
#define BUTTON_INDEX_FOR_DURATION_PLUS 7
#define BUTTON_INDEX_FOR_DURATION_MINUS 6
#define BUTTON_INDEX_FOR_EDIT_MODE_CHANGE 4

#define BUTTON_GROUP_FOR_STEP_BACKWARD 0xFE 
#define BUTTON_GROUP_FOR_LEAVE_EDIT 0x30



enum PROCESS_MODES {
  SHOW_MODE, 
  EDIT_MODE,
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

// Variables for save/load feature

struct showStep current_db_record[SHOW_STEP_COUNT];
EepromDB eepromDB;
boolean g_settings_are_saved=false;


// Varibale for the show management
int8_t g_show_step =0;
unsigned long g_show_step_start_time=0;
unsigned long g_time_in_step=0;

enum EDIT_MODES {
  EDIT_ANGLE_START,
  EDIT_ANGLE_STOP,
  EDIT_DURATION,
  RUN_TEST_TO_STEP
};

EDIT_MODES g_edit_mode=EDIT_ANGLE_STOP;
int8_t  g_edit_step=0;

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

  
  if(eepromDB.setupDB(1, sizeof(current_db_record), 20)) {
    #ifdef TRACE_DB 
      eepromDB.dumpToSerial();
    #endif
      loadSettings();
    }
   #ifdef TRACE_ON
    else {
      Serial.println(F("TRACE_ON> ### DB INIT ERROR ###")); 
    }
   #endif

  enter_SHOW_MODE();

}  


void loop() {

  input_pollSerial();

  input_switches_scan_tick();

  switch(g_process_mode) {
    case SHOW_MODE: process_SHOW_MODE();break;
    case EDIT_MODE: process_EDIT_MODE();break;
    #ifdef INCLUDE_SERIAL_MODE
      case SERIAL_MODE:process_SERIAL_MODE();break;
    #endif
    //case TEST_MODE:process_TEST_MODE();break;
   } // switch

   
}

void saveSettings() {
  memcpy(current_db_record,showStepList,sizeof(current_db_record));
  if(eepromDB.updateRecord((byte*)(&current_db_record))) {
    g_settings_are_saved=true;
    #ifdef TRACE_ON
       Serial.println(F("TRACE_ON> Settings saved")); 
    #endif
  }
}

void loadSettings() {
      if(eepromDB.readRecord((byte*)(&current_db_record)))
      {    
      memcpy(showStepList,current_db_record,sizeof(showStepList));
      g_settings_are_saved=true;
      #ifdef TRACE_DB
        Serial.println(F("TRACE_DB> Settings loaded")); 
      #endif
      } 
      #ifdef TRACE_ON
        else   Serial.println(F("TRACE_ON> NO DB RECORD FOUND. USING HARD CODED START VALUES ")); 
      #endif
}

/* ======================== SHOW_MODE ======================== */

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

    g_show_step=SHOW_STEP_COUNT-1; // Start with a full release and buildup
    g_show_step_start_time=g_mode_start_time;
    g_time_in_step=0;
}

void process_SHOW_MODE()
{
   // -- Manage buttons --


   if(input_moduleButtonIsPressed(BUTTON_INDEX_FOR_COMBO)) {  // Special Combos by holding button 1
      // switch to edit mode
      if(input_moduleButtonGotPressed(BUTTON_INDEX_FOR_EDITMODE)) {
        input_ignoreUntilRelease();
        enter_EDIT_MODE();
        return;
       }

      if(input_moduleButtonGotPressed(BUTTON_INDEX_FOR_SAVE)) {
        input_ignoreUntilRelease();
        saveSettings();
        return;
      }

      if(input_moduleButtonGotPressed(BUTTON_INDEX_FOR_LOAD)) {
        input_ignoreUntilRelease();
        loadSettings();
        enter_SHOW_MODE();
        return;
      }
    
      #ifdef INCLUDE_SERIAL_MODE
       // Swtich to serial Mode
       if(input_moduleButtonGotPressed(BUTTON_INDEX_FOR_SERIAL)) {
        input_ignoreUntilRelease();
        enter_SERIAL_MODE();
        return;
       }
      #endif
   }

   
   // Step Foreward
   if(input_switchGotPressed(SWITCH_INDEX_FOR_STEP)) {  
    if(++g_show_step >= SHOW_STEP_COUNT) g_show_step=0;
    start_show_step(g_show_step);
    return;
   }

   // Step Backward
   if(input_moduleButtonGroupGotPressed(BUTTON_GROUP_FOR_STEP_BACKWARD)) 
   {
    if(--g_show_step >=0) end_show_step(g_show_step);
    else start_show_step(0);
   }

   // --  Animate --
   drive_animation();

   // after final step finished goto to step 0 autmatically
   if(g_show_step==SHOW_STEP_COUNT-1) {
     if(g_time_in_step>=showStepList[g_show_step].duration) { // When step is over
          start_show_step(0);
          }
   }
   
  // -- display state --
  output_render_ShowScene();
}

/* ======================== EDIT_MODE ======================== 
  EDIT_ANGLE,
  EDIT_DURATION,
  RUN_TEST_TO_STEP */

void enter_EDIT_MODE()
{
   #ifdef TRACE_MODES
      Serial.print(F("#EDIT_MODE: "));
      Serial.print(freeMemory());
      Serial.print(F(" bytes free memory. "));
      Serial.print(millis()/1000);
      Serial.println(F(" seconds uptime"));
    #endif
    g_process_mode=EDIT_MODE;
    g_mode_start_time=millis();

    g_edit_step=0;
    start_show_step(g_edit_step); // we start at step 0 
    g_edit_mode=RUN_TEST_TO_STEP;
    
}

void process_EDIT_MODE()
{
   // -- Manage buttons --

   // exit EDIT
   if(input_moduleButtonGroupIsPressed(BUTTON_GROUP_FOR_LEAVE_EDIT) ) {
      input_ignoreUntilRelease();
      enter_SHOW_MODE();
      return;
   }
   
   // Step Foreward
   if(input_moduleButtonGotPressed(BUTTON_INDEX_FOR_STEP_FOREWARD)) {  
    if(++g_edit_step >= SHOW_STEP_COUNT) g_edit_step=0;
    start_show_step(g_edit_step);
    g_edit_mode=RUN_TEST_TO_STEP;
   }

   // Step backward
   if(input_moduleButtonGotPressed(BUTTON_INDEX_FOR_STEP_BACKWARD)) {  
    if(--g_edit_step <0) g_edit_step=SHOW_STEP_COUNT-1;
    end_show_step(g_edit_step);
    g_edit_mode=RUN_TEST_TO_STEP;
   }

   // change parameters
   switch(g_edit_mode) {
    case RUN_TEST_TO_STEP:
    case EDIT_ANGLE_START:
    case EDIT_ANGLE_STOP:
               if(input_moduleButtonGotPressed(BUTTON_INDEX_FOR_ANGLE_START_PLUS)) {  
                g_edit_mode=EDIT_ANGLE_START;
                if(++showStepList[g_edit_step].angle_start>179)showStepList[g_edit_step].angle_start=179;
                else g_settings_are_saved=false;
                myServo.write(showStepList[g_edit_step].angle_start);
               }
               if(input_moduleButtonGotPressed(BUTTON_INDEX_FOR_ANGLE_START_MINUS)) {  
                g_edit_mode=EDIT_ANGLE_START;
                if(--showStepList[g_edit_step].angle_start<0)showStepList[g_edit_step].angle_start=0;
                else g_settings_are_saved=false;
                myServo.write(showStepList[g_edit_step].angle_start);
               }
               if(input_moduleButtonGotPressed(BUTTON_INDEX_FOR_ANGLE_STOP_PLUS)) {  
                g_edit_mode=EDIT_ANGLE_STOP;
                if(++showStepList[g_edit_step].angle_stop>179)showStepList[g_edit_step].angle_stop=179;
                else g_settings_are_saved=false;
                myServo.write(showStepList[g_edit_step].angle_stop);
               }
               if(input_moduleButtonGotPressed(BUTTON_INDEX_FOR_ANGLE_STOP_MINUS)) {  
                g_edit_mode=EDIT_ANGLE_STOP;
                if(--showStepList[g_edit_step].angle_stop<0)showStepList[g_edit_step].angle_stop=0;
                else g_settings_are_saved=false;
                myServo.write(showStepList[g_edit_step].angle_stop);
               }
               if(input_moduleButtonGotPressed(BUTTON_INDEX_FOR_EDIT_MODE_CHANGE)) {  
                  g_edit_mode=EDIT_DURATION;
               }
               break;
    case EDIT_DURATION:
               if(input_moduleButtonGotPressed(BUTTON_INDEX_FOR_DURATION_PLUS)) {  
                g_edit_mode=EDIT_DURATION;
                showStepList[g_edit_step].duration+=250;
                if(showStepList[g_edit_step].duration>20000)showStepList[g_edit_step].duration=20000;
                else g_settings_are_saved=false;
               }
               if(input_moduleButtonGotPressed(BUTTON_INDEX_FOR_DURATION_MINUS)) {  
                g_edit_mode=EDIT_DURATION;
                showStepList[g_edit_step].duration-=250;
                if(showStepList[g_edit_step].duration<250)showStepList[g_edit_step].duration=250;
                else g_settings_are_saved=false;
               }
              if(input_moduleButtonGotPressed(BUTTON_INDEX_FOR_EDIT_MODE_CHANGE)) {  // Switch run step and switch to angle mode
                myServo.write(showStepList[g_edit_step].angle_start);
                delay(200);
                start_show_step(g_edit_step);
                g_edit_mode=RUN_TEST_TO_STEP;
              }
               break;
   }
   
   // --  Animate --
   if(g_edit_mode == RUN_TEST_TO_STEP) {
       drive_animation();
       if(g_edit_step> g_show_step && g_time_in_step>=showStepList[g_show_step].duration){ // Test has not reached edit step to autoforeward
           delay(500);
          start_show_step(g_show_step+1);
       }
   }
   
   output_render_EditScene();
     
}

#ifdef INCLUDE_SERIAL_MODE
/* ======================== SERIAL_MODE ========================*/

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
#endif

/* ======== animation utility function ========*/

void start_show_step(int8_t step_index) {
   if (step_index<0 || step_index>=SHOW_STEP_COUNT) return;
   g_show_step=step_index;
   g_show_step_start_time=millis();
}

void end_show_step(int8_t step_index) {
   if (step_index<0 || step_index>=SHOW_STEP_COUNT) return;
   g_show_step=step_index;
   g_show_step_start_time=millis()-showStepList[g_show_step].duration;
}

void drive_animation() {
   long angle1000s_per_ms=0;
     
   // Determine and set current angle for running step
   g_time_in_step=millis()-g_show_step_start_time;
   if(g_time_in_step<showStepList[g_show_step].duration) { // calculate intermediate angle
        angle1000s_per_ms=(((long)(showStepList[g_show_step].angle_start-showStepList[g_show_step].angle_stop))<<10)/showStepList[g_show_step].duration; // Multiply by 1024 to get good resolution
        g_servo_angle=showStepList[g_show_step].angle_start-(int)((angle1000s_per_ms*g_time_in_step)>>10);
   } else {
    g_servo_angle=showStepList[g_show_step].angle_stop;
   }
   myServo.write(g_servo_angle);
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
