#include "WiFi.h"
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "ESP_Color.h"
#include "ArduinoWebsockets.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


String lightStatus = "on";
void toggleOnOff() {
  if (lightsOn) {
    prevLedMode = ledMode;
    ledMode = 0;
    lightsOn = 0;
    lightStatus = "off";
  } else {
    ledMode = prevLedMode;
    lightsOn = 1;
    lightStatus = "on";
  }
  ws.textAll(lightStatus);
}

// Replaces placeholder with LED state value
String processor(const String& var){
  return "";
  if (lightsOn) {
    return "on";
  } else {
    return "off";
  }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String str = (char*)data;
    
    if (str == "toggle") {
      toggleOnOff();
    } else if (str.substring(0, 4) == "mode") {
      ledMode =  str.substring(4).toInt();
      lightsOn = 1;
      ws.textAll("on");
    } else if (str == "DB_HIT") {
      hitRegistered = true;
      ws.textAll("right avg: " + (String) rightTouchAvg + ", middle avg: " + (String) middleTouchAvg + ", left avg: " + (String) leftTouchAvg + ", leftTouch: " + (String) leftTouchVal + ", middleTouch: " + (String) middleTouchVal + ", rightTouch: " + (String) rightTouchVal);
    } else if (str.substring(0, 1) == "#") {
      c = ESP_Color::Color::FromHex(str);
    } else {
      brightness = str.toInt();
    }
  }
}

void onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      lightStatus = (lightsOn) ? "on" : "off";
      ws.textAll(lightStatus);
      ws.textAll("b" + String(brightness));
      ws.textAll("#" + c.ToHex(0, 0, 0));
      connectedClients = ws.count();
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      connectedClients = ws.count();
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      Serial.println("Web Socket Error!");
      break;
  }
}

void initWebSocket() {
  Serial.println("Initializing WebSocket.");
  ws.onEvent(onWSEvent);
  server.addHandler(&ws);
  Serial.println("WebSocket Initialized.");
}
 
void startWebServer() {
  Serial.println("Starting Web Server");
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    return;
  }

  Serial.println("Starting mDNS.");
  if(!MDNS.begin("ravelights")) {
     Serial.println("Error starting mDNS");
     return;
  }
  Serial.println("mDNS started.");

  initWebSocket();  

  // Route for index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to load script.js file
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", "text/javascript");
  });

  // Start server
  server.begin();
}



void handleWS(void *parameter) {
  for(;;) {
    ws.cleanupClients();
    connectedClients = ws.count();
    if (connectedClients > 0) {
      
      //ws.textAll(getBatteryVoltage());
    }
    vTaskDelay(1000);
  }
}
