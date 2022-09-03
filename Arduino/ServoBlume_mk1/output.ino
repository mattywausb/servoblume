#include "mainSettings.h"
#include <TM1638plus.h>

#ifdef TRACE_ON
  #define INCLUDE_SERIAL_MODE
#endif

TM1638plus *output_led_module;


/* ---------------- SETUP ----------------- */

void output_setup(TM1638plus *ledKeyModToUse) {
  output_led_module=ledKeyModToUse;
  output_led_module->reset();
  output_led_module->brightness(0);
  output_led_module->displayText("--------");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, false);
}

/* ********************** Show Scene ******************** */ 

void output_render_ShowScene() {
   char stringBuffer[10];
   sprintf(stringBuffer, "%1d    %03d",g_show_step,g_servo_angle);
   if(!g_settings_are_saved) if((millis()>>9)&0x01) strcat(stringBuffer,"."); // add on Decimal point about 1 Hz = divide by 512 Modulo 2
   output_led_module->displayText(stringBuffer);
   output_display_step_progress();
}


/* ********************** Edit Scene ******************** */ 

void output_render_EditScene() {
   char stringBuffer[10];
   switch(g_edit_mode) {
    case EDIT_ANGLE_START:     sprintf(stringBuffer, "%1d.%03d %03d",g_edit_step,showStepList[g_edit_step].angle_start,showStepList[g_edit_step].angle_stop);
                              output_led_module->setLEDs(0x0e00);
                              break;
    case EDIT_ANGLE_STOP:     sprintf(stringBuffer, "%1d.%03d %03d",g_edit_step,showStepList[g_edit_step].angle_start,showStepList[g_edit_step].angle_stop);
                              output_led_module->setLEDs(0xe000);
                               break;
    case EDIT_DURATION:  sprintf(stringBuffer, "%1d  %05d",g_edit_step,showStepList[g_edit_step].duration);
                              output_led_module->setLEDs(0xf800);
                    break;
    case RUN_TEST_TO_STEP: sprintf(stringBuffer, "%1d.%03d %03d",g_show_step,showStepList[g_show_step].angle_start,showStepList[g_show_step].angle_stop);
                    output_display_step_progress();
                    break;
    default: sprintf(stringBuffer, "-_-_-_-_");
   }
   if(!g_settings_are_saved) if((millis()>>9)&0x01) strcat(stringBuffer,"."); // add on Decimal point about 1 Hz = divide by 512 Modulo 2
   output_led_module->displayText(stringBuffer);
}


/* ********************** Scene helper ******************** */ 

void output_display_step_progress() {
  uint16_t pattern=0xFF00;
  uint16_t shift_count=0;
  if(g_time_in_step<showStepList[g_show_step].duration) {
    shift_count=((g_time_in_step<<3)/showStepList[g_show_step].duration);
    pattern<<=shift_count;
  } else pattern=0;
  output_led_module->setLEDs(pattern);
  
}

#ifdef INCLUDE_SERIAL_MODE
/* ********************** Serial Scene ******************** */ 

void output_render_SerialScene() {
   char stringBuffer[9];
   sprintf(stringBuffer, "%03d     ",g_servo_angle);
   output_led_module->displayText(stringBuffer);
}

#endif
