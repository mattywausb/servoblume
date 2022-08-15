/* Functions to handle all input elements */

#include "mainSettings.h"

// Activate general trace output

#ifdef TRACE_ON
#define TRACE_INPUT_EVENTS 
#define TRACE_INPUT_HIGH 
#endif


#define PORT_STEP_BUTTON 7

/* general variables of the input module */
bool input_enabled=true;

/* Normal switch management variables */
byte input_switchPortList[]={7};
#define SWITCH_COUNT sizeof(input_switchPortList)

byte input_switch_current_state = 0;
byte input_switch_last_state = 0;
byte input_switch_gotPressed_flag = 0;
byte input_switch_gotReleased_flag = 0;
unsigned long input_switch_last_read_time = 0;
unsigned long input_switch_state_start_time=0;
#define INPUT_SWITCH_COOLDOWN_MS 10


/* TM1638 Button management variables */

TM1638plus *buttonModule;
byte modButtons_current_state = 0;
byte modButtons_last_state = 0;
byte modButtons_gotPressed_flag = 0;
byte modButtons_gotReleased_flag = 0;
unsigned long modButtons_last_read_time = 0;
unsigned long modButtons_state_start_time=0;
#define MODBUTTONS_COOLDOWN_MS 10

/* Serial input management */
String input_serialBuffer="";
String input_currentSerialCommand="";
boolean input_serialCommand_isNew=false;

/* ---------------- SETUP ----------------- */
void input_setup(TM1638plus *buttonModuleToUse) {

 /* Initialize switch ports */
  for(byte si=0; si<SWITCH_COUNT;si++)  pinMode(input_switchPortList[si], INPUT_PULLUP);
  input_switch_last_state = 0;
  input_switch_gotPressed_flag = 0;
  input_switch_gotReleased_flag = 0;
  input_switch_last_read_time = 0;
  input_switch_state_start_time=0;
  
  /* Register button Module we need to ask */
  buttonModule = buttonModuleToUse;

}

/* -------- Switch query functions ----------- */

bool input_switchGotPressed(byte buttonIndex)
{
  #ifdef TRACE_INPUT_EVENTS
    if(input_enabled && bitRead(input_switch_gotPressed_flag, buttonIndex)) {
         Serial.print(F("TRACE_INPUT_EVENTS signalled input_switch got pressed for "));
         Serial.println(buttonIndex);
    }
  #endif
  return input_enabled && bitRead(input_switch_gotPressed_flag, buttonIndex); 
}

bool input_switchGroupGotPressed(byte input_switch_of_interest_pattern)
{
  #ifdef TRACE_INPUT_EVENTS
    if(input_enabled && (input_switch_gotPressed_flag&input_switch_of_interest_pattern)) {
         Serial.print(F("TRACE_INPUT_EVENTS signalled input_switchgroup got pressed for "));
         Serial.println(input_switch_of_interest_pattern,HEX);
    }
  #endif
  return input_enabled && (input_switch_gotPressed_flag & input_switch_of_interest_pattern); 
}

bool input_switchIsPressed(byte buttonIndex)
{
  return input_enabled && bitRead(input_switch_current_state,buttonIndex);
}

bool input_switchGroupIsPressed(byte input_switch_of_interest_pattern)
{
  return input_enabled && (input_switch_current_state & input_switch_of_interest_pattern);
}

bool input_switchGotReleased(byte buttonIndex)
{
  #ifdef TRACE_INPUT_EVENTS
    if(input_enabled && bitRead(input_switch_gotReleased_flag, buttonIndex)) {
         Serial.print(F("TRACE_INPUT_EVENTS signalled input_switch got released for "));
         Serial.println(buttonIndex);
    }
  #endif
  return input_enabled && bitRead(input_switch_gotReleased_flag,buttonIndex); 
}

unsigned long input_switchStateDuration()  // return the time in ms , the current press state exists
{
  return millis()-input_switch_state_start_time;
}



/* -------- Button query functions ----------- */

bool input_moduleButtonGotPressed(byte buttonIndex)
{
  #ifdef TRACE_INPUT_EVENTS
    if(input_enabled && bitRead(modButtons_gotPressed_flag, buttonIndex)) {
         Serial.print(F("TRACE_INPUT_EVENTS signalled button got pressed for "));
         Serial.println(buttonIndex);
    }
  #endif
  return input_enabled && bitRead(modButtons_gotPressed_flag, buttonIndex); 
}

bool input_moduleButtonGroupGotPressed(byte modButtons_of_interest_pattern)
{
  #ifdef TRACE_INPUT_EVENTS
    if(input_enabled && (modButtons_gotPressed_flag&modButtons_of_interest_pattern)) {
         Serial.print(F("TRACE_INPUT_EVENTS signalled buttongroup got pressed for "));
         Serial.println(modButtons_of_interest_pattern,HEX);
    }
  #endif
  return input_enabled && (modButtons_gotPressed_flag & modButtons_of_interest_pattern); 
}

bool input_moduleButtonIsPressed(byte buttonIndex)
{
  return input_enabled && bitRead(modButtons_current_state,buttonIndex);
}

bool input_moduleButtonGroupIsPressed(byte modButtons_of_interest_pattern)
{
  return input_enabled && ((modButtons_current_state & modButtons_of_interest_pattern)==modButtons_of_interest_pattern);
}

bool input_moduleButtonGotReleased(byte buttonIndex)
{
  #ifdef TRACE_INPUT_EVENTS
    if(input_enabled && bitRead(modButtons_gotReleased_flag, buttonIndex)) {
         Serial.print(F("TRACE_INPUT_EVENTS signalled button got released for "));
         Serial.println(buttonIndex);
    }
  #endif
  return input_enabled && bitRead(modButtons_gotReleased_flag,buttonIndex); 
}

unsigned long input_moduleButtonsStateDuration()  // return the time in ms , the current press state exists
{
  return millis()-modButtons_state_start_time;
}

/* ---- Serial Data Input function ---- */

boolean input_newSerialCommandAvailable() {return input_serialCommand_isNew;}

String input_getSerialCommand() 
{
    input_serialCommand_isNew=false;
    return input_currentSerialCommand;
}

/* Input Utility functions */
void input_ignoreUntilRelease()
{
  input_enabled=false;
}


/* ---- Serial Data Input  polling (must be called by loop) ---- */


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

  byte mask=0x01;
  // ** SWITCHES **
  input_switch_last_state = input_switch_current_state;
  if (millis() - input_switch_last_read_time > INPUT_SWITCH_COOLDOWN_MS)
  {
    input_switch_current_state=0;
    for(byte si=0 ; si<SWITCH_COUNT;si++) { // scan all ports an store state as bits 
       if(!digitalRead(input_switchPortList[si])) input_switch_current_state|=mask;
       mask<<=1;
    }
    #ifdef TRACE_INPUT_HIGH
      if(input_switch_current_state) { Serial.print(F("TRACE_INPUT_HIGH input_switch_current_state:"));Serial.println(input_switch_current_state,HEX);}
    #endif 
    input_switch_last_read_time = millis();
    if(input_switch_last_state!=input_switch_current_state) input_switch_state_start_time=input_switch_last_read_time; // reset change timer
  }
  // derive state change information for the button module 
  input_switch_gotPressed_flag = (input_switch_last_state ^ input_switch_current_state)&input_switch_current_state;
  input_switch_gotReleased_flag = (input_switch_last_state ^ input_switch_current_state) & ~input_switch_current_state;

  #ifdef TRACE_INPUT_EVENTS
    if(input_switch_gotPressed_flag) {
         Serial.print(F("TRACE_INPUT_EVENTS input_switch_gotPressed_flag:"));Serial.println(input_switch_gotPressed_flag,HEX);}
    if(input_switch_gotReleased_flag) {
         Serial.print(F("TRACE_INPUT_EVENTS input_switch_gotReleased_flag:"));Serial.println(input_switch_gotReleased_flag,HEX);}
  #endif

  
  // ** MODULE BUTTONS **
  modButtons_last_state = modButtons_current_state;
  if (millis() - modButtons_last_read_time > MODBUTTONS_COOLDOWN_MS)
  {
    modButtons_current_state = buttonModule->readButtons();
    modButtons_last_read_time = millis();
    if(modButtons_last_state!=modButtons_current_state) modButtons_state_start_time=modButtons_last_read_time; // reset change timer
  }
  // derive state change information for the button module 
  modButtons_gotPressed_flag = (modButtons_last_state ^ modButtons_current_state)&modButtons_current_state;
  modButtons_gotReleased_flag = (modButtons_last_state ^ modButtons_current_state) & ~modButtons_current_state;

  // When all buttons are released, enable input 
  if(modButtons_current_state==0 && input_switch_current_state==0)  input_enabled=true;

  #ifdef TRACE_INPUT_EVENTS
    if(modButtons_gotPressed_flag) {
         Serial.print(F("TRACE_INPUT_EVENTS modButtons_gotPressed_flag:"));Serial.println(modButtons_gotPressed_flag,HEX);}
    if(modButtons_gotReleased_flag) {
         Serial.print(F("TRACE_INPUT_EVENTS modButtons_gotReleased_flag:"));Serial.println(modButtons_gotReleased_flag,HEX);}
  #endif
}


