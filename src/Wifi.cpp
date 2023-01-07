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
#include "../../TrainControl/common.h"  // Shared with Python client application

#define NB_TRY_ROUTER 5       // Retry to connect the router
#define DELAY_ROUTER  5000    // RETRY DELAY ROUTER CONNECTION

const char *ssid1             = "LouisRoussy";    // SSID for train Application
const char *password1         = "monastier";      // Password for train Application

WiFiClient  Client2;          // ESP32 WifiClient object

char        E2P_ssid[MAX_SIZE_SSID];
char        E2P_pwd[MAX_SIZE_PWD];
char        E2P_IpAdr[MAX_SIZE_IP_ADR];

// ############################################################################
// LOCAL FUNCTION:  Connection to Wifi NetWork
//    - nbTry: number of retries
//    - delay: delay between between try
// ###########################################################################

int WifiConnection(int nbTry, int delayTry) {
  int i;
  int connected=0;
  
  WiFi.begin(ssid1,password1);

  for (i=0;i<nbTry;i++){
      connected = WiFi.status();
      if (connected == WL_CONNECTED) {
         Serial.println("CONNECTED TO WIFI");
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
// PUBLIC FUNCTION:  Connection to the Wifi NetWork and to the host
//    - 1: connect to the router with X retries
//    - 2: once connected to the router, connect to the host
// ###########################################################################
int WiFiSetup() {
IPAddress myIP;
int       myPort;
char      myIPadress; 


// Retrieve data store in the E2PROM_mgt
EEPROM_READ_STR(E2P_ADR_SSID,   MAX_SIZE_SSID,  E2P_ssid);
EEPROM_READ_STR(E2P_ADR_PWD,    MAX_SIZE_PWD,   E2P_pwd);
EEPROM_READ_STR(E2P_ADR_IP_ADR, MAX_SIZE_IP_ADR,E2P_IpAdr);

Serial.println("DATA from E2PROM: ");
Serial.print  ("ssid:");
Serial.println(E2P_ssid);
Serial.print  ("PWD :");
Serial.println(E2P_pwd);
Serial.print  ("IP  :");
Serial.println(E2P_IpAdr);
Serial.println("----------------------------------");

if (TCP_IP_check_string(E2P_ssid  , MAX_SIZE_SSID)) {
    Serial.println(E2P_ssid);

//    if (TCP_IP_check_string(E2P_pwd   , MAX_SIZE_PWD )) {
//        Serial.println(E2P_pwd);
//      if (TCP_IP_check_string(E2P_IpAdr , MAX_SIZE_IP_ADR )) {

    EEPROM_READ_STR(E2P_ADR_IP_ADR, MAX_SIZE_IP_ADR,E2P_IpAdr);
    Serial.print(" ### ");
    Serial.print(E2P_IpAdr);
    Serial.println(" ### ");
    
    int numberOfNetworks = WiFi.scanNetworks();
  
    Serial.print("Number of networks found: ");
    Serial.println(numberOfNetworks);
    for (int i = 0; i < numberOfNetworks; i++) {
    
        Serial.print("Network name: ");
        Serial.println(WiFi.SSID(i));
    
        Serial.print("Signal strength: ");
        Serial.println(WiFi.RSSI(i));
    
        Serial.print("MAC address: ");
        Serial.println(WiFi.BSSIDstr(i));
    
        Serial.print("Encryption type: ");
//        String encryptionTypeDescription = translateEncryptionType(WiFi.encryptionType(i));
//        Serial.println(encryptionTypeDescription);
    
        Serial.println("-----------------------");
    
      }
    Serial.println(" -----------  Essai Connection ");
     
    if (WifiConnection(NB_TRY_ROUTER, DELAY_ROUTER)==WL_CONNECTED) {
      myIP = WiFi.localIP();
      Serial.print("LOCAL IP:");
      Serial.println(WiFi.localIP());
      
      // The TCP/IP port defined by this formula: fourth byte of the TCP/IP + 43150 ()
      myPort = myIP[3]+PORT_BASE;    
      Serial.print("PORT: ");
      Serial.println(myPort);

//      myIP = myIP[0] + myIP[1] + myIP[2] + myIP[3]
//      Serial.print(" remote Port:");    # Même valeur.
//      Serial.println(Client2.remotePort());

      Serial.println("### E2PROM DATA ROUTER connection SUCCESS" );
//      E2P_ADR, int maxChar, char *StringBuf) {
//      EEPROM_WRITE_STR( E2P_ADR_IP_ADR, sizeof(myIP),);
      return(myPort);
    } else {
        Serial.println("### E2PROM DATA ROUTER connection FAILED###");
    }    
}
  return 0;
}


// ############################################################################
// PUBLIC FUNCTION:  Accept any entry connection
//    - none
//    -
// ###########################################################################
int WifiClientAvailable(){
 return (Client2.available());
}

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
}

// ############################################################################
// PUBLIC FUNCTION:  Send data to the host
//    - none
//    -
// ###########################################################################
void WifiSendMsgToHost(char *buf){
//int len;
  WifiPrint((const char *)buf, 0);
// Serial.print("Sending data to host");
// len = Client2.write(buf,MSG_LENGTH);
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
//    Serial.println("RCV");
//    Serial.print((char *)buff);
//    Serial.print(len);
  }
  return len;
}

