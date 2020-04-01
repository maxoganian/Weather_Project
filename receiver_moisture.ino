// Include RadioHead ReliableDatagram & NRF24 Libraries
#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
// Include dependant SPI Library 
#include <SPI.h>
//include LCD display
#include <LiquidCrystal.h>
// include the wire library
#include <Wire.h>

// Define addresses for radio channels
#define CLIENT_ADDRESS 1   
#define SERVER_ADDRESS 2

// Create an instance of the radio driver
RH_NRF24 RadioDriver;
//RF24 radio(7, 8); // CE, CSN
// Sets the radio driver to NRF24 and the server address to 2
RHReliableDatagram RadioManager(RadioDriver, SERVER_ADDRESS);

// Define a message to return if values received
uint8_t ReturnMessage[] = "JoyStick Data Received"; 

// Define the Message Buffer
uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
//setup LCD pins
LiquidCrystal lcd(9,7,5,4,3,2);
//LED pin
int ledPin = 6;
void setup()
{
  // Setup wire transmition as master
  Wire.begin();
  //initiate LED pin
  pinMode(ledPin, OUTPUT);
  // Setup Serial Monitor
  Serial.begin(9600);
  //begin LCD screen
  lcd.begin(16, 2);
  //clear LCD display
  lcd.clear();
  if (!RadioManager.init()){
    Serial.println("init failed");
    lcd.setCursor(0,0);
    lcd.print("init failed");
  }
} 

void loop()
{
  if (RadioManager.available())
  {
 // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (RadioManager.recvfromAck(buf, &len, &from))

    {
      //map(buf[1], 0, 255, 0, 100);
      //map(buf[2], 0, 255, 0, 100);
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
      if(buf[1]){
        digitalWrite(ledPin, HIGH);
      }
      else{
        digitalWrite(ledPin, LOW);
      }
      //Send the array to the other arduino  
      for(int i=0;i<=3;i++){  
        Serial.print("sending wire ");
        Serial.println(i);
        Wire.beginTransmission(9);
        //Wire.write(00);
        Wire.write(buf[i]);
  //      Wire.write(01);
  //      Wire.write(buf[2]);
  //      Wire.write(02);
  //      //Wire.write(buf[3]);
        delay(100);
        Wire.endTransmission();
      } 

    }
  }              
}
