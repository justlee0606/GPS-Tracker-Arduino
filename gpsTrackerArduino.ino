#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define FIREBASE_HOST "XXXXXXXXXXXXX" //--> URL address of Firebase Realtime Database.
#define FIREBASE_AUTH "XXXXXXXXXXXXX" //--> Firebase database secret code.

#define ON_Board_LED 2  //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router

//----------------------------------------SSID and Password of WiFi router or hotspot.
const char* ssid = "WIFI NAME";
const char* password = "WIFI PASSWORD";
//----------------------------------------

float cur_lat = 0.0;
float cur_long = 0.0;
float prev_lat = 0.0;
float prev_long = 0.0;
String pin = "UNIQUE PIN FOR LOCATION TRACKER";
TinyGPSPlus gps;
SoftwareSerial ss(5, 16);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ss.begin(9600);
  delay(500);

  WiFi.begin(ssid, password); //--> Connect to WiFi router
  Serial.println("");

  pinMode(ON_Board_LED, OUTPUT); //--> On Board LED port Direction output
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off Led On Board

  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //----------------------------------------Make the On Board Flashing LED on the process of connecting to the wifi router.
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
    //----------------------------------------
  }
  //----------------------------------------
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off the On Board LED when it is connected to the wifi router.
  //----------------------------------------If successfully connected to the wifi router, the IP Address that will be visited is displayed in the serial monitor.
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //----------------------------------------

  //----------------------------------------Firebase Realtime Database Configuration.
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //----------------------------------------
}

void loop() {
  // Get gps location from the module
  while (ss.available() > 0) {
    gps.encode(ss.read());
    if (gps.location.isUpdated())
    {
      cur_lat = gps.location.lat();
      cur_long = gps.location.lng();
      Serial.println("From GPS");
      Serial.println(cur_lat, 6);
      Serial.println(cur_long, 6);
      break;
    }
  }

  //----------------------------------------Send GPS data to the Firebase Realtime Database.

  //Code to send data or update data to the Firebase Realtime Database if GPS detects new location.

  if ((abs(cur_lat - prev_lat) > 0.001) || (abs(cur_long - prev_long) > 0.001)) {

    Firebase.setFloat(pin + "/lat", cur_lat);
    Firebase.setFloat(pin + "/long", cur_long);

    // Conditions for handling errors.
    if (Firebase.failed()) {
      Serial.print("Setting /Value failed :");
      Serial.println(Firebase.error());
      delay(500);
      return;
    }

    Serial.println("Setting successful");
    Serial.println();

    prev_lat = cur_lat;
    prev_long = cur_long;
  }
}
