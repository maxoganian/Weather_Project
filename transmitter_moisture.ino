// Include RadioHead ReliableDatagram & NRF24 Libraries
#include <RHReliableDatagram.h>
#include <RH_NRF24.h>

// Include dependant SPI Library 
#include <SPI.h>

// Define addresses for radio channels
#define CLIENT_ADDRESS 1   
#define SERVER_ADDRESS 2

// Create an instance of the radio driver
RH_NRF24 RadioDriver;

// Sets the radio driver to NRF24 and the client address to 1
RHReliableDatagram RadioManager(RadioDriver, CLIENT_ADDRESS);

// Declare unsigned 8-bit data array
uint8_t data[4]; 

// Define the Message Buffer
uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
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
  // Setup Serial Monitor
  Serial.begin(9600);
  //intitialize button pin
  for(int i=0;i++;i<=sizeof(buttonPin)){
    pinMode(buttonPin[i], INPUT);
  }
  // Initialize RadioManager with defaults - 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!RadioManager.init())
    Serial.println("init failed");
}

void loop()
{
  //see if a button has been pressed
  for(int j=0;j<=sizeof(buttonPressed);j++){
    buttonPressed[j] = digitalRead(buttonPin[j]);
  }
  //map the readings we get from the moisture sensor to something we can send
  for(int k=0;k<=sizeof(k);k++){
    currentMoisture[k] = map(analogRead(sensorPins[k+1]), 0, 1028, 0, 255);
  }
  //if the button is pressed set a new to dry value
  if(buttonPressed[0] == HIGH){
    tooDryValue[0] = currentMoisture[0];
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
  //Display the moisture values in the serial monitor.
  data[0] = map(analogRead(sensorPins[0]), 0, 950, 100, 0);
  data[1] = isTooDry;
  data[2] = currentMoisture[0];
  data[3] = tooDryValue[0];
  Serial.println("-----------");
  Serial.print("moisture:");
  Serial.println(data[2]);
  Serial.println(currentMoisture[0]);
  Serial.print("light:");
  Serial.println(analogRead(sensorPins[0]));
  Serial.print("tooDryValue: ");
  Serial.println(tooDryValue[0]);
  Serial.print("isTooDry: ");
  Serial.println(isTooDry);
  Serial.print("buttonState: ");
  Serial.println(buttonPressed[0]);
  //Serial.println("Sending Joystick data to nrf24_reliable_datagram_server");
  RadioManager.sendtoWait(data, sizeof(data), SERVER_ADDRESS);

}
