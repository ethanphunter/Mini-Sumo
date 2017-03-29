/*
  File: motorTest.ino
  Author: Nate Lawernce-Richards
  Description: This code is for testing modified servos.
*/

// Note: Comments are good, use them often. Use // for single line commets

/*
for
multi
line
comments
*/

#include <Servo.h> // Include servo library, see file/examples/servo in the menu for more information

Servo servoR;  // Declare servo objects
Servo servoL;

#define servoR_pin 10  // Servos
#define servoL_pin 11

void setup() {
  servoR.attach(servoR_pin);  // Attaches the servo on the pin that was defined above, to the servo object
  servoL.attach(servoL_pin);  // You attach the servos after the 5 second wait so they won't move at all before time is up, incase they aren't calibrated properly
}

void loop() {
  motors(100,100); // Motors full forward
  delay (1000);
  motors(-100, -100); // Motors full reverse
  delay (1000);
  motors(50, 50);  // Motors half forward
  delay (1000);
  motors(-50, -50);  // Motors half reverse
  delay (1000);
}

// This funtion is called in the main loop to control the motors
void motors(int l, int r) {  // Syntax motors(left motor value, right motor value)
  r = map(r, -100, 100, 180, 0); // Normal servo library uses 0 to 180 to define postion for a regular servo
  l = map(l, -100, 100, 0, 180);  // We have modified our servos for continuous rotation so 0 is full back, 180 is full forward, and 90 is stop
  servoL.write(l);        // This is remapped to -100 to 100 for convenience and flipped for one motor because it will me mounted backwards on the chassis
  servoR.write(r);
}
