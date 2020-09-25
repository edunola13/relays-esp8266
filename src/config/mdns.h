#include <Arduino.h>

// Alternative you can create and instance of MDNS
// MDNS = MDNSResponder();

void initMDNS() {
  if (!MDNS.begin(DEVICE_ID))
  {
    DEB_DO_PRINTLN("Error iniciando mDNS");
  }
  DEB_DO_PRINTLN("mDNS iniciado");
  // Register a know service and protocol (http and tcp)
  MDNS.addService("http", "tcp", 80); // Esto puede ser con _, _http y _tcp

  // Register me internal service
  MDNS.addService("iot", "tcp", 80);
  // Register info of service
  MDNS.addServiceTxt("iot", "tcp", "type", DEVICE_TYPE);

  // We can add dynamic info. For that we need to reset and set again all
  // MDNS.clearServiceTxt("iot", "tcp")
  // MDNS.addServiceTxt(...)
}
