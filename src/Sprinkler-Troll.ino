/*
 * Project Sprinkler-Troll
 * Description: Sprinkler Controller
 * Author: Veenema Design Works
 */
#include <ArduinoJson.h>

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {

  bool wifiReady = WiFi.ready();
	bool cloudReady = Particle.connected();
  static uint32_t firstAvailable;

	if (wifiReady) {
		if (firstAvailable == 0) {
			firstAvailable = millis();
		}
		if (millis() - firstAvailable > 30000) {
			publishMessage("googleDocs","test");
      delay(2000);
			System.sleep(30);
		}
	}
	else {
		firstAvailable = 0;
	}





  // Wait 1 hour
  //System.sleep(30);
  // Wait 10 seconds
  delay(10000);
}

void publishMessage(const char* destination, const char* message){
  const size_t bufferSize = JSON_OBJECT_SIZE(2)+80;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  root["message"] = message;
  root["time"] = Time.now();

  char buffer[bufferSize];
  root.printTo(buffer);

  Particle.publish(destination, buffer, 60, PRIVATE);

  return;
}
