// nrf24_reliable_datagram_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging client
// with the RHReliableDatagram class, using the RH_NRF24 driver to control a NRF24 radio.
// It is designed to work with the other example nrf24_reliable_datagram_server
// Tested on Uno with Sparkfun WRL-00691 NRF24L01 module
// Tested on Teensy with Sparkfun WRL-00691 NRF24L01 module
// Tested on Anarduino Mini (http://www.anarduino.com/mini/) with RFM73 module
// Tested on Arduino Mega with Sparkfun WRL-00691 NRF25L01 module

#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_NRF24 driver;
// RH_NRF24 driver(8, 7);   // For RFM73 on Anarduino Mini

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

//define sensor pins
byte sensorPins[] = {A0, A1, A2};
//define pin the button is plugged into
int buttonPin[] = {4,3};
//Boolean for wether or not the button is pressed
int buttonPressed[] = {0, 0};
//variable for the value that a plant becomes to low at
int tooDryValue[] = {0, 0};
//variable for the current readings of the sensor
int currentMoisture[] = {10, 10};
//boolean to tell us if a variable is to low
boolean isTooDry = false;

void setup() 
{
  Serial.begin(9600);
  if (!manager.init())
    Serial.println("init failed");

   
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
}

uint8_t data[4];
// Dont put this on the stack:
uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];

void loop()
{
  //map the readings we get from the moisture sensor to something we can send
  for(int k=0;k<=sizeof(k);k++){
    currentMoisture[k] = map(analogRead(sensorPins[k+1]), 0, 1028, 0, 255);
  }
  //if the current moistre of the plant is below a threshhold then we are to dry
  for(int l=0;l<=sizeof(currentMoisture);l++){
    if(currentMoisture[l] < tooDryValue[l]){
      isTooDry = true;
    }
    else{
      isTooDry = false;
    }
  }
  data[0] = map(analogRead(sensorPins[0]), 0, 950, 100, 0);
  data[1] = isTooDry;
  data[2] = currentMoisture[0];
  data[3] = tooDryValue[0];
  Serial.println("Sending to nrf24_reliable_datagram_server");
    
  // Send a message to manager_server
  if (manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;   
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
    }
    else
    {
      Serial.println("No reply, is nrf24_reliable_datagram_server running?");
    }
  }
  else
    Serial.println("sendtoWait failed");
  delay(500);
}
