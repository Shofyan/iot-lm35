#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

int outputpin= A0;
const int refresh=3;
float tValue;


#ifndef STASSID
#define STASSID "JerukManis" // Your WiFi SSID
#define STAPSK  "apelhijau" //Your WiFi password
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
ESP8266WebServer server(80);


void sendTemp() {

  String page = "<!DOCTYPE html>";
  page +="    <meta http-equiv='refresh' content='";
  page += String(refresh);// how often temperature is read
  page +="'/>";  
  page +="<html>";
  page +="<body>"; 
  page +="<h1>Shofyan nodemcu 8266</h1>";    
  page +="<p style=\"font-size:50px;\">Temperature<br/>";  
  page +="<p style=\"color:red; font-size:50px;\">";
  page += String(tValue, 2);
  page +=" Celcius</p>";  
  page +="</body>";  
  page +="</html>";  
 server.send(200,  "text/html",page);

}

void handleNotFound() {
 
  String message = "File Not Found";
  message += "URI: ";
  message += server.uri();
  message += "Method: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "Arguments: ";
  message += server.args();
  message += "";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "";
  }
  server.send(404, "text/plain", message);

}


void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    uint8_t macAddr[6];
    WiFi.macAddress(macAddr);
    Serial.printf("Connected, mac address: %02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
  }  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("shofyan test temp")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", sendTemp);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop()       

{
  server.handleClient();
  MDNS.update();
  int analogValue = analogRead(outputpin);
  float millivolts = (analogValue * 3300/1024.0)  ; 
  float celsius = millivolts;
  Serial.print("in DegreeC=   ");
  Serial.println(celsius);
  tValue =celsius;
  delay(100);

}
