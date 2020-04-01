// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void setup()
{
  Wire.begin(9);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
}
int i = 0;
void loop()
{
  delay(100);
}

// we are grouping our data by 4 ints
int data[4];

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  //for(int i = 0; i++; i < 5) // loop through all but the last
    
    // We are getting one of the four data
    // data bytes we want
    data[i] = Wire.read(); // receive byte as a character
    Serial.println(data[i]);         // print the character
    i++;
  
    // check if that was the last of our data group;
    // if so, reset the index to the beginning
    if(i > 3){ 
      Serial.println("----------");
      i = 0;
    }
}

