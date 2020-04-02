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

const int chipSelect = 10;

void setup()
{
  Wire.begin(9);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  
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
void loop()
{
  delay(100);
}
int data[7];
// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  //for(int i = 0; i++; i < 5) // loop through all but the last
    
    data[i] = Wire.read(); // receive byte as a character
    //.print("0");
    //Serial.println(i);
    Serial.println(data[i]);         // print the character
    i++;
    // make a string for assembling the data to log:
    String dataString = "";
  
    // read three sensors and append to the string:
    dataString += String(data[i]);
     // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open("datalog3.txt", FILE_WRITE);
  
    // if the file is available, write to it:
    if (dataFile) {
      if(i == 0)
        dataFile.print("Light: ");
      if(i == 1)
        dataFile.print("isTooDry: ");
      if(i == 2)
        dataFile.print("Moisture 1: ");
      if(i == 3)
        dataFile.print("Too Dry Value 1: ");
      if(i == 4)
        dataFile.print("Moisture 2: ");
      if(i == 5)
        dataFile.print("Too Dry Value 2: ");
      if(i == 6)
        dataFile.print("------------");
      
      dataFile.println(dataString);
      dataFile.close();
      // print to the serial port too:
      Serial.println("Saving");
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
    }
    if(i > 6){ 
      i = 0;
    }
}
