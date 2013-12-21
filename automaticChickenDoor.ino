/*
Automatic Chicken Coop Door
*/
#include <LiquidCrystal.h>

// initialize the LCD library
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// L293D Pins
int enablePin = 4;
int in1Pin = 5;
int in2Pin = 6;
int powerPin = 3;
int directionPin = 2;
int delayTime = 1000;



// Used for testing
int motorStopped = true;
int reverse = false;
int isPowerPressed = true;
int isDirectionPressed = true;

String powerText = "Motor Stopped";
String directionText = "Not Going";


//int motorPin = 3;
int ledPin = 13;

void setup() {

 Serial.begin(9600);
 
 digitalWrite(ledPin, LOW);
 delay(2000);
 
 initDoor(); 
 digitalWrite(ledPin, HIGH);
 
 // Initialize Motor Pin Settings
 pinMode(in1Pin, OUTPUT);
 pinMode(in2Pin, OUTPUT);
 pinMode(enablePin, OUTPUT);
 pinMode(powerPin, INPUT_PULLUP);
 pinMode(directionPin, INPUT_PULLUP);
  
 // Send a greeting when the arduino has initialized.
 lcdGreeting();
 
 digitalWrite(ledPin, LOW);
 displayOpenCloseTimes();
 digitalWrite(ledPin, HIGH);
 // while (! Serial);
 //Serial.println("Speed 0 to 255");
 //Serial.println("If speed is 25, the red light will come on :)"); 
}

void loop() {
  
    manualControl();
    
    //timeControl();
    
    paintDisplay();

    
   
  
} // END LOOP  
  
// Paints the Variables and text to the 
// LED Screen.
void paintDisplay() {
    lcd.clear();
    lcd.print(powerText);
    lcd.setCursor(0, 1);
    lcd.print(directionText);  
}


// Door opens/closes based on time

// Manual Control Functionality
void manualControl() {
  
  isPowerPressed = digitalRead(powerPin);
  isDirectionPressed = digitalRead(directionPin);
  
  if(!isPowerPressed || !isDirectionPressed) {
    
    if(!isPowerPressed) {
      motorStopped = !motorStopped;
    
      //digitalWrite(in1Pin, motorStopped);
      //digitalWrite(in2Pin, !motorStopped);
    
      // Stop / Start Code
      if(motorStopped) {
        digitalWrite(enablePin, 0);
        powerText = "Door Stopped";
      } else {
        digitalWrite(enablePin, 255);
        powerText = "Door Moving";
      }
    }
      
      //Direction Code
    if(!isDirectionPressed) {
     reverse = !reverse; 
     
     if(reverse) {
       digitalWrite(in1Pin, LOW);
       digitalWrite(in2Pin, HIGH);
       directionText = "Up";
     } else {
       digitalWrite(in1Pin, HIGH);
       digitalWrite(in2Pin, LOW);
       directionText = "Down";
     }  
       
    }
    // Stop the code so we can debounce
    // the button.
    delay(delayTime); 
    
     // Reset Button Variables
    isPowerPressed = true;
    isDirectionPressed = true;
  } // END POWER OR DIRECTION PRESSED
  
}


// Initialize the LCD Screen
void initDoor() {
  
   lcd.begin(16, 2);
   lcd.print("The chicken door");
   lcd.setCursor(0, 1);
   lcd.print("is booting up..."); 
   Serial.println("init Door");
   
}

// A greeting for the Natty
void lcdGreeting()
{
   Serial.println("lcd Greeting");
   lcd.clear();
   lcd.print("Hello Natty :)"); 
   delay(2000); 
   lcd.clear();
   lcd.print("The chicken door");
   lcd.setCursor(0, 1);
   lcd.print("is ready.");
   delay(1500);
}

// Display the times the door will
// open and close.
void displayOpenCloseTimes() {
   lcd.clear();
   lcd.print("Door open/close");
   lcd.setCursor(0, 1);
   lcd.print("times are:");
   delay(2000);
   Serial.println("Display Open Close Times");
   
   lcd.clear();
   lcd.print("Open:  9:00 A.M.");
   lcd.setCursor(0, 1);
   lcd.print("Close: 8:00 P.M.");
}


