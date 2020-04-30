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
byte potPin = A15;
int potValue = 0;
//variables for led pins
int blueLedPin = 30;
int greenLedPin = 31;
int redLedPin = 32;
//variable for the number of moisture sensors
int numMoistSensors = 2;
//define sensor pins
byte sensorPins[] = {A0, A1, A2};
//define pin the button is plugged into
int buttonPin[] = {28,29};
//Boolean for wether or not the button is pressed
int buttonPressed[] = {0, 0};
//variable for the value that a plant becomes to low at
int tooDryValue[] = {0, 0};
//variable for the current readings of the sensor
int currentMoisture[] = {10, 10};
//boolean to tell us if a variable is to low
boolean isTooDry = false;
//int for the value of the light sensor
int lightValue = 0;
//int for the pwm pin of the pumps
int pwmAPin = 2;
//variables for determining maximum wind reading
uint8_t prevWind[] = {0};
uint8_t maxWind[] = {0};
// Singleton instance of the radio driver
RH_NRF24 driver;
// RH_NRF24 driver(8, 7);   // For RFM73 on Anarduino Mini

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, SERVER_ADDRESS);

//setup LCD pins
LiquidCrystal lcd(22,23,24,25,26,27);

LiquidCrystal lcdWeather(34,35,36,37,38,39);
void setup() 
{
  Serial.begin(9600);
  // Setup wire transmition as slave with adress 9
  Wire.begin(9);
  Wire.onRequest(requestEvent); // register event
  //begin LCD screen
  lcd.begin(16, 2);
  //clear LCD display
  lcd.clear();
  //begin 2nd LCD screen
  lcdWeather.begin(16, 2);
  //clear 2nd LCD display
  lcdWeather.clear();
  
  //led pins initillization
  pinMode(blueLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  //pwm pin
  pinMode(pwmAPin, OUTPUT);
  //potentiometer pin
  pinMode(potPin, INPUT);
  if (!manager.init()){
    Serial.println("init failed");
    lcd.setCursor(0,0);
    lcd.print("init failed");
  }
  else
  {
    lcdWeather.setCursor(0,0);
    lcdWeather.print("init success");
  }
  
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
}

uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
int x = 0;
void loop()
{
  potValue = analogRead(potPin);
  lightValue = map(analogRead(sensorPins[0]), 0, 1028, 100, 0);
  if(lightValue > 100)
    lightValue=0;
  //see if a button has been pressed
  buttonPressed[0] = digitalRead(buttonPin[0]);
  buttonPressed[1] = digitalRead(buttonPin[1]);
  //map the readings we get from the moisture sensor to something we can send
  for(int k=0;k<=numMoistSensors;k++){
    currentMoisture[k] = map(analogRead(sensorPins[k+1]), 0, 1028, 0, 100);
  }
  //if the button is pressed set a new to dry value
  //for(int m=0;m<=numMoistSensors; m++){
  if(digitalRead(buttonPin[0]) == HIGH){
    tooDryValue[0] = currentMoisture[0];
  }
  if(buttonPressed[1] == HIGH){
    tooDryValue[1] = currentMoisture[1];
  }
  //}
  //if the current moistre of the plant is below a threshhold then we are to dry
  //for(int l=0;l<=numMoistSensors;l++){
  if(currentMoisture[0] < tooDryValue[0] || currentMoisture[1] < tooDryValue[1]){
    isTooDry = true;
  }
  else{
    isTooDry = false;
  }
  if(isTooDry){
    digitalWrite(pwmAPin, 255);
    Serial.println("Watering");
  }
  else
    digitalWrite(pwmAPin, 0);
  Serial.print("Light: ");
  Serial.println(lightValue);
  Serial.print("isTooDry: ");
  Serial.println(isTooDry);
  Serial.print("Moisture: ");
  Serial.println(currentMoisture[0]);
  Serial.print("To Dry Value: ");
  Serial.println(tooDryValue[0]);
  Serial.print("Moisture2: ");
  Serial.println(currentMoisture[1]);
  Serial.print("To Dry Value2: ");
  Serial.println(currentMoisture[1]);
  Serial.print("Button 1: ");
  Serial.println(digitalRead(buttonPin[0]));
  Serial.print("Button 2: ");
  Serial.println(buttonPressed[1]);
 
  //print values to LCD display
  if(potValue < 360){
    lcd.setCursor(0,0);
    lcd.print("M1 Sun: H20: TDV:");
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(3,1);
    lcd.print(lightValue);
    lcd.setCursor(8,1);
    lcd.print(currentMoisture[0]);
    lcd.setCursor(13,1);
    lcd.print(tooDryValue[0]);
  }
  if(potValue > 360 && potValue < 720){
    lcd.setCursor(0,0);
    lcd.print("M2 Sun: H20: TDV:");
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(3,1);
    lcd.print(lightValue);
    lcd.setCursor(8,1);
    lcd.print(currentMoisture[1]);
    lcd.setCursor(13,1);
    lcd.print(tooDryValue[1]);
  }
  if(isTooDry)
    digitalWrite(redLedPin, HIGH);
  else
    digitalWrite(redLedPin, LOW);
   //Serial.println("Sending");
  Serial.print("prevWind: ");
  Serial.println(prevWind[0]);
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    
    if (manager.recvfromAck(buf, &len, &from))
    {
      x++;
      lcdWeather.setCursor(0,0);    
      lcdWeather.print("WR:  Cur:  Max:");
      lcdWeather.setCursor(0,1);
      lcdWeather.print("         ");
      lcdWeather.setCursor(5,1);
      lcdWeather.print(buf[0]);
      digitalWrite(greenLedPin, HIGH);
      Serial.print("prevWind: ");
      Serial.println(prevWind[0]);
      if(prevWind[0] < buf[0]){
        Serial.println("MAX WIND--------------");
        maxWind[0] = buf[0];
        lcdWeather.setCursor(11, 1);
        lcdWeather.print(maxWind[0]);
      }
      
      Serial.print("Wind Readings: ");
      Serial.println(buf[0]);
      prevWind[0] = buf[0];
      Serial.print("prevWind: ");
      Serial.println(prevWind[0]);
      // Send a reply back to the originator client
      if (!manager.sendtoWait(data, sizeof(data), from))
        Serial.println("sendtoWait failed");
    }
  }
  digitalWrite(greenLedPin, LOW);
  wireData[0] = lightValue;
}
int i = 0;
void requestEvent()
{  
  Serial.println("--------------------------------------------");
  wireData[0] = lightValue;
  wireData[1] = isTooDry;
  wireData[2] = currentMoisture[0]; 
  wireData[3] = tooDryValue[0];
  wireData[4] = currentMoisture[1];
  wireData[5] = tooDryValue[1];
  wireData[6] = buf[0];
  wireData[7] = 99;
  if(wireData[0]==99)
    wireData[0]=98;
  if(wireData[1]==99)
    wireData[1]=98;
  if(wireData[2]==99)
    wireData[2]=98;
  if(wireData[3]==99)
    wireData[3]=98;
  if(wireData[4]==99)
    wireData[4]=98;
  if(wireData[5]==99)
    wireData[5]=98;
  if(wireData[6]==99)
    wireData[6]=98;
   
  Serial.print("sending wire ");
  Serial.println(i);
  Wire.write(wireData[i]);
  delay(50);
  i++;
  if(i>7){
    i = 0;
  }
} 
