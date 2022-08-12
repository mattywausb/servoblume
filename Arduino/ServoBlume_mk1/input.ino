
/* Serial input management */
String input_serialBuffer="";
String input_currentSerialCommand="";
boolean input_serialCommand_isNew=false;

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
