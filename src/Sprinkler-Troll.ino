/*
 * Project Sprinkler-Troll
 * Description: Sprinkler Controller
 * Author: Veenema Design Works
 */
#include "publishMessage.h"
#include <ArduinoJson.h>

const pin_t SWITCH_PIN = D3;

const char* sprinklerTimeMessage = "{\"duration\":1200,\"deadline\":1522919753}";

//TODO
// struct SprinklerStats {
//   uint8_t version;
//   uint32_t duration;
//   uint32_t deadline;
//   uint32_t targetStartTime;
// }
uint32_t duration = 0;
uint32_t deadline = 0;

void setup() {
  pinMode(SWITCH_PIN,INPUT_PULLUP);
}


void loop() {
	bool cloudReady = Particle.connected();
  static uint32_t firstAvailable;

	if (cloudReady) {
		if (firstAvailable == 0) {
			firstAvailable = millis();
      parseMessage(sprinklerTimeMessage);
      String outmessage = Time.timeStr(deadline) + ": " + String(duration);
      Particle.publish("message",outmessage,60,PRIVATE);
      Particle.process();
		}
		if (millis() - firstAvailable > 30000) {
      if(!digitalRead(SWITCH_PIN)){
        publishMessage("googleDocs","Swith ON", PROCESS);
      } else {
        publishMessage("googleDocs", "Switch OFF", PROCESS);
      }
      Particle.process();
			System.sleep(SLEEP_MODE_DEEP,30);
		}
	}
	else {
		firstAvailable = 0;
	}
}

void parseMessage(const char* message){
  const size_t bufferSize = JSON_OBJECT_SIZE(2) + 40;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.parseObject(message);

  duration = root["duration"]; // 1200
  deadline = root["deadline"]; // 1522919753
}
