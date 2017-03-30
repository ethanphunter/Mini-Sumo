/*
  File: lineFollowing.ino
  Author: Nate Lawernce-Richards
  Description: This program follows a white line.
*/

#include <Servo.h> // Include servo library, see file/examples/servo in the menu for more information

Servo servoR;  // Declare servo variables
Servo servoL;

/*
We define the pin values here so if we need to change the pin for a part we only
 have to change it in one place.

#define is used because the values of these variables won't be changed in the program
*/
#define servoR_pin 10 // Servos
#define servoL_pin 11
#define lineR_pin 5   // Line Sensors
#define lineL_pin 4
#define button_Pin 9  // Start button
#define LED1_pin 12   // LED 2
#define LED2_pin 13   // LED 1

// Declare varibles for future use with sensors
// int is used because these variables will hold an interger and will change
int lineL;
int lineR;

int buttonState = HIGH; // The initial state of the button


// Set up function, runs once to initialize the controller
void setup() {
  pinMode(lineR_pin, INPUT);  // Declare pinmode for line tracking
  pinMode(lineL_pin, INPUT);
  pinMode(button_Pin, INPUT_PULLUP); // Declare pinmode for start button
  /*
  We are using the internal resistor for the button pin, this saves us having so
    solder in our own resistor, it also means we can physicaly break the pin using code.
  If you deactivate the internal resistor by setting the pin mode to OUTPUT then
    the pin is shorted to ground and any output from this pin (aka setting it HIGH)
    will fry the pin.  This cannot be done accedintaly, it must be explicity written
    in the code because the pins will default to input mode if no pinMode is declared.
  */
  pinMode(LED1_pin, OUTPUT);
  pinMode(LED2_pin, OUTPUT);
  while(buttonState == HIGH){ // While the button isn't pushed just sit there, you can run loops in setup
    digitalWrite(LED1_pin, HIGH); // LED is on so you know the robot is ready
    buttonState = digitalRead(button_Pin); // Keep checking to see if the button is pushed
  }
  digitalWrite(LED1_pin, LOW); // After the condition is met the loop ends, LED goes off to indicate this
  servoR.attach(servoR_pin);  // Attaches the servo on pin (defined earlier) to the servo object
  servoL.attach(servoL_pin);  // You attach the servos after you press the button so they won't move at all, in case they aren't calibrated properly
}

void loop (){

  lineR = digitalRead(lineR_pin); // Read sensors and assign the values to variable
  lineL = digitalRead(lineL_pin); // We pass the pin variable to digitalRead so it knows which sensor to check

  // Set motors based on the values read from the line sensors
  if (lineR == HIGH && lineL == HIGH) {   // Both line sensors are on black
    motors(100,100); // Full ahead
  }
  else if (lineR == HIGH && lineL == LOW) { // Left sensor sees the line
    motors(0,100); // Stop the left motor and go forward with the right to reposistion
  }
  else if (lineR == LOW && lineL == HIGH){ // Right sensor sees
    motors(100,0); // Stop the right motor and go forward with the left to reposistion
  }
  // HINT, try adding some delays in to the turns to see if it tracks better

} // end loop

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// Functions, normaly these are put in a header file, but are placed here for your convience
//---------------------------------------------------------------------------------------------------------------------------------------------------------------

// This funtion is used to control the motors
void motors(int l, int r) {      // Syntax motors(left motor value, right motor value)
  r = map(r, -100, 100, 180, 0); // Normal servo library uses 0 to 180 to define postion for a regular servo
  l = map(l, -100, 100, 0, 180); // We have modified our servos for continuous rotation so 0 is full back, 180 is full forward, and 90 is stop
  servoL.write(l);               // This is remapped to -100 to 100 for convenience and flipped for one motor because it will me mounted backwards on the chassis
  servoR.write(r);
}
