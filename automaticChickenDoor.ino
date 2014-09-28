/*
Automatic Chicken Coop Door
*/
#include <LiquidCrystal.h>
#include <Time.h>

// Low Power consumption library
// #include <Narcoleptic.h>
#include <LowPower.h>

// Time Settings
#define TIME_MSG_LEN 11 // time sync is HEADER followed by Unix time_t as 10 ASCII digits
#define TIME_HEADER 'T'
#define TIME_REQUEST 7

// DEBUG SETTINGS
// Seconds to move up and down.
int upTime = 20000;
int downTime = 18000;  //NOT USED
String upTimeText = "Up for 20"; 
String downTimeText = "Down for 18";


// %%%%%%%%  USEFUL SETTINGS %%%%%%%%
int upHour = 17;
int upMinute = 46;
int downHour = 19;
int downMinute = 05;
int isDoorOpen = false;
// %%%%%%%%  USEFUL SETTINGS %%%%%%%%


int EST = 60 * 60 * 5; //offset 18000 seconds or -5 hours.
int ESTDST = 60 * 60 * 4; //offset 14400 seconds or -4 hours.
int savedTime = 0;


// initialize the LCD library
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// L293D Pins
int enablePin = 4;
int in1Pin = 5;
int in2Pin = 6;
int powerPin = 3;
int directionPin = 2;
int miniSwitchPin = 13;
int delayTime = 1000;
int miniSwitchValue = 0;
int mode = 0;
int modeSet = false;
int doorClosed = false;

// Time Settings



int isDoorMoving = false;
int acd_timeSet = false;


int timeSynced = false;



// Used for testing
int motorStopped = true;
int reverse = false;
int isPowerPressed = true;
int isDirectionPressed = true;
int isDisplayPainted = false;

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
   pinMode(miniSwitchPin, INPUT);
    
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
  
  // ########## MAIN CONTROL ############
  if (mode == 1) {
    if (!modeSet) {
       modeSet = true;     
       lcd.clear();
       lcd.print("Init.. Manual");
       delay(1000);
    }

    manualControl();  
  
  } else if(mode == 2) {
    if(!acd_timeSet) {
      if(!modeSet) {
        lcd.clear();
        lcd.print("Init.. Prod Mode");
      }
      if(!modeSet) {
        modeSet = true;
      }
      delay(1000);
    }
    productionMode();

  } else {
    chooseMode(); 
  }
  
} // END LOOP  

void productionMode() {
  // Once we are in this loop, we want to stay here
  while(true) {
    // Vars to use are: upTime, upHour, upMinute, downHour, 
    // downMinute, isDoorOpen
    

    // Check to see if the time is set. If
    // it is not. We need to do that first
    // before continuing.
    if (Serial.available() && !timeSynced) {
        processSyncMessage();
        lcd.clear();
        lcd.print("Time is synced");
        Serial.println("Time is synced");
        timeSynced = true;
        acd_timeSet = true;
        delay(3000);
    }

    if(timeStatus() == timeNotSet) {
        Serial.print("Time Status: ");
        Serial.println(timeStatus());
        
        lcd.clear();
        lcd.print("Time Sync");
        lcd.setCursor(0, 1);
        lcd.print("Required");
        delay(2000);
    } else {
        lcd.clear();

         //########## SLEEP EXPERIMENT - BEGIN ############  
         // If the current time is NOT an (hour and 2 hours) before or 10 minutes after the time
         // the door is going to close.  Let's sleep.  We want NattyP to be able to 
         // do a manual override in the hour(s) before the door closes.  We don't want
         // her to worry that it didn't work.
//         if(!(hour() == downHour && minute() < 10) || (!(hour() == (downHour - 1)) || !(hour() == (downHour - 2)))) {
//           Narcoleptic.delay(8000);
//           // For testing purposes, we will mark the display here so we know its working.
//           // ND - Narcoleptic Down
//           lcd.setCursor(14, 1);
//           lcd.print("ND");
//           delay(1000);   
//           lcd.setCursor(0, 0);
//         }
         
         // We also want to sleep if not close to the couple of minutes before 
         // and after the time we open the door.
//         else if(!(hour() == (upHour - 1) && minute() > 50) || !(hour() == upHour && minute() < 10)) { 
//           Narcoleptic.delay(8000);
//           // For testing purposes, we will mark the display here so we know its working.
//           // NU - Narcoleptic Up
//           lcd.setCursor(14, 1);
//           lcd.print("NU");
//           delay(1000);
//           lcd.setCursor(0, 0);
//         } 
         
         // Testing for what it is, is much easier than testing for what its not.  Let's try that.
         // Let's go Narcoleptic if:
         // 1. If the hour is less than upHour - 2
         // 2. If the hour is upHour - 1 and minute is less than 50.
         // 3. If the hour is upHour and minute is less than 3 or greater than 3.
         // 4. If the hour is greater than upHour and less than downHour - 2.
         // 5. If the hour is downHour - 1 and minute is less than 50.
         // 6. If the hour is downHour and minute is less than 3 or greater than 3.
         // 7. If the hour is greater than downHour
         Serial.print("UpMinute ");
         Serial.println((upMinute - 3));
         Serial.print("Minute ");
         Serial.println(minute());
         Serial.print("is Minute less than up minute: ");
         Serial.println(minute() < (upMinute - 3));
         Serial.print("Is 3 true: ");
         Serial.println(((hour() == upHour) && ((minute() < (upMinute - 3)) || (minute() > (upMinute + 3)))));
         Serial.println("----------");
         if((hour() < (upHour - 1)) || // 1
            //((hour() == (upHour - 1)) && (minute() < (upMinute - 3))) || // 2
            ((hour() == upHour) && ((minute() < (upMinute - 3)) || (minute() > (upMinute + 3)))) || // 3
            (hour() > upHour && (hour() < (downHour - 2))) || // 4
            //((hour() == (downHour - 1)) && (minute() < (downMinute - 3))) || //5
            (hour() == downHour && ((minute() < (downMinute - 3)) || (minute() > (downMinute + 3)))) || // 6
            (hour() > downHour) // 7
             
            ) {
               // Narcoleptic turns off virtually everything.  Even
               // the timer. We have to fast forward the time using
               // Narcoleptics millis function.
               //savedTime = now();
               //Narcoleptic.delay(8000);
               //setTime(savedTime + Narcoleptic.millis());
               // For testing purposes, we will mark the display here so we know its working.
               // NARC 
               // DEBUG
               LowPower.idle(SLEEP_4S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_ON, SPI_ON, USART0_ON, TWI_ON);
               lcd.setCursor(12, 1);
               lcd.print("LOW");
               delay(1000);
               lcd.setCursor(0, 0);
            }
           
         
         
      
            
         //########## SLEEP EXPERIMENT - END ############  

        // Print the current time on line 1
        lcd.print("Current: ");
        lcd.print(hour());
        printDigits(minute());
        //lcd.print(":");
        //lcd.print(minute());
        

        // Print the next Trigger Time on line 2
        lcd.setCursor(0, 1);
        
        // START - SECTION - CLOSING DOOR
        
        if(isDoorOpen) {
            lcd.print("Down at ");

            lcd.print(downHour);
            //lcd.print(":");
            //lcd.print(downMinute);
            printDigits(downMinute);
            

            //Test for manual override
            isPowerPressed = digitalRead(powerPin);
            
            // If the current time is equal
            // to the time set for closing the door, let's
            // do that.
            if(hour() == downHour && minute() == downMinute) {
              
              isDoorMoving = true;
              
            } else if (!isPowerPressed) {
              isDoorMoving = true;
              lcd.clear();
              lcd.print("Manual Override");
              lcd.setCursor(0, 1);
              lcd.print("Close Door in 3s");
              delay(3000);
            }
            
            if(isDoorMoving) {
                // Set the door to move down on the L293D IC
                digitalWrite(in1Pin, HIGH);
                digitalWrite(in2Pin, LOW);
              
                // Turn on the motor
                digitalWrite(enablePin, 255);
                lcd.clear();
                lcd.print("Door Closing...");
                
                miniSwitchValue = digitalRead(miniSwitchPin);
                
                // This loop should continue to run until the
                // switch has been triggered and then leave.
                int testOne = 0;
                while(miniSwitchValue) {
                  
                  miniSwitchValue = digitalRead(miniSwitchPin);
                  
                  // We can slow it down to 100 cycles per second
                  // instead of 1000.
                  delay(100);
                  lcd.print("Down Test");
                  lcd.print(testOne);
                  testOne++;
                }
                
                lcd.clear();
                lcd.print("Door Closed");
                
                // Stop the door 
                digitalWrite(enablePin, 0);
                isDoorMoving = false;
                isDoorOpen = false;
                isPowerPressed = true;
                
                // Let the door closed message display for a
                // second.
                delay(3000);
            } // END - isDoorMoving
        } // END = isDoorOpen
        
        // END - SECTION - CLOSING DOOR
        
        // START - SECTION - OPENING DOOR
        
        else {
          
         lcd.print("Up at: ");
         lcd.print(upHour);
         //lcd.print(":");
         //lcd.print(upMinute);
         printDigits(upMinute);
         
         // If the time is equal to the set time
         // for opening the door, let's do that.
         if(hour() == upHour && minute() == upMinute) {
           digitalWrite(in1Pin, LOW);
           digitalWrite(in2Pin, HIGH);
           
           isDoorMoving = true;
           
         }
         
         if(isDoorMoving) {
           lcd.clear();
           lcd.print("Door Opening...");
           digitalWrite(enablePin, 255);
           
           // Open the door for the set
           // amount of time.
           delay(upTime);
           digitalWrite(enablePin, 0);
           isDoorMoving = false;
           isDoorOpen = true;
           
           lcd.clear();
           lcd.print("Door is now open");
           
           // Show that message for a sec.
           delay(3000);
         }
         
          
        }
        
        // END - SECTTION - OPENING DOOR   
    }

    // Whoa.. Hold on there partner
    if(!acd_timeSet) {
      lcd.clear();
      lcd.print("Main Loop");
      lcd.setCursor(0, 1);
      lcd.print("Complete");
    }
    
    delay(3000);
  } // END - while(true) loop to stay in Prod Mode
}

  


  
// Paints the Variables and text to the 
// LED Screen.
// @param - d Time to delay before display
void paintDisplay(int d) {
    delay(d);
    lcd.clear();
    lcd.print(powerText);
    lcd.setCursor(0, 1);
    lcd.print(directionText);  
}

void miniSwitchTest() {
 miniSwitchValue = analogRead(miniSwitchPin);
 delay(300);
 lcd.clear();
 lcd.print("Switch: ");
 lcd.setCursor(10, 0);
 lcd.print(miniSwitchValue); 
 if(miniSwitchValue > 100) {
   lcd.setCursor(5, 1);
   lcd.print("SWITCH!!");
 }
}


// Door opens/closes based on time

// Manual Control Functionality
void manualControl() {
  
   
   
   // Check for Power / Direction Button Pressed   
   isPowerPressed = digitalRead(powerPin);
   isDirectionPressed = digitalRead(directionPin);
   miniSwitchValue = digitalRead(miniSwitchPin);
   /*
   // Test for the closed door switch
   miniSwitchValue = digitalRead(miniSwitchPin);
   lcd.print("Switch ");
   lcd.setCursor(0,1); 
   lcd.print(miniSwitchValue);
   if(!miniSwitchValue) {
    lcd.setCursor(5, 1);
    lcd.print("Stop"); 
   }
   */
   
   
   if (!miniSwitchValue && doorClosed == false) {
      doorClosed = true;
      // If the swtich has been triggered, the 
      // door was moving down.  We should switch 
      // the motor to the stop position and change
      // the direction to up.
      isPowerPressed = false;
      isDirectionPressed = false;
      Serial.println("The switch has been triggered");
   }
   
   Serial.print("Switch Value: ");
   Serial.println(miniSwitchValue);

   if(!isPowerPressed || !isDirectionPressed) {
     Serial.print("Power Button Mode: ");
     Serial.println(isPowerPressed);
     Serial.print("Direction Button Mode: ");
     Serial.println(isDirectionPressed);    
     
     
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
        powerText = "Door Moving - NC";
        delay(2000);
        doorClosed = false;
        powerText = "Door Moving - C";
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
    
    // Change the Display to reflect changes.
    Serial.println("Are we painting");

    
    
    // Stop the code so we can debounce
    // the button.
    delay(delayTime); 
    
     // Reset Button Variables
    isPowerPressed = true;
    isDirectionPressed = true;
    
    
  } // END POWER OR DIRECTION PRESSED */
  
  
  paintDisplay(200);
  
}

void fixedTimeMovements() {
  
     isPowerPressed = digitalRead(powerPin);
     isDirectionPressed = digitalRead(directionPin);
     
     
     if(!isDisplayPainted) {
       isDisplayPainted = true;
       
       lcd.clear();
       lcd.print("Press power to");
       lcd.setCursor(0, 1);
       lcd.print("start!!");
     }
  

  
  if(!isPowerPressed) {
    
     isDisplayPainted = false;
     lcd.clear();
     lcd.print("Countdown in 5");
     lcd.setCursor(0,1);
     
     delay(3000); 
     int counter = 5;
     for (int i = counter; i > 0; i--) {
       lcd.clear();
       lcd.print(i);
       lcd.setCursor(0, 1);
       delay(1000);
     } 
     
     lcd.clear();
     lcd.print(upTimeText);
     
     digitalWrite(in1Pin, LOW);
     digitalWrite(in2Pin, HIGH);
     digitalWrite(enablePin, 255);
     delay(upTime);
     
     digitalWrite(enablePin, 0);
     lcd.clear();
     lcd.print("Stop for 3");
     delay(3000);
     
     lcd.clear();
     lcd.print(downTimeText);
     
     digitalWrite(in1Pin, HIGH);
     digitalWrite(in2Pin, LOW);
     digitalWrite(enablePin, 255);
     delay(downTime);
     
     digitalWrite(enablePin, 0);
     lcd.clear();
     lcd.print("Test Complete");
     delay(500);
     
  }
     
     
     
     
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
   delay(1000);
}

// Display the times the door will
// open and close.
void displayOpenCloseTimes() {
   lcd.clear();
   lcd.print("Door open/close");
   lcd.setCursor(0, 1);
   lcd.print("times are:");
   delay(1000);
   Serial.println("Display Open Close Times");
   
   lcd.clear();
   lcd.print("Open: ");
   lcd.print(upHour);
   //lcd.print(":");
   //lcd.print(upMinute);
   printDigits(upMinute);
   lcd.setCursor(0, 1);
   lcd.print("Close: ");
   lcd.print(downHour);
   printDigits(downMinute);
   //lcd.print(":");
   //lcd.print(downMinute);
   delay(2000);
}

void chooseMode () {
 lcd.clear();
 lcd.print("Choose Mode: ");
 lcd.setCursor(0, 1);
 lcd.print("1-M | 2-P");
 
 if (mode == 0) {
   if (!digitalRead(powerPin)) {
     mode = 2;
   } else if (!digitalRead(directionPin)) {
     mode = 1;  
   }
 }   
 delay(300); 

}

void doTimeTest() {
   lcd.clear();
   lcd.print("Time: ");
  
   if(Serial.available()) {
    
     processSyncMessage();
    
   } 
   
   if(timeStatus() == timeNotSet)
     Serial.println("waiting for sync message");
   else
     digitalClockDisplay();
   delay(1000);
    
}

void digitalClockDisplay() {
 
 //digital clock display of time
 lcd.setCursor(9, 1);
 lcd.print(hour());
 printDigits(minute());
 printDigits(second()); 
 
}

void printDigits(int d) {
 // utility function for digital clock display
 // ## prints preceding colon and leading 0
 
 lcd.print(":");
 if (d < 10)
   lcd.print('0');
 lcd.print(d);

}

void processSyncMessage() {
  // if time sync available from serial 
  // port, update time and return true
  
  while(Serial.available() >= TIME_MSG_LEN) {
    // time message consists of header 
    // & 10 ASCII digits
    char c = Serial.read();
    Serial.print(c);
    if ( c == TIME_HEADER) {
     time_t pctime = 0;
     for (int i = 0; i < TIME_MSG_LEN - 1; i++) {
      c = Serial.read();
      if ( c >= '0' && c <= '9') {
       pctime = ( 10 * pctime) + (c - '0'); //convert digits to a number 
      }
     }
     
     // Sync Arduino clock to the time received on the serial port
     // We use EST to offset the GMT to our own EST (-5)
     setTime(pctime - ESTDST);
    }
    
  }
}



