# Servo Flower (Servo Blume)

## Purpose
For a puppet show I needed a flower, that can wither an recover depending on the progress
of the story.
This project solves this by building a servo driven flower. A sequence of animations 
can be triggered by hitting a Button during the show. 

## Parts
* A Puppet Flower, controlled via strings (Cardboard, Wood, Kite String)
* 1 servo motor
* 1 Arcade Button (can even be pushed with puppets on the hand)
* 1 LED&KEY (8 Buttons, 8x 7 Segmet LED, 8x single LED)
* 1 Arduino UNO 
* Cables, Plugs, small PCB with , 100uF capacitor

## Operation Modes

### Show Mode
After Powering up, the flower will start by resetting and go to the start position.

#### Display
* the display shows the step number and the current angle of the servo
* during the animation, the leds will indicate the progress
  
#### Operations
* press the **step button** (Arcade Button) to start **next step** of animation 
* hold **step button for 3 seconds** to reset animation to first step
* press **any button but 1** to jump the **end of the previous step**. When trying to go bevore the start step, the start step itself will be played
* the final step will automatically switch to the start step when animation is finished

#### Special Operations while holding button 1
* press **button 8** , to switch to the **configuration mode**.
* press **button 3** to **save** the current settings and start again
* press **button 6** to **load** the previous settings and start again

### configuration mode
When entering configuration mode, the flower will go to the start position (play step 0).

#### Display
* The display shows the step number blinking together with, start angle, end angle or milli seconds for the animation duration
* during the animation, the leds will indicate the progress

#### Operations

Flower will react immedatly 
* press **Button 1/2** to increase/decrease **step number** (increase will play step immedately, decrease will configure target angle)
* press **Step Button** to increase **step number** (will play step immedately)
* press **Button 3/4** to increase/decrease **start angle** by 1 degree. Hold to fast foreward. 
* press **Button 7/8** to increase/decrease **stop angle**. Hold to fast foreward.
* press **Button 5** to switch between **angle setup<->duration setup** 
* press **Button 7/8** to increase/decrease **animation duration** by 100ms. Hold to fast foreward.
* press **Button 6** to test step (go to end value of prev step, wait 1 second, play animation)
* press **Button 1+2 for 2 seconds" to go into **Show Mode**


