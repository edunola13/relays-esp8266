#include <Arduino.h>
#include <ESP8266WebServer.h>
// #include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

#define DOMO_DEBUG
#define DOMO_SPEED 9600
//#define USE_WDT
//#define WDT_TIME WDTO_8S
// UUID -> Every provider assign a unique ID
// It's the way to identify every device.
#define DEVICE_ID "6e0d871c-7b12-479d-aaa7-227e0e36ccc4"
#define DEVICE_TYPE "RELAYS"
#define DEVICE_VERSION "1"

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 20
#define WIFI_REINTENT_AFTER_SETUP 1000*60*5  // 5 Minutes reintent if ssid is set
#define MQTT_REINTENT_AFTER_SETUP 1000*60*1  // 1 Minutes reintent if ssid is set
#define MQTT_CLIENT_PREFIX ""
#define MQTT_CHANNEL_PREFIX "client/"

#include <common_initial.h>
#include "messages.h"
// Dont touch this
#include "core/config.h"
#include "core/memory.h"
#include "core/controllers.h"

// Your Modules
#define RELAY_CONFIG_POSITION 200  // From where store relay config
#include "modules/relays.h"

// Your Configuration
#include "config/mdns.h"
#include "config/mqtt.h"
#include "config/memory.h"
#include "config/controllers.h"

void setup(void) {
    Serial.begin(9600);
    initialGeneric();
    DEB_DO_PRINTLN(MSG_START);

    // Leer config de Epprom
    loadConfig();
    // Inicializo WiFi
    initWifi();
    // Inicializo MDNS
    initMDNS();

    // Config rest server routing
    config_rest_server_routing();

    DEB_DO_PRINTLN("HTTP REST Server Started");
}

void loop(void) {
    MDNS.update();

    // Test WiFi Connection
    if (status.status != 'C' && String(config.ssid) != "") {
      reconnect_wifi();
    }

    // Handle Requests
    http_rest_server.handleClient();
    // http_rest_server_ssh.handleClient();

    if (!mqttClient.connected()) {
      reconnect();
    }
    mqttClient.loop();
}
