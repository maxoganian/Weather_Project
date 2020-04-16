// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include <SparkFunDS1307RTC.h>

const int chipSelect = 10;
unsigned long timeSinceStart = 0;

#define SQW_INPUT_PIN 2   // Input pin to read SQW
#define SQW_OUTPUT_PIN 13 // LED to indicate SQW's state

void setup()
{
  Wire.begin();                // join i2c bus as master
  Serial.begin(9600);           // start serial for output

  Serial.begin(9600);
  
  pinMode(SQW_INPUT_PIN, INPUT_PULLUP);
  pinMode(SQW_OUTPUT_PIN, OUTPUT);
  digitalWrite(SQW_OUTPUT_PIN, digitalRead(SQW_INPUT_PIN));
  
  rtc.begin(); // Call rtc.begin() to initialize the library
  // (Optional) Sets the SQW output to a 1Hz square wave.
  // (Pull-up resistor is required to use the SQW pin.)
  rtc.writeSQW(SQW_SQUARE_1);

  // Now set the time...
  // You can use the autoTime() function to set the RTC's clock and
  // date to the compiliers predefined time. (It'll be a few seconds
  // behind, but close!)
  rtc.autoTime();
  rtc.set12Hour(); // Use rtc.set12Hour to set to 12-hour mode
  
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
}
int i = 0;
int data[8];
void loop()
{
    Wire.requestFrom(9,64);
    
    while(Wire.available()){
      data[i] = Wire.read(); // receive byte as a character
      //.print("0");
      //Serial.println(i);
      // make a string for assembling the data to log:
      String dataString = "";
    
      // read three sensors and append to the string:
      dataString += String(data[i]);
       
      // if the file is available, write to it:
      if(data[i]  < 100){
        // open the file. note that only one file can be open at a time,
        // so you have to close this one before opening another.
        File dataFile = SD.open("windtest.txt", FILE_WRITE);
      
        if (dataFile) {
          if(i == 0){           
            dataFile.print("Light: ");
            Serial.print("Light: ");
          }
          if(i == 1){
            dataFile.print("isTooDry: ");
            Serial.print("isTooDry: ");    
          }
          if(i == 2){
            dataFile.print("Moisture 1: ");
            Serial.print("Moisture 1: ");
          }
          if(i == 3){
            dataFile.print("Too Dry Value 1: ");
            Serial.print("Too Dry Value 1: ");
          }
          if(i == 4){
            dataFile.print("Moisture 2: ");
            Serial.print("Moisture 2: ");
          }
          if(i == 5){
            dataFile.print("Too Dry Value 2: ");
            Serial.print("Too Dry Value 2: ");
          }
          if(i == 6){
            dataFile.print("Wind Reading: ");
            Serial.print("Wind Reading: ");
          }
          if(i == 7){
            dataFile.println("------------");
            Serial.println("------------");
          }      
          if(i != 7){
            dataFile.println(dataString);
            Serial.println(dataString);         // print the character
          }  
          Serial.print("i = ");
          Serial.println(i);
          dataFile.close();
        }
        // if the file isn't open, pop up an error:
        else {
          Serial.println("error opening datalog.txt");
        }
      }
      if(data[i]==99){
        i=7;
      }
      i++;
      if(i>7){ 
        //Serial.println("Reseting i");
        i = 0;
      }
    //delay(100);
  }
}

void printTime()
{ 
  
}
