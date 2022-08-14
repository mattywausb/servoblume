#include "mainSettings.h"
#include <TM1638plus.h>

#ifdef TRACE_ON

#endif

TM1638plus *output_led_module;


/* ---------------- SETUP ----------------- */

void output_setup(TM1638plus *ledKeyModToUse) {
  output_led_module=ledKeyModToUse;
  output_led_module->reset();
  output_led_module->brightness(2);
  output_led_module->displayText("--------");
}

/* ********************** Show Scene ******************** */ 

void output_render_ShowScene() {
   char stringBuffer[9];
   sprintf(stringBuffer, "%1d    %03d",g_show_step,g_servo_angle);
   output_led_module->displayText(stringBuffer);
}


/* ********************** Serial Scene ******************** */ 

void output_render_SerialScene() {
   char stringBuffer[9];
   sprintf(stringBuffer, "%03d     ",g_servo_angle);
   output_led_module->displayText(stringBuffer);
}

