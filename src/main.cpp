#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include "ESP8266mDNS.h"
#include "HardwareSerial.h"
#include "Updater.h"
#include "index.h"
#include "ArduinoOTA.h"
#include "FastLED.h"

#define DATA_PIN 5
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 90
CRGB leds[NUM_LEDS];

***REMOVED***
***REMOVED***

ESP8266WebServer server(80);

void handle_post() {
    server.send(200, "text/html", HTML_CONTENT);
    int count = server.args();
    for (int i = 0; i < count; ++i) {
        Serial.println(server.arg(i));
    }
}

void setup() {
    Serial.begin(115200);
    // for debugging
    // delay(1500);
    Serial.setDebugOutput(true);


    // WiFi.persistent(true);
    Serial.println("Booting");
    // WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    // delay(150);
    // WiFi.setPhyMode(WIFI_PHY_MODE_11G);
    // delay(150);
    WiFi.begin(SSID, PWD);
    // delay(1500);

    while (WiFi.status() != WL_CONNECTED) {
        /* Serial.print("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
        */
        Serial.print(".");
        delay(500);
    }

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else {
            type = "filesystem";
        }

        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        Serial.println("Start updating " + type);
        });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
        });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
        });

    ArduinoOTA.begin();

    Serial.println("Ready");
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

    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(255);

    for (int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = CRGB::Green;
    }

    Serial.println("Setup done");
}

void loop() {
    MDNS.update();
    ArduinoOTA.handle();
    server.handleClient();

    FastLED.show();
}
