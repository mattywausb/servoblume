# Servo Flower (Puppet play assistent)

## Purpose
For a puppet show I needed a flower, that can wither an recover on demand in multiple steps, depending on the progress
of the story. 
To achieve this effect, I have built a servo driven and arduino controlled flower. A sequence of animations 
can be triggered by hitting a button during the show. Additionally, the programming and finetuning of the servo positions and timing
can be done directly on the device, to adapt to spontanoues physical changes (temperature / humidity / repairs). The 
settings are stored in the EEPROM.

## Parts
* A Puppet Flower, controlled via strings (cardboard, wood, kite string)
* 1 servo motor
* 1 Arcade Button (can even be pushed with puppets on the hand)
* 1 LED&KEY Board (8 Buttons, 8x 7 Segmet LED, 8x single LED)
* 1 Arduino UNO 
* Cables, Plugs, small PCB with , 100uF capacitor

## Operation Modes

### Show Mode
After Powering up, the flower will initialize and then go to the start position.

#### Display
* the display shows the step number and the current angle of the servo
* during the animation, the leds will indicate the progress until the target is reached 
* in case, settings have been changed but not saved yet, the decimal point on the last digit will blink
  
#### Operations
* press the **step button** (Arcade Button) to start **next step** of animation 
* press **any button but 1** to jump the **end of the previous step**. When trying to go before the start step, the start step itself will be played
* the final step will automatically switch to the start step when animation is finished

* (open feature) hold **step button for 3 seconds** to reset animation to first step

#### Special Operations while holding button 1
* press **button 6** , to switch to the **configuration mode**.
* press **button 8** to **save** the current settings
* press **button 3** to **load** the previous settings and start again

### configuration mode
When entering configuration mode, the flower will go to the start position (play step 0).

#### Display
* Depending on the "page" the display shows the step number, dot, start angle, end angle or step number and milli seconds for the animation duration
* When editing an angle the leds above the angle that is currently targeted by the servo are lit
* during an animation, the leds will indicate the progress

#### Operations

Flower will drive immedatly to the changed angle parameter 
* press **Button 1/2** to decrease/increase **step number** (increase will play step immedately, decrease will go to the target angle of the step)
* press **Button 5+6** to **leave configuration** mode
* angle page:
  * press **Button 3/4** to decrease/increase **start angle** by 1 degree. 
  * press **Button 7/8** to decrease/increase **stop angle**. 
  * press **Button 5** to switch to **duration page** 
* duration page:
  * press **Button 7/8** to decrease/increase **animation duration** by 250ms. 
  * press **Button 5** to switch to **angle page** an **run the current step**
  
* (open feature) press **Step Button** to Start from beginning and run until current step (will play step immedately)


