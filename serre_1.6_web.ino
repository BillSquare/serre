/*
 * GREEN HOUSE ACTUATORS CONTROL VIA WEB SERVER
 * Manual or AUTO mode 
 * auto = opening/closing of actuators is controled by 3 sensors : temperature (Adadfruit SHT31-D), rain (2 x sensors MH-RD) & wind (QS-FS01)
 * Manual : buttons "CLOSE", "NEUTRAL or "OPEN".  NEUTRAL stops any movement and keeps the opening/closing in standby
 * Attention : Relay Output LOW = relay ON,  Relay Output HIGH = relay OFF. Relay is triggered when output is LOW !!!
 * Il y a 4 relais inverseurs : 2 pour chacun des 2 circuits de fenêtres = 4 relais.
 * prévoir un relais pour le transfo de puissance MeanWell de 22Amp afin qu'il ne soit pas allumé tout le temps  -->> a faire dans une version ultérieure !-)
 * l'adresse MAC du Arduino wifi REV2 est ->  84:0D:8E:33:A2:BC (lui assigner une adresse IP fixe dans le router !)
 */
//#include <avr/wdt.h>                            // watchdog 

#include <Arduino.h>                            // library générale Arduino (pas spécialement nécessaire ... ?)
#include <Wire.h>                               // library nécessaire pour i2c ?

#include <SPI.h>                                // Serial Peripheral Interfacelibrary needed for web server
#include <WiFiNINA.h>                           // library for web server on UNO Wifi rev2
#include "arduino_secrets.h"                    // login & WAP2 password for wifi (voir onglet "arduino_secrets.h")

char ssid[] = SECRET_SSID;                      // your network SSID (name)                                     ->  enter your sensitive data in the Secret tab/arduino_secrets.h
char pass[] = SECRET_PASS;                      // your network password (use for WPA, or use as key for WEP)   ->  enter your sensitive data in the Secret tab/arduino_secrets.h

int status = WL_IDLE_STATUS;
WiFiServer server(80);                          // définit le port du web server (= 80). Utile pour la redirection de port lors d'accès depuis l'extérieur.

const int Relay1 = 2;                                   //défini le Nr de pin pour la sortie vers les relais inverseur 1. (polarité 1 du circuit de fenêtres 1)
const int Relay2 = 3;                                   //défini le Nr de pin pour la sortie vers les relais inverseur 2. (polarité 2 du circuit de fenêtres 1)
const int Relay3 = 4;                                   //défini le Nr de pin pour la sortie vers les relais inverseur 3. (polarité 1 du circuit de fenêtres 2) 
const int Relay4 = 5;                                   //défini le Nr de pin pour la sortie vers les relais inverseur 4. (polarité 2 du circuit de fenêtres 2)

String StatusSystem;                            // la variable statusSystem affichera le statut du système : OPEN, CLOSE, STATUS QUO
String ModeSystem;                                   //Mode "AUTO" ou "MANUAL"

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}


void Opening(){
    digitalWrite(Relay1,HIGH);
    digitalWrite(Relay2,LOW);
    digitalWrite(Relay3,HIGH);
    digitalWrite(Relay4,LOW);
    Serial.println("windows are opening. Relay 1 OFF,   Relay 2 ON,   Relay 3 OFF,   Relay 4 ON.");
    StatusSystem = "OPEN";
  }


void Closing(){
    digitalWrite(Relay1,LOW);
    digitalWrite(Relay2,HIGH);
    digitalWrite(Relay3,LOW);
    digitalWrite(Relay4,HIGH);
    Serial.println("windows are closing. Relay 1 ON,   Relay 2 OFF,  Relay 3 ON,   Relay 4 OFF.");
    StatusSystem = "CLOSE";
    }


void StatusQuo(){
    digitalWrite(Relay1,HIGH);
    digitalWrite(Relay2,HIGH);
    digitalWrite(Relay3,HIGH);
    digitalWrite(Relay4,HIGH);
    Serial.println("STATUS QUO.  All Relays OFF");
    StatusSystem = "STATUS QUO";
    }

/*void software_Reboot()            // watchdog
  {                               // watchdog
  wdt_enable(WDTO_500MS);         // watchdog
 while(1)                         // watchdog
  {                               // watchdog
  }                               // watchdog
}                                 // watchdog
*/
    
void setup() {
  
  pinMode(Relay1, OUTPUT);                        // si Relay1  est LOW  => le relais 1 bascule
  pinMode(Relay2, OUTPUT);                        // si Relay2  est LOW  => le relais 2 bascule
  pinMode(Relay3, OUTPUT);                        // si Relay3  est LOW  => le relais 3 bascule
  pinMode(Relay4, OUTPUT);                        // si Relay4  est LOW  => le relais 4 bascule
  
  Serial.begin(9600);                           // initialize serial communication
  StatusQuo();                                  // initialise l'état des relais au status quo au démarrage
    
  if (WiFi.status() == WL_NO_MODULE) {          // check for the WiFi module:
    Serial.println("Communication with WiFi module failed!");
                                                // don't continue
    while (true);
  }
  
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

                                                // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                       // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                               // start the web server on port 80
  printWifiStatus();                            // you're connected now, so print out the status

                           }



void loop(){


  Serial.println("----------------------------loop begin");
    Serial.print("Mode : ");
    Serial.println(ModeSystem);
    Serial.print("Status system : ");
    Serial.println(StatusSystem);
    
    Serial.println();
  
    Serial.print("Relay 1 : ");
    Serial.println(digitalRead(Relay1));
    Serial.print("Relay 2 : ");
    Serial.println(digitalRead(Relay2));
    Serial.print("Relay 3 : ");
    Serial.println(digitalRead(Relay3));
    Serial.print("Relay 4 : ");
    Serial.println(digitalRead(Relay4));
    
  WiFiClient client = server.available();         // listen for incoming clients

  if (client) {                                   // if you get a client,
    Serial.println("new client");                 // print a message out the serial port
    String currentLine = "";                      // make a String to hold incoming data from the client
    while (client.connected()) {                  // loop while the client's connected
      if (client.available()) {                   // if there's bytes to read from the client,
        char w = client.read();                   // read a byte, then
        Serial.write(w);                          // print it out the serial monitor
        if (w == '\n') {                          // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
client.print("<HTML><HEAD><TITLE>SERDO</TITLE></HEAD><BODY>");
            // the content of the HTTP response follows the header:
            client.print("<h1>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;La Serre de Donatienne</h1> <br><br>"); 
            client.print("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"/O\">OPEN</a>");
            client.print("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"/N\">STAND-BY</a>");
            client.print("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"/C\">CLOSE</a></h1>");
            client.print("<br><br>");
            client.print("<h2>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Status :");
            client.print("&nbsp;&nbsp;");
            client.print(ModeSystem); 
            client.print("   +   ");           
            client.print(StatusSystem);
                        
            client.print("</h2></BODY>");
            

            // The HTTP response ends with another blank line:
            client.println();
              // break out of the while loop:
              break;
            } else {    // if you got a newline, then clear currentLine:
              currentLine = "";
            }
            } else if (w != '\r') {  // if you got anything else but a carriage return character,
              currentLine += w;      // add it to the end of the currentLine
            }

        // Check to see if the client request was "GET /A"  "GET /C"   ......  :
 
              if (currentLine.endsWith("GET /O")) {Opening();   StatusSystem = "OPEN"; ModeSystem="MANUAL";}                                      // if Button Open activated           ->   Opening & statusSystem = OPEN
              if (currentLine.endsWith("GET /C")) {Closing();   StatusSystem = "CLOSE"; ModeSystem="MANUAL";}                                     // if Button Close activated          ->   Closing & statusSystem = CLOSE
              if (currentLine.endsWith("GET /N")) {StatusQuo();   StatusSystem = "STAND-BY"; ModeSystem="MANUAL";}                                // if Button STAND-BY activated      ->   Status Quo & statusSystem = STAND-BY

                 

      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }   
}
