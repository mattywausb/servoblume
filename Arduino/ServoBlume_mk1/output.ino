#include "mainSettings.h"
#include <TM1638plus.h>

#ifdef TRACE_ON

#endif

TM1638plus *output_led_module;

void output_setup(TM1638plus *ledKeyModToUse) {
  output_led_module=ledKeyModToUse;
  output_led_module->reset();
  output_led_module->brightness(2);
  output_led_module->displayText("--------");
}
