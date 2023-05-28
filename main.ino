/* used Pins =
MATRIX =  D2, D3, D4, D5, D6, D7, D8, D9.
74HC595 = A3, A4, A5.
MASTERSLAVE = D10, D11, D12, D13.

Free Pins
O/P and I/P = A0, A1, A2
I/P = A6,A7

*/





#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#define SerialMon Serial
#define APPLEMIDI_DEBUG SerialMon
#include <AppleMIDI.h>
#include "piano_Keypad.h"
#include "ShiftRegister.h"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

char ssid[] = "WiFi"; //  your network SSID (name)
char pass[] = "odedara21";    // your network password (use for WPA, or use as key for WEP)

AsyncWebServer server(80);
///////////////////////////////////////////////////////


const byte ROWS = 8; // rows
const byte COLS = 8; // columns
//define the symbols on the buttons of the keypads
char keys[ROWS][COLS] = {
  {24,25,26,27,28,29,30,31},
  {32,33,34,35,36,37,38,39},
  {40,41,42,43,44,45,46,47},
  {48,49,50,51,52,53,54,55},
  {56,57,58,59,60,61,62,63},
  {64,65,66,67,68,69,70,71},
  {72,73,74,75,76,77,78,79},
  {80,81,82,83,84,85,86,87},
};

int piano[85];

byte rowPins[ROWS] = {7, 6, 5, 4, 3, 2, 1, 0}; //74HC595 Pins Only Output
byte colPins[COLS] = {0, 1, 2, 3, 4, 5, 6, 7}; //Digital pins Input Pins

int8_t isConnected = 0;

//initialize an instance of class NewKeypad
Piano_Keypad customKeypad = Piano_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
APPLEMIDI_CREATE_DEFAULTSESSION_INSTANCE();

void setup() {
  
  Serial.begin(115200);
  DBG_SETUP(115200);
  DBG("Booting");
  WiFi.begin(ssid, pass);
  customKeypad.begin();  
  arduinoOTA();  
  WebSerialSetup();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);

    DBG("Establishing connection to WiFi..");
    /*if (WiFi.status() == WL_CONNECTED){ 
        delay(500); 
        break;
      }*/
  }

  for(int i = 0; i < 85; i++){
       piano[i] = 0;
    }
  
  DBG("Connected to network");

  DBG(F("OK, now make sure you an rtpMIDI session that is Enabled"));
  DBG(F("Add device named Arduino with Host"), WiFi.localIP(), "Port", AppleMIDI.getPort(), "(Name", AppleMIDI.getName(), ")");
  DBG(F("Select and then press the Connect button"));
  DBG(F("Then open a MIDI listener and monitor incoming notes"));
  DBG(F("Listen to incoming MIDI commands"));

  MIDI.begin();

  AppleMIDI.setHandleConnected([](const APPLEMIDI_NAMESPACE::ssrc_t & ssrc, const char* name) {
    isConnected++;
    DBG(F("Connected to session"), ssrc, name);
  });
  AppleMIDI.setHandleDisconnected([](const APPLEMIDI_NAMESPACE::ssrc_t & ssrc) {
    isConnected--;
    DBG(F("Disconnected"), ssrc);
  });
 MIDI.setHandleNoteOn([](byte channel, byte note, byte velocity) {
    DBG(F("NoteOn"), note); 
     WebSerial.println("");
     WebSerial.print("NoteOn : ");
     WebSerial.print(note);
     WebSerial.println(""); 
  }); 
 MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {
     WebSerial.println("");
     WebSerial.print("NoteOff : ");
     WebSerial.print(note);
     WebSerial.println("");
    DBG(F("NoteOff"), note);
  });
  
}

void loop() {
 customKeypad.tick();  
 ArduinoOTA.handle();
 MIDI.read();
 if (isConnected > 0){   
  // If you change key than change this value
  while(customKeypad.available()){
    keypadEvent e = customKeypad.read();
    piano[e.bit.KEY] = e.bit.EVENT;
    
    for(int i = 0 ; i< 85 ;i++){
      if (piano[i] == 1){
         MIDI.sendNoteOn(e.bit.KEY , 80 , 1); // Note Number , Velocity , Channel  
        }
         else if(i == 84){
            break;
         }
      }
        
        if(piano[e.bit.KEY] ==  0){ // If you change key than change this value    
                    MIDI.sendNoteOff(e.bit.KEY, 80, 1);
               }  
         }
     }
  }


void arduinoOTA() {
  
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();     

}


/* Message callback of WebSerial */
void recvMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  WebSerial.println(d);
}

void WebSerialSetup(){
    WebSerial.begin(&server);
    WebSerial.msgCallback(recvMsg);
    server.begin();
  }
