#include <Arduino.h>

void get_topic(char *buffer, char *extra) {
  String topic = String(MQTT_CHANNEL_PREFIX) + String(DEVICE_ID) + String(extra);
  topic.toCharArray(buffer, topic.length() + 1);
}

void subscribe_mqtt(PubSubClient &mqttClient) {
  char topic[50];
  get_topic(topic, "/sub");
  mqttClient.subscribe(topic);
}

void publish_relays() {
  StaticJsonDocument<200> jsonBuffer;
  char JSONmessageBuffer[200];
  JsonArray array = jsonBuffer.to<JsonArray>();

  for (uint8_t i= 0; i < sizeof(relays) / sizeof(Relay); i++) {
    JsonObject nested = array.createNestedObject();
    nested["id"] = i + 1;
    nested["on"] = relays[i].isOpen();
  }

  serializeJson(jsonBuffer, JSONmessageBuffer);

  char topic[50];
  get_topic(topic, "/pub");
  mqttClient.publish(topic, JSONmessageBuffer);
}

void callback_mqtt(char* topic, byte* payload, unsigned int length) {
  String buffer;
  for (unsigned int i = 0; i < length; i++) {
    buffer += (char)payload[i];
  }
  StaticJsonDocument<500> jsonBuffer;
  DeserializationError error = deserializeJson(jsonBuffer, buffer);

  if (error) {
    return;
  }

  if (! jsonBuffer.containsKey("method")) {
    return;
  }

  String method = jsonBuffer["method"];
  if (method == "refresh") {
    publish_relays();
    return;
  }

  if (method == "update") {
    JsonArray relaysBuffer = jsonBuffer["relays"].as<JsonArray>();
    for(JsonVariant rBuffer : relaysBuffer) {
      uint8_t id = rBuffer["id"].as<int>() - 1;
      if (id >= 0 || id < sizeof(relays) / sizeof(Relay)) {
        bool on = rBuffer["on"];
        change_status_relay(relays[id], on);
        Serial.println(on);
      }
    }

    publish_relays();
  }

}
