#include "mainSettings.h"
#include <TM1638plus.h>

#ifdef TRACE_ON

#endif

TM1638plus *output_led_module;


/* ---------------- SETUP ----------------- */

void output_setup(TM1638plus *ledKeyModToUse) {
  output_led_module=ledKeyModToUse;
  output_led_module->reset();
  output_led_module->brightness(1);
  output_led_module->displayText("--------");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, false);
}

/* ********************** Show Scene ******************** */ 

void output_render_ShowScene() {
   char stringBuffer[9];
   sprintf(stringBuffer, "%1d    %03d",g_show_step,g_servo_angle);
   output_led_module->displayText(stringBuffer);
   output_display_step_progress();
}




/* ********************** Scene helper ******************** */ 

void output_display_step_progress() {
  uint16_t pattern=0x0100;
  uint16_t shift_count=0;
  if(g_time_in_step<showStepList[g_show_step].duration) {
    shift_count=((g_time_in_step<<3)/showStepList[g_show_step].duration);
    pattern<<=shift_count;
  } else pattern=0;
  output_led_module->setLEDs(pattern);
  
}

/* ********************** Serial Scene ******************** */ 

void output_render_SerialScene() {
   char stringBuffer[9];
   sprintf(stringBuffer, "%03d     ",g_servo_angle);
   output_led_module->displayText(stringBuffer);
}

