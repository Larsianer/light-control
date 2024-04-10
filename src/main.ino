#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include "ESP8266mDNS.h"
#include "index.h"

***REMOVED***
***REMOVED***

enum class Status {HEADER, BODY};

ESP8266WebServer server(80);

void setup() {
    Serial.begin(115200);
    // for debugging
    // delay(1500);
    Serial.setDebugOutput(true);


    // WiFi.persistent(true);
    Serial.print("Connecting");
    // WiFi.disconnect();
    // WiFi.mode(WIFI_STA);
    // delay(150);
    // WiFi.setPhyMode(WIFI_PHY_MODE_11G);
    // delay(150);
    WiFi.begin(SSID, PWD);
    // delay(1500);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("esp8266")) {
        Serial.println("MDNS responder started");
    }

    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);

    server.on("/", HTTP_GET, [](){ server.send(200, "text/html", HTML_CONTENT); });
    server.on("/", HTTP_POST, handle_post);

    server.begin();

    Serial.println("Setup done");
}

void loop() {
    MDNS.update();
    server.handleClient();
}

void handle_post() {
    server.send(200, "text/html", HTML_CONTENT);
    int count = server.args();
    for (int i = 0; i < count; ++i) {
        Serial.println(server.arg(i));
    }
}
