/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Social networks:            http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example shows how to use Arduino Ethernet shield (W5100)
  to connect your project to Blynk.

  NOTE: Pins 10, 11, 12 and 13 are reserved for Ethernet module.
        DON'T use them in your sketch directly!

  WARNING: If you have an SD card, you may need to disable it
        by setting pin 4 to HIGH. Read more here:
        https://www.arduino.cc/en/Main/ArduinoEthernetShield

  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <SPI.h>
#include <Ethernet2.h>
#include <BlynkSimpleEthernet2.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "Write your token here. You get your token from Blynk";

            // Pin Nr for the relays 1, 2, 3 & 4. 
const int Relay1 = 2;                                   // Pin Nr for relay/inverter 1. (polarity 1 of windows circuit 1)
const int Relay2 = 3;                                   // Pin Nr for relay/inverter 2. (polarity 2 of windows circuit 1)  
const int Relay3 = 4;                                   // Pin Nr for relay/inverter 3. (polarity 1 of windows circuit 2) 
const int Relay4 = 5;                                   // Pin Nr for relay/inverter 4. (polarity 2 of windows circuit 2)
                                  
            // déclaration des variable de statut et de mode du système
            //char ModeSystem   = 'A';                                // Declaration of Mode System : A (AUTO) ou M (MANUAL). Default Mode = "A"
char StatusSystem = 'S';                                // Declaration of Status System : 0 (OPEN), C (CLOSE), S (STATUS QUO). Default Status = "S"

            // Virtual pins for BLYNK
int pinValueV0=2;
int PreviousPinValueV0=2;
int pinValueV1=2;
int PreviousPinValueV1=2;
int pinValueV2=2;
int PreviousPinValueV2=2;
int pinValueV3=2;
int PreviousPinValueV3=2;

void Closing(){
    digitalWrite(Relay1,LOW);
    digitalWrite(Relay2,HIGH);
    digitalWrite(Relay3,LOW);
    digitalWrite(Relay4,HIGH);
    Serial.println("windows are closing.");
    StatusSystem = 'C';
    }

void Opening(){
    digitalWrite(Relay1,HIGH);
    digitalWrite(Relay2,LOW);
    digitalWrite(Relay3,HIGH);
    digitalWrite(Relay4,LOW);
    Serial.println("windows are opening.");
    StatusSystem = 'O';
  }

void StatusQuo(){
    digitalWrite(Relay1,HIGH);
    digitalWrite(Relay2,HIGH);
    digitalWrite(Relay3,HIGH);
    digitalWrite(Relay4,HIGH);
    Serial.println("STATUS QUO");
    StatusSystem = 'S';
    }
             
            /* CLOSE & MANUAL  when V0 button pressed in app  */  
BLYNK_WRITE(V0)  
  {
     pinValueV0 = param.asInt();                      
        if (pinValueV0 == 1) {
            StatusSystem = 'C' ;              //Closing();
        Blynk.virtualWrite(V3,LOW);
        Blynk.virtualWrite(V1,LOW);
        Blynk.virtualWrite(V2,LOW);        
        } 
  }

            /* STAND-BY & MANUAL  when V1 button pressed in app  */ 
BLYNK_WRITE(V1)  
  {
     pinValueV1 = param.asInt();                      
        if (pinValueV1 == 1) {
            StatusSystem = 'S';             //StatusQuo();
        Blynk.virtualWrite(V3,LOW);
        Blynk.virtualWrite(V0,LOW);
        Blynk.virtualWrite(V2,LOW);        
        }
  }

            /* OPEN & MANUAL  when V2 button pressed in app  */ 
BLYNK_WRITE(V2)  
  {
    pinValueV2 = param.asInt();                      
        if (pinValueV2 == 1) {
            StatusSystem = 'O';             //Opening();
        Blynk.virtualWrite(V3,LOW);
        Blynk.virtualWrite(V0,LOW);
        Blynk.virtualWrite(V1,LOW);        
        }
  }

void colorButton (){
//  if (ModeSystem == 'M') {Blynk.setProperty(V3, "color", "#D3435C");}   // V3 red
//  if (ModeSystem == 'A') {Blynk.setProperty(V3, "color", "#23C48E");}   // V3 green
  if (StatusSystem == 'O') {Blynk.setProperty(V2, "color", "#23C48E");Blynk.setProperty(V0, "color", "#D3435C");Blynk.setProperty(V1, "color", "#D3435C");}   // V2 green, VO red, V1 red
  if (StatusSystem == 'C') {Blynk.setProperty(V0, "color", "#23C48E");Blynk.setProperty(V1, "color", "#D3435C");Blynk.setProperty(V2, "color", "#D3435C");}   // V0 green, V2 red, V1 red
  if (StatusSystem == 'S') {Blynk.setProperty(V1, "color", "#23C48E");Blynk.setProperty(V0, "color", "#D3435C");Blynk.setProperty(V2, "color", "#D3435C");}   // V1 green, VO red, V2 red  
}

void logicModeStatus () {
    Serial.println("logicModeStatus");            //test to see if/when the function logicModeStatus is running
  if (StatusSystem == 'O')                    {Opening(); }      // if Button Open activated           ->   Opening & statusSystem = OPEN
  if (StatusSystem == 'C')                    {Closing(); }      // if Button Close activated          ->   Closing & statusSystem = CLOSE
  if (StatusSystem == 'S')                    {StatusQuo();}     // if Button STAND-BY activated      ->   Status Quo & statusSystem = STAND-BY
  colorButton ();
  }

BlynkTimer timer;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V0, millis() / 1000);
  Blynk.virtualWrite(V1, millis() / 1000);
  Blynk.virtualWrite(V2, millis() / 1000);
  Blynk.virtualWrite(V3, millis() / 1000);
 // Blynk.virtualWrite(V4, millis() / 2000);
  Blynk.virtualWrite(V5, millis() / 1000);
  Blynk.virtualWrite(V6, millis() / 3000);
  Blynk.virtualWrite(V7, millis() / 3000);
  Blynk.virtualWrite(V8, millis() / 1000);
}


void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth); 
  
  pinMode(Relay1,OUTPUT);
  pinMode(Relay2,OUTPUT);
  pinMode(Relay3,OUTPUT);
  pinMode(Relay4,OUTPUT);

  digitalWrite(Relay1,LOW);
  digitalWrite(Relay2,LOW);
  digitalWrite(Relay3,LOW);
  digitalWrite(Relay4,LOW); 

  timer.setInterval(1500L, logicModeStatus);
}

            /* !!! don't put any delay() in the void loop of the code when using BLYNK !!!!  */
void loop()
{
  Blynk.run();
  timer.run();                                      // Initiates BlynkTimer
}
