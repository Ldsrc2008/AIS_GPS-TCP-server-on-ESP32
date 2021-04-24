/*
  20210329
  Start the esp32 as AP and transmit data with TCP
  this one works :
  Only start to detect gps uart data when tcp is connected
  Only one connection at time, the 2nd connection won't get a shit.

  then try to add the 2nd uart of AIS, but it seems to be kind of delay...
  2021/4/4
  after testing task on esp32, seems tcp task could only run on core1
  and only one tcp task is stable.
  So this is a test on running task to reas gps/ais and print through serial
  the tcp is run in loop, print with gps/ais sentences are ready

  if th etask runs on cup1, it miss message, since these 2 task not running on matching speed.
  if the task wuns on cpu0, it cause watch dog reset

  to improve the missing gps sentence, identify the sentence type
    seperate the different sentence, it work better, every second,
    4 messages are reveiced through tcp
*/

#include "WiFi.h"
TaskHandle_t Task1;
const char *ssid = "ESP32AP";
const char *password = "yourPW";
WiFiServer wifiServer(1111);

int new_gps_message = 0;
int new_ais_message = 0;
int new_GPRMC = 0;
int new_GPVTG = 0;
int new_GPGGA = 0;
int new_GPGSA = 0;



//=======define UART for GPS========
HardwareSerial GPSUART(1);
#define GPSRX_pin  0
#define GPSTX_pin  4
static const uint32_t GPSBaud = 9600;
//=======define UART for AIS========
HardwareSerial AISUART(2);
#define AISRX_pin  2
#define AISTX_pin  15
static const uint32_t AISBaud = 38400;

String gps_sentence = "";
String ais_sentence = "";
String GPRMC_sentence = "";
String GPVTG_sentence = "";
String GPGGA_sentence = "";
String GPGSA_sentence = "";

void codeForTask1( void * parameter )
{
  for (;;) {
    while (GPSUART.available()) {
      char in = GPSUART.read();
      if (in == '$') {
        //  Serial.println("start of sentence");
        gps_sentence = "";
        new_gps_message = 0;
        gps_sentence += in;
      } else if (in == '\n')
      {
        gps_sentence += in;
        Serial.print(gps_sentence);
        new_gps_message = 1;
        if (gps_sentence.substring(1, 6) == "GPRMC") {
          GPRMC_sentence = gps_sentence;
          new_GPRMC = 1;
        } else if (gps_sentence.substring(1, 6) == "GPVTG") {
          GPVTG_sentence = gps_sentence;
          new_GPVTG = 1;
        } else if (gps_sentence.substring(1, 6) == "GPGGA") {
          GPGGA_sentence = gps_sentence;
          new_GPGGA = 1;
        } else if (gps_sentence.substring(1, 6) == "GPGSA") {
          GPGSA_sentence = gps_sentence;
          new_GPGSA = 1;
        }


      } else {
        gps_sentence += in;
      }
    }// end of while gpsuart
    //---------AIS-----------------------------
    while (AISUART.available()) {
      char in = AISUART.read();
      if (in == '!') {
        //  Serial.println("start of sentence");
        ais_sentence = "";
        ais_sentence += in;
      } else if (in == '\n')
      {
        ais_sentence += in;
        Serial.print(ais_sentence);
        new_ais_message = 1;
        //client.print(ais_sentence);
      } else {
        ais_sentence += in;
      }
    }// end of while aisuart




  }// end of for(;;)
}

void setup() {

  Serial.begin(115200);
  GPSUART.begin(GPSBaud, SERIAL_8N1, GPSRX_pin, GPSTX_pin);// connecting GPS tx on ESP's GPIO0
  AISUART.begin(AISBaud, SERIAL_8N1, AISRX_pin, AISTX_pin);// connecting AIS tx on ESP's GPIO2

  WiFi.softAP(ssid, password);

  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  wifiServer.begin();
  Serial.println("wifi server start");

  xTaskCreatePinnedToCore(
    codeForTask1,
    "gpsaisTask",
    1000,
    NULL,
    1,
    &Task1,
    1);// place on core 0, cause watch dog reset
  // place on core 1, sometimes miss message
  delay(500);  // needed to start-up task1
}

void loop() {
  WiFiClient client = wifiServer.available();
  if (client) {
    Serial.println("client connected");
    while (client.connected()) {

      if (new_GPRMC == 1) {
        client.print(GPRMC_sentence);
        new_GPRMC = 0;
      } else if (new_GPVTG == 1) {
        client.print(GPVTG_sentence);
        new_GPVTG = 0;
      } else if (new_GPGGA == 1) {
        client.print(GPGGA_sentence);
        new_GPGGA = 0;
      } else if (new_GPGSA == 1) {
        client.print(GPGSA_sentence);
        new_GPGSA = 0;
      }



      // end of if GPS
      if (new_ais_message == 1) {
        client.print(ais_sentence);
        new_ais_message = 0;
      }// end of if AIS
    }//end of while client connected
  }//end of if client

  client.stop();
  // Serial.println("Client disconnected");

}// end of loop
