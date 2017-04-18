/*
 File: advancedRunCode.ino
 Created by Nate Lawernce-Richards for the University of Northern Iowa
 Description: This is the same program as the basic run code, but it is a bit smarter
*/

#include <Servo.h> // Include servo library

Servo servoR;  // Declare servo variables
Servo servoL;

// Set vaules for pin in variables
#define servoR_pin 10 // Servos
#define servoL_pin 11
#define lineR_pin 5   // Line Sensors
#define lineL_pin 4
#define midR_pin A5   // Midrange Sensors
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

// For running the timer
unsigned long end_time;

int threshold = 125; /* Thresholdold for confirmed analog hit, derived from
                        experimental testing Larger numbers mean the target is closer */

int buttonState = HIGH; // The initial state of the button

// For averaging
#define numReadings 10
int index = 0;

// For midR averaging
int midR_readings[numReadings];
int midR_total = 0;
int midR_avg = 0;

// For midL averaging
int midL_readings[numReadings];
int midL_total = 0;
int midL_avg = 0;



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

  // Initialize midR array
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    midR_readings[thisReading] = 0;

  // Initialize midL array
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    midL_readings[thisReading] = 0;

  while(buttonState == HIGH) { // While the button isn't pressed, the robot just sits there, you can run loops in setup
    digitalWrite(LED1_pin, HIGH); // LED is on so you know the robot is ready
    buttonState = digitalRead(button_Pin); // Keep checking to see if the button is pressed
  }

  digitalWrite(LED1_pin, LOW); // After the condition is met the loop ends, LED goes off to indicate this
  delay(5000);                 // Now wait 5 sec, dealy is in milisecond
  servoR.attach(servoR_pin);   // Attaches the servo on pin (defined earlier) to the servo object
  servoL.attach(servoL_pin);   // You attach the servos after the 5 second wait so they won't move at all before time is up, in case they aren't calibrated properly

 // Serial.begin(9600); //uncomment if using serial communication
}

// Main loop, this will run continuously until the controller is reset
void loop() {

  sensors();

  // Average out the sensor data from the midranges
  // Mid R averaging
  midR_total = midR_total - midR_readings[index];
  // Read from the sensor:
  midR_readings[index] = midR;
  // Add the reading to the midR_total:
  midR_total = midR_total + midR_readings[index];
  // Advance to the next position in the array:
  midR_avg = midR_total / numReadings;

  // Mid L averaging
  midL_total = midL_total - midL_readings[index];
  // Read from the sensor:
  midL_readings[index] = midL;
  // Add the reading to the midL_total:
  midL_total= midL_total + midL_readings[index];
  // Advance to the next position in the array:
  midL_avg = midL_total / numReadings;

  // Progress the index
  index = index + 1;
  // If we're at the end of the array...
  if (index >= numReadings) {
    // ...wrap around to the beginning:
    index = 0;
  }

  // Target in front
  if (prox == LOW){ // The front proximity sensor goes low when it detects an object
    motors(100, 100); // Sets motors to drive full forward
    digitalWrite(LED1_pin, HIGH);
    digitalWrite(LED2_pin, HIGH);
    delay(50);
  }

  // Midrange Sensor checks
  else if (midR_avg > threshold && midL_avg > threshold) { // Both mid range sensors are above the detection threshold
    motors(100,100);  // Full foward
    digitalWrite(LED1_pin, HIGH);
    digitalWrite(LED2_pin, HIGH);
  }

  else if (midL_avg > threshold && midR_avg < threshold) { // Only mid L spots
    motors(-20,100);  // Turn left
    digitalWrite(LED2_pin, HIGH); // Signal with left LED
    digitalWrite(LED1_pin, LOW);
  }


  else if (midR_avg > threshold && midL_avg < threshold) { // Only midR spots
    motors(100,-20); // Turn right
    digitalWrite(LED1_pin, HIGH); // Signal with right LED
    digitalWrite(LED2_pin, LOW);
  }

  // No robots, time to check to see if you are driving out of the arena

  else if (lineR == HIGH && lineL == HIGH) {   // If both line sensor are on black, full ahead
    motors(100,100);
    digitalWrite(LED1_pin, LOW);
    digitalWrite(LED2_pin, LOW);
  }

  else if (lineR == LOW && lineL == HIGH) { // Right line sensor hits the white line and triggers low
    motors(-100, -100);           // Back up
    delay(200);                  // Only back up for .2 seconds
    end_time = millis() + 400;   // Find the time 400 miliseconds into the future
    while(millis() < end_time) { // Spin away from edge untill the time limit is hit
      motors(-100, 100);
      sensors();
      if(midL > threshold || midR > threshold || prox == LOW){  // If you see a target, break loop
        break;
      }
      digitalWrite(LED1_pin, LOW);
      digitalWrite(LED2_pin, LOW);
    }
  }

  else if (lineR == HIGH && lineL == LOW) { // Left line sensor hits
    motors(-100, -100);  // Back up
    delay(200);        // Only back up for .2 seconds
    end_time = millis() + 400;
    while(millis() < end_time) {
      motors(100, -100);  //spin away from edge
      sensors();
      digitalWrite(LED1_pin, LOW);
      digitalWrite(LED2_pin, LOW);
      if(midL > threshold || midR > threshold || prox == LOW){
        break;
      }
      digitalWrite(LED1_pin, LOW);
      digitalWrite(LED2_pin, LOW);
    }
  }

  else if(lineR == LOW && lineL == LOW) { // Both sensors are on the line, not a good position
    motors(-100, -100);  // Back up
    delay(200);        // Only back up for .2 seconds
    end_time = millis() + 400;
    while(millis() < end_time){
      motors(-100, 100);  // Spin away from edge
      sensors();
      if(midL > threshold || midR > threshold || prox == LOW){
        break;
      }
      digitalWrite(LED1_pin, LOW);
      digitalWrite(LED2_pin, LOW);
    }
  }

  else { // For some reason no conditions trigger keep going forward, just a backup for the unseen because I'm a bit paranoid with my code, there is no reason for this condition to occur
    motors(100, 100);
    digitalWrite(LED1_pin, LOW);
    digitalWrite(LED2_pin, LOW);
  }

  delay(5); // A slight delay at the end of the code is good for stability, and prevents jitery movement.
} // end loop


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Functions, normaly these are put in a header file, but are placed here for your convience
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


// This function reads all of the values from the sensors
void sensors() {
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
  Serial.print("Midrange Right Average = ");
  Serial.println(midR_avg);
  Serial.print("Midrange Left = ");
  Serial.println(midL);
  Serial.print("Midrange Left Average = ");
  Serial.println(midL_avg);
  Serial.print("Prxoimity = ");
  Serial.println(prox);
  Serial.print("Right Line Sensor = ");
  Serial.println(lineR);
  Serial.print("Left Line Sensor = ");
  Serial.println(lineL);
  Serial.println("--------------------------------------");
}

// This funtion is used to control the motors
void motors(int l, int r) {      // Syntax motors(left motor value, right motor value)
  r = map(r, -100, 100, 180, 0); // Normal servo library uses 0 to 180 to define postion for a regular servo
  l = map(l, -100, 100, 0, 180); // We have modified our servos for continuous rotation so 0 is full back, 180 is full forward, and 90 is stop
  servoL.write(l);               // This is remapped to -100 to 100 for convenience and flipped for one motor because it will me mounted backwards on the chassis
  servoR.write(r);
}
