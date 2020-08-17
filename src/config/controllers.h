#include <Arduino.h>
#include <ESP8266WebServer.h>
// #include <ESP8266WebServerSecure.h>

void get_relays();
void get_relay();
void put_relay();

void config_routing_modules(ESP8266WebServer &http_rest_server) {
  // Make your routes
  http_rest_server.on("/relays", HTTP_GET, get_relays);  // Relays
  http_rest_server.on("/relays/detail", HTTP_GET, get_relay);
  http_rest_server.on("/relays/detail", HTTP_PUT, put_relay);
}

// void config_routing_modules_secure(BearSSL::ESP8266WebServerSecure &http_rest_server) {
//   DO SOMETHING
// }

//
// Logic of your routes
//

void _response_relay(Relay &relay) {
  StaticJsonDocument<255> jsonBuffer;
  char JSONmessageBuffer[255];

  jsonBuffer["start"] = relay.getStarted();
  jsonBuffer["pin"] = relay.getPinD();
  jsonBuffer["on"] = relay.isOpen();

  serializeJson(jsonBuffer, JSONmessageBuffer);
  http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void get_relays() {
  if (! has_access()) {
    return;
  }

  StaticJsonDocument<500> jsonBuffer;
  char JSONmessageBuffer[500];
  JsonArray array = jsonBuffer.to<JsonArray>();

  for (uint8_t i= 0; i < sizeof(relays) / sizeof(Relay); i++) {
    JsonObject nested = array.createNestedObject();
    nested["id"] = i + 1;  // Start from 1
    nested["start"] = relays[i].getStarted();
    nested["pin"] = relays[i].getPinD();
    nested["on"] = relays[i].isOpen();
  }

  serializeJson(jsonBuffer, JSONmessageBuffer);
  http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void get_relay() {
  if (! has_access()) {
    return;
  }

  uint8_t id = http_rest_server.arg("id").toInt() - 1;  // Start from 1
  if (id < 0 || id >= sizeof(relays) / sizeof(Relay)) {
      http_rest_server.send(404);
  }
  _response_relay(relays[id]);
}

void put_relay() {
  if (! has_access()) {
    return;
  }

  uint8_t id = http_rest_server.arg("id").toInt() - 1;  // Start from 1
  if (id < 0 || id >= sizeof(relays) / sizeof(Relay)) {
      http_rest_server.send(404);
  }

  StaticJsonDocument<500> jsonBuffer;
  String post_body = http_rest_server.arg("plain");

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(jsonBuffer, post_body);
  if (error) {
      Serial.println("error in parsin json body");
      http_rest_server.send(400, "application/json", "{\"error\": \"Invalid Json\"}");
  }
  else {
    bool on = jsonBuffer["on"];
    change_status_relay(relays[id], on);

    _response_relay(relays[id]);

    publish_relays();
  }
}
