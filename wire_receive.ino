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

String fileName = "4-16-20.txt";
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

  //Use the RTC demo software to set the time.
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
    //request one byte of data from device 9 
    Wire.requestFrom(9,1);
    
    while(Wire.available()){
      data[i] = Wire.read(); // receive byte as a character
    }
    //Check to make sure we have real data
    if(data[i]  < 100){
        // open the file. note that only one file can be open at a time,
        // so you have to close this one before opening another.
      
        File dataFile = SD.open(fileName, FILE_WRITE);
        
        String dataString = "";
    
        // read three sensors and append to the string:
        dataString += String(data[i]);
       
      
        if (dataFile) {
          if(i == 0){           
            rtc.update();
            dataFile.close();
            printTime();
            
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
          //Serial.print("i = ");
          //Serial.println(i);
          dataFile.close();
        }
        // if the file isn't open, pop up an error:
        else {
          Serial.println("error opening datalog.txt");
        }
    }
    if(data[i]==99){
       //Serial.println("Setting i to 7");
       i=7;
    }
    i++;
    if(i>7){ 
      //Serial.println("Reseting i");
      i = 0;
    }
}
        
void printTime()
{
  File dataFile = SD.open(fileName, FILE_WRITE);
  
  Serial.print(String(rtc.hour()) + ":"); // Print hour
  dataFile.print(String(rtc.hour()) + ":"); // Print hour
  
  if (rtc.minute() < 10){
    Serial.print('0'); // Print leading '0' for minute
    dataFile.print('0'); // Print leading '0' for minute
  }
  Serial.print(String(rtc.minute()) + ":"); // Print minute
  dataFile.print(String(rtc.minute()) + ":"); // Print minute
  
  if (rtc.second() < 10){
    Serial.print('0'); // Print leading '0' for second
    dataFile.print('0'); // Print leading '0' for second    
  }
  Serial.print(String(rtc.second())); // Print second
  dataFile.print(String(rtc.second())); // Print second

  if (rtc.is12Hour()) // If we're in 12-hour mode
  {
    // Use rtc.pm() to read the AM/PM state of the hour
    if (rtc.pm()){ 
      Serial.print(" PM"); // Returns true if PM
      dataFile.print("PM");
    }
    else{
      Serial.print(" AM");
      dataFile.print(" AM");
    }
  }
  
  Serial.print(" | ");
  dataFile.print(" | ");
  
  // Few options for printing the day, pick one:
  Serial.print(rtc.dayStr()); // Print day string
  dataFile.print(rtc.dayStr()); // Print day string
  
  //Serial.print(rtc.dayC()); // Print day character
  //Serial.print(rtc.day()); // Print day integer (1-7, Sun-Sat)
  Serial.print(" - ");
  Serial.print(String(rtc.month()) + "/" +   // Print month
                 String(rtc.date()) + "/");  // Print date
  Serial.println(String(rtc.year()));        // Print year
  dataFile.print(" - ");
  dataFile.print(String(rtc.month()) + "/" +   // Print month
                 String(rtc.date()) + "/");  // Print date
  dataFile.println(String(rtc.year()));        // Print year
  dataFile.close();
}
