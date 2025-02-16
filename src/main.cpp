#include <ArduinoHA.h>
#include <ArduinoOTA.h>
#include <FastLED.h>
#include "secrets.h"

#define BROKER_ADDR IPAddress(192, 168, 2, 145)
#define DATA_PIN 5
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 90

CRGB leds[NUM_LEDS];
CRGB ledColor(255, 255, 255);

WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);

HALight light("bookshelf", HALight::BrightnessFeature | HALight::RGBFeature);

void updateLeds(CRGB newColor) {
    for (int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = newColor;
    }
}

void enable_leds(bool enable) {
    if (enable) {
        updateLeds(ledColor);
    } else {
        updateLeds(CRGB::Black);
    }
}

void onStateCommand(bool state, HALight* sender) {
    enable_leds(state);
    sender->setState(state);
}

void onBrightnessCommand(uint8_t brightness, HALight* sender) {
    FastLED.setBrightness(brightness);
    sender->setBrightness(brightness);
}

void onRGBColorCommand(HALight::RGBColor color, HALight* sender) {
    ledColor.red = color.red; 
    ledColor.green = color.green; 
    ledColor.blue = color.blue; 
    updateLeds(ledColor);
    sender->setRGBColor(color);
}

void setup() {
    Serial.begin(115200);
    // for debugging
    /*delay(1500);*/
    Serial.setDebugOutput(true);

    // WiFi.persistent(true);
    Serial.println("Booting");

    byte mac[WL_MAC_ADDR_LENGTH];
    WiFi.macAddress(mac);
    device.setUniqueId(mac, sizeof(mac));

    device.enableSharedAvailability();
    device.enableLastWill();

    // WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    /*delay(150);*/
    // WiFi.setPhyMode(WIFI_PHY_MODE_11G);
    // delay(150);
    WiFi.begin(SSID, PWD);
    /*delay(1500);*/

    while (WiFi.status() != WL_CONNECTED) {
        /* Serial.print("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
        */
        Serial.print(".");
        delay(500);
    }

    // Begin: OTA code
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
    // End: OTA code

    Serial.println("Ready");
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
    FastLED.setBrightness(255);

    // set the light before connecting to the mqtt client, so that it is reported correctly
    light.setCurrentRGBColor(HALight::RGBColor(ledColor.red, ledColor.green, ledColor.blue));
    light.setCurrentBrightness(255);
    light.setCurrentState(true);

    // enable light on startup
    enable_leds(true);

    // set device details
    device.setName("Esp8266 on bookshelf");
    device.setSoftwareVersion("0.1.1");

    // handle light states
    light.onStateCommand(onStateCommand);
    light.onBrightnessCommand(onBrightnessCommand);
    light.onRGBColorCommand(onRGBColorCommand);

    mqtt.begin(BROKER_ADDR);

    Serial.println("Setup done");
}

void loop() {
    ArduinoOTA.handle();

    mqtt.loop();
    FastLED.show();
}
