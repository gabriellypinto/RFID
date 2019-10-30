// Wireless RFID Door Lock Using NodeMCU
// Created by LUIS SANTOS & RICARDO VEIGA
// 7th of June, 2017


#include <Wire.h>
#include "SSD1306.h"
#include <MFRC522.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define RST_PIN 20 // RST-PIN for RC522 - RFID - SPI - Module GPIO15 
#define SS_PIN  2  // SDA-PIN for RC522 - RFID - SPI - Module GPIO2
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

#define Relay 10

#define BlueLed 15
#define GreenLed 0
#define RedLed 3

//Wireless name and password
const char* ssid     = "********"; // replace with you wireless network name
const char* password = "********"; //replace with you wireless network password

// Remote site information
const char* host = "********"; // IP address of your local server
String url = "********"; // folder location of the txt file with the RFID cards identification, p.e. "/test.txt" if on the root of the server

int time_buffer = 5000; // amount of time in miliseconds that the relay will remain open

void setup() {
  pinMode(Relay, OUTPUT);
  digitalWrite(Relay,0);
  
  Serial.begin(115200);    // Initialize serial communications
  SPI.begin();           // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522

  // We start by connecting to a WiFi network

  Serial.println("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  leds_off();
  delay(3000);
}

void leds_off() {
  analogWrite(BlueLed, 0);   // turn the LED off
  analogWrite(GreenLed, 0);   // turn the LED off
  analogWrite(RedLed, 0);   // turn the LED off
}

void reject() {
  analogWrite(RedLed, 767);   // turn the Red LED on
  delay(2000);
  leds_off(); 
}

void authorize() {
  analogWrite(GreenLed, 767);   // turn the Green LED on
  digitalWrite(Relay,1);
  delay(time_buffer);              // wait for a second 
  digitalWrite(Relay,0);
  leds_off(); 
}

// Helper routine to dump a byte array as hex values to Serial
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void loop() {
  int authorized_flag = 0;
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {   
    delay(50);
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {   
    delay(50);
    return;
  }

////-------------------------------------------------RFID----------------------------------------------


  // Shows the card ID on the serial console
  String content= "";
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();
  Serial.println("Cart read:" + content);

////-------------------------------------------------SERVER----------------------------------------------

  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  delay(10);

  // Read all the lines of the reply from server and print them to Serial
  String line;
  while(client.available()){
     line = client.readStringUntil('\n');
     
    if(line==content){
      authorized_flag=1;
    }
  }
  
  if(authorized_flag==1){
    Serial.println("AUTHORIZED");
    authorize();
  }
  else{
    Serial.println("NOT AUTHORIZED");
    reject();
  }
}
