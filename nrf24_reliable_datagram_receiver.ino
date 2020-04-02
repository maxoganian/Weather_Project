// nrf24_reliable_datagram_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging server
// with the RHReliableDatagram class, using the RH_NRF24 driver to control a NRF24 radio.
// It is designed to work with the other example nrf24_reliable_datagram_client
// Tested on Uno with Sparkfun WRL-00691 NRF24L01 module
// Tested on Teensy with Sparkfun WRL-00691 NRF24L01 module
// Tested on Anarduino Mini (http://www.anarduino.com/mini/) with RFM73 module
// Tested on Arduino Mega with Sparkfun WRL-00691 NRF25L01 module

#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#include <Wire.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

uint8_t wireData[7]; 
int ledPin = 6;
// Singleton instance of the radio driver
RH_NRF24 driver;
// RH_NRF24 driver(8, 7);   // For RFM73 on Anarduino Mini

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, SERVER_ADDRESS);

//setup LCD pins
LiquidCrystal lcd(9,7,5,4,3,2);

void setup() 
{
  Serial.begin(9600);
  // Setup wire transmition as master
  Wire.begin();
  //begin LCD screen
  lcd.begin(16, 2);
  //clear LCD display
  lcd.clear();
  //led pin
  pinMode(ledPin, OUTPUT);
  if (!manager.init()){
    Serial.println("init failed");
    lcd.setCursor(0,0);
    lcd.print("init failed");
  }
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
}

uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];

void loop()
{
  //Serial.println("Sending");
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      Serial.print("got request from : 0x");
      Serial.println(from, HEX);
      Serial.print("Light: ");
      Serial.println(buf[0]);
      Serial.print("isTooDry: ");
      Serial.println(buf[1]);
      Serial.print("Moisture: ");
      Serial.println(buf[2]);
      Serial.print("To Dry Value: ");
      Serial.println(buf[3]);
      Serial.print("Moisture2: ");
      Serial.println(buf[4]);
      Serial.print("To Dry Value2: ");
      Serial.println(buf[5]);
      //print values to LCD display
      lcd.setCursor(0,0);
      lcd.print("Sun: H20: TDV:");
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print(buf[0]);
      lcd.setCursor(5,1);
      lcd.print(buf[2]);
      lcd.setCursor(10,1);
      lcd.print(buf[3]);
      if(buf[1])
        digitalWrite(ledPin, HIGH);
      else
        digitalWrite(ledPin, LOW);
      wireData[0] = buf[0];
      wireData[1] = buf[1];
      wireData[2] = buf[2]; 
      wireData[3] = buf[3];
      wireData[4] = buf[4];
      wireData[5] = buf[5];
      wireData[6] = 100;
      for(int i=0;i<=6;i++){
        Serial.print("sending wire ");
        Serial.println(i);
        Wire.beginTransmission(9);
        Wire.write(wireData[i]);
        delay(100);
        Wire.endTransmission();
      } 
      // Send a reply back to the originator client
      if (!manager.sendtoWait(data, sizeof(data), from))
        Serial.println("sendtoWait failed");
    }
  }
}
