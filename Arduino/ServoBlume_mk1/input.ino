/* Functions to handle all input elements */

#include "mainSettings.h"

// Activate general trace output

#ifdef TRACE_ON
#define TRACE_INPUT 1
//#define TRACE_INPUT_HIGH 1
#define TRACE_INPUT_traceValue_acceleration
#endif


#define PORT_STEP_BUTTON 6


/* TM1638 Button management variables */

TM1638plus *buttonModule;
byte buttons_current_state = 0;
byte buttons_last_state = 0;
byte buttons_gotPressed_flag = 0;
byte buttons_gotReleased_flag = 0;
unsigned long buttons_last_read_time = 0;
#define BUTTON_MODULE_COOLDOWN_MS 10

/* Serial input management */
String input_serialBuffer="";
String input_currentSerialCommand="";
boolean input_serialCommand_isNew=false;

/* ---------------- SETUP ----------------- */
void input_setup(TM1638plus *buttonModuleToUse) {
  /* Register button Module we need to ask */
  buttonModule = buttonModuleToUse;

  /* Initialize switch  */
  pinMode(PORT_STEP_BUTTON, INPUT_PULLUP);
}


/* ---- Serial Data Input ---- */

boolean input_newSerialCommandAvailable() {return input_serialCommand_isNew;}

String input_getSerialCommand() 
{
    input_serialCommand_isNew=false;
    return input_currentSerialCommand;
}

/* ---- Serial Data Input ---- */


void input_pollSerial() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    input_serialBuffer += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      input_serialCommand_isNew=true;
      input_currentSerialCommand=input_serialBuffer;
      input_serialBuffer="";
      #ifdef TR_INP
          Serial.print(F("TR_INP>Fetched serial command:"));
          Serial.println(input_currentSerialCommand);
      #endif
    }
  }
}

void input_switches_scan_tick() {
   /* capture buttonsModule states, prevent bouncing with simple cooldown  */
  buttons_last_state = buttons_current_state;
  if (millis() - buttons_last_read_time > BUTTON_MODULE_COOLDOWN_MS)
  {
    buttons_current_state = buttonModule->readButtons();
    buttons_last_read_time = millis();
  }
}


