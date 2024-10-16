/*
 
---------------------------------------------------------
     /\    / ____|__   __|  __ \ / __ \ / ____|
    /  \  | (___    | |  | |__) | |  | | (___  
   / /\ \  \___ \   | |  |  _  /| |  | |\___ \ 
  / ____ \ ____) |  | |  | | \ \| |__| |____) |
 /_/    \_\_____/   |_|  |_|  \_\\____/|_____/ 
Arduino Mega2560                                          
Jason Deatrick
October 16, 2024
Version: A.1.0SRV_LCD
---------------------------------------------------------

*/
// Arduino Libraries
#include <BraccioRobot.h>
#include <Servo.h>
#include <LCD_I2C.h>

// Arduino I/O
int ledPin = 13;
int safetyPin = A0;

// COM
int convLevel = 0;
byte com1RX = 0;
LCD_I2C lcd(0x27);

// Braccio I/O
int braccioHomeStart = 0;
int braccioAtHome = 0;
int braccioTakeStart = 0;
int braccioAtTake = 0;
int braccioTakeHomeStart = 0;
int braccioAtTakeHome = 0;
int braccioHoldingObject = 0;

// Variables
boolean initialBoot = true;
boolean alwaysTrue = true;
boolean alwaysFalse = false;
boolean safetyActive = true;
boolean lcdBootMessageLatch = true;

// Braccio Positions
Position braccioHome;
Position braccioTake;
Position braccioTakeCloseGripper;
Position braccioTakeHome;

void setup() {
// Setup Serial1 UART Baud Rate
  Serial1.begin(9600);

// Setup LCD
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Initializing...");

// I/O
  pinMode(ledPin, OUTPUT);
  pinMode(safetyPin, INPUT);
  digitalWrite(ledPin, LOW);
  
}

void loop() {
  // Network Scan START (Serial 1)
  while (Serial1.available()) {
    com1RX = Serial1.read();
    if (com1RX == 0x30) { // Check for "0x30" from Serial Bus
      if (braccioHoldingObject == 1) {
        // run PutDown program
      } else {
        braccioHomeStart = 1; // Start Braccio Home Program
        goto homeProgram;
        returnToHomeSerial:
        com1RX = 0;
      }
    }
    if (com1RX == 0x31) { // Check for "0x31" from Serial Bus
      if (alwaysTrue == true) {
        braccioTakeStart = 1; // Start Braccio Take Program
        goto takeProgram;
        returnToTakeSerial:
        delay(1000);
        com1RX = 0;
        }
      }
    if (com1RX == 0x32) { // Check for "0x32" from Serial Bus
      if (alwaysTrue == true) {
        braccioTakeHomeStart = 1; // Start Braccio Wake Program
        goto takeHomeProgram;
        returnToTakeHomeSerial:
        //delay(1000);
        //Serial1.write(0xAA); // Start Bit
        //Serial1.write(0x00); // Send "Waiting" Command to VCM
        //delay(3000);
        //Serial1.write(0xAA); // Start Bit
        //Serial1.write(0x22); // Send "Group 2" Command to VCM
        com1RX = 0;
        }
      }
    }
  // Network Scan END (Serial 1)

  // LCD Message START
  if (initialBoot == false); {
    if (lcdBootMessageLatch == true) {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Initialization");
      lcd.setCursor(4,1);
      lcd.print("Complete");
      delay(3000);
      lcd.clear();
      lcdBootMessageLatch = false;
    }
  }
  if (braccioHomeStart == 1) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Running Program:");
    lcd.setCursor(6,1);
    lcd.print("Home");
  }
  // LCD Message END

  // safetyActive Condition must be made true for robot to move
  if (safetyActive == true) {
    
    // Set Positons                   ( M1,  M2,  M3,  M4,  M5,  M6)
    braccioHome.set                   (  0,  55,  50,   0,   0,  70);
    braccioTake.set                   (140,  80,  50,  20,   0,   0);
    braccioTakeCloseGripper.set       (140,  80,  50,  20,   0,  50);
    braccioTakeHome.set               (  0,  55,  50,   0,   0,  50);

    // Initial Boot Program START
    if (initialBoot == true) {
      if (alwaysTrue == true) {
        delay(1000);
        BraccioRobot.init();
        delay(1500);
        BraccioRobot.moveToPosition(braccioHome, 75);
        braccioAtHome = 1;
        initialBoot = false;
      }
    }
    // Initial Boot Program END

    // Home Program START
    homeProgram:
    if (braccioHomeStart == 1) {
      if (braccioHoldingObject == 1) {
        braccioTakeHomeStart = 1; // Start Braccio Home Program
        goto takeHomeProgram;
      } else {
        delay(1000);
        BraccioRobot.moveToPosition(braccioHome, 90);
        //delay(5000);
        braccioAtHome = 1;
        braccioAtTake = 0;
        braccioAtTakeHome = 0;
        braccioHomeStart = 0;
        goto returnToHomeSerial;
      }
    }
    // Home Program END

    // Take Program START
    takeProgram:
    if (braccioTakeStart == 1) {
      if (braccioHoldingObject == 1) {
        // run PutDown program
      } else {
        delay(1000);
        BraccioRobot.moveToPosition(braccioTake, 99);
        //delay(5000);
        braccioAtHome = 0;
        braccioAtTake = 1;
        braccioAtTakeHome = 0;
        braccioTakeStart = 0;
        goto returnToTakeSerial;
      }
    }
    // Take Program END

    // TakeHome Program START
    takeHomeProgram:
    if (braccioTakeHomeStart == 1) {
      if (alwaysTrue = true) {
        BraccioRobot.moveToPosition(braccioTakeCloseGripper, 99);
        delay(2000);
        BraccioRobot.moveToPosition(braccioTakeHome, 99);
        //delay(5000);
        braccioAtHome = 0;
        braccioAtTake = 0;
        braccioAtTakeHome = 1;
        braccioTakeHomeStart = 0;
        braccioHoldingObject = 1;
        goto returnToTakeHomeSerial;
      }
    }
    // TakeHome Program END
  }
}
