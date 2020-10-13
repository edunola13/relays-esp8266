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

void publish_sync(uint8_t type) {
  StaticJsonDocument<500> jsonBuffer;
  char JSONmessageBuffer[500];

  jsonBuffer["type"] = type;
  jsonBuffer["sub_type"] = PAYLOAD_SUB_TYPE_NONE;

  JsonObject device = jsonBuffer.createNestedObject("device");
  // device["device_id"] = String(DEVICE_ID);
  if (type == PAYLOAD_TYPE_STATE) {
    device["name"] = config.name;
  }

  JsonArray traits = jsonBuffer.createNestedArray("traits");
  for (uint8_t i= 0; i < sizeof(relays) / sizeof(Relay); i++) {
    JsonObject trait = traits.createNestedObject();
    trait["id"] = String(i + 1);
    JsonObject values = trait.createNestedObject("values");
    values["on"] = String(relays[i].isOpen());
  }

  serializeJson(jsonBuffer, JSONmessageBuffer);

  char topic[50];
  get_topic(topic, "/pub");
  Serial.println(JSONmessageBuffer);
  mqttClient.publish(topic, JSONmessageBuffer);
}

void publish_relay(uint8_t id) {
  StaticJsonDocument<500> jsonBuffer;
  char JSONmessageBuffer[500];

  jsonBuffer["type"] = PAYLOAD_TYPE_STATE;
  jsonBuffer["sub_type"] = PAYLOAD_SUB_TYPE_NOTIFY;

  JsonArray traits = jsonBuffer.createNestedArray("traits");
  JsonObject trait = traits.createNestedObject();
  trait["id"] = String(id + 1);
  JsonObject values = trait.createNestedObject("values");
  values["on"] = String(relays[id].isOpen());

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

  if (! jsonBuffer.containsKey("type")) {
    return;
  }

  uint8_t type = jsonBuffer["type"];

  // Return state only if in SYNC MODE
  // if (type == "REGISTER") {
  //   if (status.sync) {
  //     publish_sync();
  //   }
  // }

  // Return Sync and set sync to false
  if (type == PAYLOAD_TYPE_SYNC && status.sync) {
    publish_sync(type);
    status.sync = false;
  }

  // Return actual state
  if (type == PAYLOAD_TYPE_STATE) {
    publish_sync(type);
  }

  // Change state and return
  if (type == PAYLOAD_TYPE_EXECUTE) {
    JsonArray traitsBuffer = jsonBuffer["traits"].as<JsonArray>();
    for(JsonVariant traitBuffer : traitsBuffer) {
      uint8_t id = traitBuffer["id"].as<int>() - 1;
      if (id >= 0 || id < sizeof(relays) / sizeof(Relay)) {
        JsonObject valuesBuffer = traitBuffer["values"].as<JsonObject>();
        bool on = valuesBuffer["on"].as<int>();
        change_status_relay(relays[id], on);
        publish_relay(id);
      }
    }
  }
}
