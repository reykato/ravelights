#include <ArduinoWebsockets.h>

using namespace websockets;

WebsocketsClient client;

void initWebClient() {
  Serial.println("Starting Web Client.");
  
  if (!client.available()) {
    ws.textAll("Dartboard not available, connecting.");
    bool conn = client.connect("192.168.1.179", 80, "/ws");
    ws.textAll("Connection status: " + (String) conn);
    client.onMessage([&](WebsocketsMessage message){
      ws.textAll("Got Message: " + message.data());
      Serial.print("Got Message: ");
      Serial.println(message.data());
      if (message.data().equals("DB_HIT")) {
        hitRegistered = true;
      }
    });
  }
  //Serial.println("Web Client: " + (String)conn);  
}

void handleWSClient(void *parameter) {
  for (;;) {
    client.poll();
    vTaskDelay(20);
  }
}
