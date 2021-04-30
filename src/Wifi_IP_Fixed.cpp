/*
# This piece of software is under the GPL licence https://www.gnu.org/licenses/gpl-3.0.fr.html
# Thanks for considering letting this licence as part the source code

DISCLAIMER:
  *** The communication between the client application and the server (ESP32 module) ***
  *** is completely 'open', hence UNSAGE.                                            ***

*/

/*

  WiFi Communication:

  Local Functions:

    *


  Service function:
    - void WifiConnectToHost(void):
      - Establish connection to the router, SSID and Password are hard coded.
    
    -

*/

/* ESP32 headers */
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* Local Application headers */
#include "APP_common.h"       
#include "../../HMI/common.h"    // Shared with Python client application

#define NB_TRY_ROUTER 5       // Retry to connect the router
#define DELAY_ROUTER  1000    // RETRY DELAY ROUTER CONNECTION


const char* ssid1             = "LouisRoussy";    // SSID for train Application
const char* password1         = "monastier";      // Password for train Application


IPAddress myIP(192, 168,1,81);
IPAddress Gateway(192, 168, 1, 1);          // Attention au un seul swith actif 
IPAddress SubNet(255, 255, 255, 1);
IPAddress DNS(255,255,255,0);

WiFiClient  Client2;          // ESP32 WifiClient object

char        E2P_ssid[MAX_SIZE_SSID];
char        E2P_pwd[MAX_SIZE_PWD];
char        E2P_IpAdr[MAX_SIZE_IP_ADR];

int  WifiConnection(int nbTry, int delayTry);

// ############################################################################
// PUBLIC FUNCTION:  Connection to the Wifi NetWork and to the host
//    - 1: connect to the router with X retries
//    - 2: once connected to the router, connect to the host
// ###########################################################################
int WiFiSetup(void) {

int       myPort;

  myPort = PORT_BASE + myIP[3];

  Serial.println("WiFiSetup xxx: DATA from E2PROM: ");
  Serial.print  ("WiFiSetup xxx: ssid:");
  Serial.println(ssid1);
  Serial.print  ("WiFiSetup xxx: PWD :");
  Serial.println(password1);

  if  (WiFi.config(myIP,Gateway,SubNet, DNS, DNS) == false) {
      Serial.println('Failed');
  }
  else
    Serial.println('Success');

  WiFi.begin(ssid1, password1);


  if (WifiConnection(NB_TRY_ROUTER, DELAY_ROUTER)==WL_CONNECTED) {
    Serial.print("WiFiSetup ---: E2PROM: WIFI IP address: ");
    Serial.println(myIP);


    Serial.print("WiFiSetup ---: PORT: ");
    Serial.println(myPort);
  }
  return myPort;
}

// ############################################################################
// LOCAL FUNCTION:  Connection to Wifi NetWork
//    - nbTry: number of retries
//    - delay: delay between between try
// ###########################################################################

int WifiConnection(int nbTry, int delayTry) {
  int i;
  int connected=0;

  for (i=0;i<nbTry;i++){
      connected = WiFi.status();
      if (connected == WL_CONNECTED) {
         Serial.println("CONNECTED TO WIFI");
         myIP = WiFi.localIP();         // ##########################
         Serial.println(myIP);          // ##########################

         break;
      }
      delay(delayTry);
    }
  return connected;
}


// ############################################################################
// LOCAL FUNCTION:  Connection to the host (after connection to Wifi NetWork)
//    - none
//    -
// ###########################################################################
void Wifi_UDP(){

 Serial.println("Wifi UDP setup");


} // Wifi_UDP

// ############################################################################
// PUBLIC FUNCTION:  Acknowledge received data
//    - const char *string: description string
//    - int value: value of the data
// ###########################################################################
void WifiPrint(const char *string, int value) {
char buf[ACK_MSG_LENGTH+1];   // +1, for null terminination

  memset(buf,'-',ACK_MSG_LENGTH);
  sprintf(buf, "%s: %u", string, value);
  Client2.write(buf,ACK_MSG_LENGTH);
  Serial.print("Client2.write");
}

// ############################################################################
// PUBLIC FUNCTION:  Send data to the host
//    - none
//    -
// ###########################################################################
void WifiSendMsgToHost(char *buf){
int len;
  WifiPrint((const char *)buf, 0);
  Serial.print("Sending data to host");
  len = Client2.write(buf, 100);
}

// ############################################################################
// PUBLIC FUNCTION:  Receive data from thee host
//    - none
//    -
// ###########################################################################

int WifiRead(char *buff) {
int len=0;
  while (Client2.available()>0) {
    len=Client2.read((uint8_t *)buff,ORDER_SIZE);
// debug traces, "printl" will affect performances (locking calls ?)
    Serial.println("RCV");
    Serial.print((char *)buff);
    Serial.print(len);
  }
  return len;
}



