#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>    
#include "SdsDustSensor.h"
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WifiLocation.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN D8
#define LED_COUNT 1
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

int rxPin = 14;
int txPin = 12;
SdsDustSensor sds(rxPin, txPin);
String readString;
/////////////////////////////////////////////////////////////
const char* ssid = "";
const char* password = "";
////////////////////////////////////////////////////////////
const char* ssid1 = "";
const char* password1 = "";
/////////////////////////////////////////////////////////////
const char* ssid2 = "";
const char* password2 = "";
////////////////////////////////////////////////////////////
const char* device_name = "";                                        // Set your device name !!!!!

const char* googleApiKey = "";
const char* host="http://jsonplaceholder.typicode.com/";
////////////////////////////////////////////////////////////

double pm2_5;
double pm10;
int sensorVal1;
int sensorVal2;
int sensorVal3;
float temps_ref;
float temps_ref2;
double value1;
double value2;
String lat1;
String long2;
int i;

WifiLocation location(googleApiKey);
WiFiServer server(80);
ESP8266WiFiMulti wifiMulti;

void setup()                                                     //SETUP Start
{
  
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  
  wifiMulti.addAP(ssid, password);   // add Wi-Fi networks you want to connect to
  wifiMulti.addAP(ssid1, password1);
  wifiMulti.addAP(ssid2, password2);

  Serial.println("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer

  if (!MDNS.begin("esp8266")) {             // Start the mDNS responder for esp8266.local
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
  
  Serial.println(sds.queryFirmwareVersion().toString()); // prints firmware version
  Serial.println(sds.setActiveReportingMode().toString()); // ensures sensor is in 'active' reporting mode
  Serial.println(sds.setContinuousWorkingPeriod().toString()); // ensures sensor has continuous working period - default but not recommended
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  
  MDNS.addService("http", "tcp", 80);
  server.begin();
  sds.begin();  
  
  while (WiFi.status() != WL_CONNECTED) {
    
   Serial.print(".");
   delay(500);
   strip.setPixelColor(0, strip.Color(255, 165, 0));
   strip.show();
   delay(500);
   strip.setPixelColor(0, strip.Color(30, 144, 255)); 
   strip.show();         
  } 
}

void loop()                                                   // Loop start
{
MDNS.update();
PmResult pm = sds.queryPm();
  if (pm.isOk()) {
    pm2_5 = pm.pm25;
    pm10 = pm.pm10;
    value1 = value1 + pm.pm25;
    value2 = value2 + pm.pm10;
    i++;
  }

while (i >= 20)
{
  value1 = value1 / i;
  value2 = value2 / i;  
  Serial.println(value1);
  Serial.println(value2);
  location_t loc = location.getGeoFromWiFi(); //get location with google api
  lat1 = String(loc.lat, 7) ;
  long2 = String(loc.lon, 7) ;
  //sendData(value1, value2, sensorVal1, sensorVal2, sensorVal3, String(loc.lat, 7), String(loc.lon, 7));
  updateLed(value1, value2);
  value1 = 0; 
  value2 = 0; 
  i = 0;
}

WiFiClient client = server.available();

  if (!client)
  {
  return;
  }

  delay(500);
  // Read the first line of HTTP request
  String req = client.readStringUntil('\r');

  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  
  if (addr_start == -1 || addr_end == -1) {
    Serial.print("Invalid request: ");
    Serial.println(req);
    return;
  }
  req = req.substring(addr_start + 1, addr_end);
  Serial.print("Request: ");
  Serial.println(req);
 
  IPAddress ip = WiFi.localIP();
   
//  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>GPS Interfacing with NodeMCU</title> <style>";
//  String s = "<html lang='fr'><head><meta http-equiv='refresh' content='60' name='viewport' content='width=device-width, initial-scale=1'/>";
//s += "<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'><script src='https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js'></script><script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script>";
//s += "<div class='container-fluid' style='min-height: 100%;'>";
// String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>GPS Interfacing with NodeMCU</title> <style>";
String s = "<html lang='fr'><head><meta http-equiv='refresh' content='60' name='viewport' content='width=device-width, initial-scale=1'/>";

s += "<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'>";
s += "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js'></script>";
s += "<script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script>";

s += "<body style='background:url('https://zupimages.net/up/19/29/jblq.jpg') no-repeat center/100% 100%;'>";
s += "<div class='container container-fluid px-5' style='min-height: 100%;'>";
s += "<div class='row'>";
s += "<div class='col-md-12'>";
s += "<h3 class='text-center'>";
s += device_name;
s += " Device DATA";
s += "</h3>";
s += "<br /><br />";
s += "<table class='table text-center'>";
s += "<tbody>";
s += "<tr>";
s += "<td>PM2.5</td>";
s += "<td>";
s += pm2_5;
s += "</td>";
s += "</tr>";
s += "<tr class='table-active'>";
s += "<td>PM10</td>";
s += "<td>";
s += pm10;
s += "</td>";
s += "</tr>";
s += "<tr class='table-success'>";
s += "<td>Latitude</td>";
s += "<td>";
s += lat1;
s += "</td>";
s += "</tr>";
s += "<tr class='table-warning'>";
s += "<td>Longitude</td>";
s += "<td>";
s += long2;
s += "</td>";
s += "</tr>";
s += "</tbody>";
s += "<th />";
s += "</table>";
s += "<h3 class='text-center'>My FEELING</h3>";
s += "<br /><br />";
s += "<div class='row'>";
s += "<div class='col-md-12'>";
s += "<a class='btn btn-success btn-lg btn-block' style='padding-top:3%;padding-bottom:3%;' href=\"/BIEN\"\">FRESH AIR</a>";
s += "<br /><br />";
s += "<a class='btn btn-primary btn-lg btn-block' style='padding-top:3%;padding-bottom:3%;' href=\"/STD\"\">COMMON AIR</a>";
s += "<br /><br />";
s += "<a class='btn btn-danger btn-lg btn-block' style='padding-top:3%;padding-bottom:3%;' href=\"/PASBIEN\"\">STALE AIR</a>";
s += "<br />";
s += "</div>";
s += "</div>";
s += "</div>";
s += "</div>";
s += "</div>";
s += "<p align=center><a style='color:RED;font-size:125%;' href='http://maps.google.com/maps?&z=15&mrt=yp&t=k&q='";
s += lat1;
s += "+";
s += long2;
s += "'target=''_top'>";
s += "Click here!</a> To check the location in Google maps.</p>";
s += "</body> </html> \n";
 
  String request = client.readStringUntil('\r');
  client.flush();

  if (request.indexOf("/BIEN") != -1)  {
    Serial.print("button1");
    sensorVal1 = 1; 
    sensorVal2 = 0; 
    sensorVal3 = 0;
  }
  if (request.indexOf("/PASBIEN") != -1)  {
    Serial.print("button2");
  }
    if (request.indexOf("/PASBIEN") != -1)  {
    Serial.print("button2");
  }

  client.print(s);
  delay(100);
}


void sendData(double z, double k, int a, int b, int c, String d, String e)
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  String string_a     =  String(a, 2);
  String string_b     =  String(b, 2);
  String string_c     =  String(c, 2);
  String string_z     =  String(z, 2);
  String string_k     =  String(k, 2);
  root["devise"] = device_name;
  root["PM2_5"] = string_z;
  root["PM10"] = string_k;
  root["mixed_feeling"] = string_b;
  root["negative_feeling"] = string_c;
  root["positive_feeling"] = string_a;
  root["latitude"] =  d;
  root["longitude"] = e;  
  root.printTo(Serial);
  char JSONmessageBuffer[300];
  root.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println(JSONmessageBuffer);

 if (WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
 
  HTTPClient http;    //Declare object of class HTTPClient
  http.begin("http://simple-ai4good-sensors-api.herokuapp.com/data");      //Specify request destination
  http.addHeader("Content-Type", "application/json");  //Specify content-type header
  int httpCode = http.POST(JSONmessageBuffer);   //Send the request
  String payload = http.getString();                  //Get the response payload
  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  http.end();  //Close connection 
  }
 else 
  { 
    delay(500);
    strip.setPixelColor(0, strip.Color(255, 165, 0));
    strip.show();
    delay(500);
    strip.setPixelColor(0, strip.Color(30, 144, 255)); 
    strip.show();  
  }
}

void updateLed(double a, double b)     //Function UpdateOled Pm value
{
  int myInt = (int) a;
  switch(myInt)
  {
    case 0 ... 10:
    strip.setPixelColor(0, strip.Color(0, 128, 0)); 
  break;
  case 11 ... 20:
    strip.setPixelColor(0, strip.Color(255, 165, 0));
  break;
  case  21 ... 30: 
    strip.setPixelColor(0, strip.Color(127, 0, 0));
  break;
  }
  strip.show();  
  }
