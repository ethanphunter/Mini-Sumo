/*
  File: sensorTest.ino
  Author: Nate Lawernce-Richards
  Description: This code is for testing all 5 sensors on a basic robot.
*/

// #define can be used for when a value does not need to be change
#define lineR_pin 5 // Line Sensors
#define lineL_pin 4
#define midR_pin A5 // Midrange Sensors
#define midL_pin A4 // A stands for analog 
#define prox_pin 6  // Proximity Sensor
#define LED1_pin 12 // LED 2
#define LED2_pin 13 // LED 1

// int declares that these variables will be intergers
int lineL;
int lineR;
int prox;
int midR;
int midL;

void setup() {
  pinMode(lineR_pin, INPUT); // Declare pinmode for line tracking
  pinMode(lineL_pin, INPUT);
  pinMode(midR_pin, INPUT);  // Declare pinmode for midrange sensors
  pinMode(midL_pin, INPUT);
  pinMode(prox_pin, INPUT);  // Declare pinmode for proximity detector
  pinMode(LED1_pin, OUTPUT);
  pinMode(LED2_pin, OUTPUT);

  Serial.begin(9600); // Start serial communication
}

void loop(){ // Main loop
  sensors(); // This is a user made function, the functions are defined after the loop, this one reads all of the sensors

  sensors_print(); // Another funtion that we have written
  delay(2000);
}

// ----------------------------------------------------------------------------------------------------------------------
// Funtions
// ----------------------------------------------------------------------------------------------------------------------

/*
This function reads all of the values from the sensors
*/
void sensors() {
  midR = analogRead(midR_pin);
  midL = analogRead(midL_pin);
  prox = digitalRead(prox_pin);
  lineR = digitalRead(lineR_pin);
  lineL = digitalRead(lineL_pin);
}

/*
This function prints off all sensor values to serial port, it doesn't return any
 value to the main loop so it is return type is void
*/
void sensors_print() {
  Serial.print("Midrange Right = "); // `print` prints the given string out to the serial monitor
  Serial.println(midR);              // `println` adds a newline character onto the end where `print` does not
  Serial.print("Midrange Left =");
  Serial.println(midL);
  Serial.print("Proximity = ");
  Serial.println(prox);
  Serial.print("Right Line Sensor = ");
  Serial.println(lineR);
  Serial.print("Left Line Sensor = ");
  Serial.println(lineL);
  Serial.println("--------------------------------------");
}
