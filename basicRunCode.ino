/*
  File: basicRunCode.ino
  Author: Nate Lawernce-Richards
  Description: This is the basic run code that is used for a mini sumo match
*/

#include <Servo.h> // Include servo library, see file/examples/servo in the menu for more information

Servo servoR; // Declare servo variables
Servo servoL;


#define servoR_pin 10 // Servos
#define servoL_pin 11
#define lineR_pin 5   // Line sensors
#define lineL_pin 4
#define midR_pin A5   // Midrange sensors
#define midL_pin A4
#define prox_pin 6    // Proximity Sensor
#define button_Pin 9  // Start button
#define LED1_pin 12   // LED 2
#define LED2_pin 13   // LED 1

// Declare varibles for future use with sensors
// int is used because these values will be an interger and will change
int lineL;
int lineR;
int prox;
int midR;
int midL;

int threshold = 200; /* Thresholdold for confirmed analog hit, derived from experimental testing
                     Larger numbers mean the target is closer */

int buttonState = HIGH; // The initial state of the button


// Set up loop, runs once to initialize the controller
void setup() {
  pinMode(lineR_pin, INPUT);         // Declare pinmode for line tracking
  pinMode(lineL_pin, INPUT);
  pinMode(midR_pin, INPUT);          // Declare pinmode for midrange sensors
  pinMode(midL_pin, INPUT);
  pinMode(prox_pin, INPUT);          // Declare pinmode for proximity detector
  pinMode(button_Pin, INPUT_PULLUP); // Declare pinmode for start button
  pinMode(LED1_pin, OUTPUT);
  pinMode(LED2_pin, OUTPUT);

  while(buttonState == HIGH) { // While the button isn't pressed, the robot just sits there, you can run loops in setup
    digitalWrite(LED1_pin, HIGH); // LED is on so you know the robot is ready
    buttonState = digitalRead(button_Pin); // Keep checking to see if the button is pressed
  }
  digitalWrite(LED1_pin, LOW); // After the condition is met the loop ends, LED goes off to indicate this
  delay(5000);                 // Now wait 5 sec, dealy is in milisecond
  servoR.attach(servoR_pin);   // Attaches the servo on pin (defined earlier) to the servo object
  servoL.attach(servoL_pin);   // You attach the servos after the 5 second wait so they won't move at all before time is up, in case they aren't calibrated properly
}

// Main loop, this will run continuously until the controller is reset
void loop() {

  sensors(); // This function is defined below and checks the values of the sensors

  /* After sensors are checked the code is one big "else if" loop
  the hierarchy is decided by the importance of conditions, if the front proximity sensor is tripped then the opponet is right in front
  no need to check any other cases at this point so the loop ends and starts over again
  otherwise the loop progresses to the midrange sensors and then the line sensors
  */

  // Target in front
  if (prox == LOW) { // The front proximity sensor goes low when it detects an object
    motors(100, 100); // Sets motors to drive full forward
  }
  // Both spot
  else if (midR > threshold && midL > threshold) { // Both mid range sensors are above the detection threshold
    motors(100, 100);  // Full foward
  }
  else if (midL > threshold && midR < threshold) {  // Only mid L spots
    motors(-100, 100);  // Turns left
  }
  else if (midR > threshold && midL < threshold) {  // Only mid R spots
    motors(100, -100);  // Turns right
  }
  // No robots, time to check to see if you are driving out of the arena
  else if (lineR == HIGH && lineL == HIGH) {   // If both line sensors are on black, full ahead
    motors(100, 100);
  }
  else if (lineR == LOW && lineL == HIGH) { // Right line sensor hits the white line and triggers low
    motors(-100, -100); // Back up
    delay(300);        // Only back up for .3 seconds
    motors(-100, 100);  // Spin away from edge
    delay(400);        // Keep spinning for .4 seconds to turn around
  }
  else if (lineR == HIGH && lineL == LOW) { // Left line sensor hits
    motors(-100, -100);
    delay(300);
    motors(100, -100);
    delay(400);
  }
  else if(lineR == LOW && lineL == LOW) {  // Both sensors are over the line, not a good position
    motors(-100, -100);
    delay(300);
    motors(-100, 100);
    delay(400);
  }
  else { // If for some reason no conditions trigger keep going forward, just a backup for the unseen
    motors(100, 100);
  }

  delay(10); // A slight delay at the end of the code is good for stability, and prevents jitery movement.
} //end loop


//-----------------------------------------------------------------------------------------------------------------------------------
// Functions, normaly these are put in a header file, but are placed here for your convience
//-----------------------------------------------------------------------------------------------------------------------------------

// Sensor check
int sensors() { // int because its returning variables
  midR = analogRead(midR_pin);
  midL = analogRead(midL_pin);
  prox = digitalRead(prox_pin);
  lineR = digitalRead(lineR_pin);
  lineL = digitalRead(lineL_pin);
}

// Prints off all sensor values to serial port, in the default version of this code the serial funtion is not used
void sensors_print() {
  Serial.print("Midrange Right = ");
  Serial.println(midR);
  Serial.print("Midrange Left = ");
  Serial.println(midL);
  Serial.print("Proximity = ");
  Serial.println(prox);
  Serial.print("Right Line Sensor = ");
  Serial.println(lineR);
  Serial.print("Left Line Sensor = ");
  Serial.println(lineL);
}

// This funtion is used to control the motors
void motors(int l, int r) {      // Syntax motors(left motor value, right motor value)
  r = map(r, -100, 100, 180, 0); // Normal servo library uses 0 to 180 to define postion for a regular servo
  l = map(l, -100, 100, 0, 180); // We have modified our servos for continuous rotation so 0 is full back, 180 is full forward, and 90 is stop
  servoL.write(l);               // This is remapped to -100 to 100 for convenience and flipped for one motor because it will me mounted backwards on the chassis
  servoR.write(r);
}
