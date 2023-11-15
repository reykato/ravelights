#include "WiFiUDP.h"

//define ip and port for udp
IPAddress UDP_ADDRESS = IPAddress(192,168,1,179);
#define UDP_PORT 6969

//create udp object
WiFiUDP udp;

void initUDP() {
  Serial.println("Starting UDP Client.");
  udp.begin(UDP_PORT);
  ws.textAll("Connected to UDP");
}

void handleUDPClient(void *parameter) {
  for (;;) {
    if(udp.parsePacket() > 0) {
      char buff[100];
      udp.read(buff, size_t(100));
      String message = String(buff);
      //ws.textAll("Got new udp message: " + message);
      if(message.equals("DB_HIT")) hitRegistered = true;
    }
    vTaskDelay(1);
  }
}
