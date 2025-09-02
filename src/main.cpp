#define BOOKSHELF
#include "Adafruit_SGP30.h"
#include <ArduinoHADefines.h>
#include <ArduinoHA.h>
#include <ArduinoOTA.h>
#include <FastLED.h>
#include <Adafruit_AHTX0.h>
#include <TelnetStream.h>
#include "secrets.h"

// include the right header file and for ide completion define some placeholders
#ifdef BOOKSHELF
#define NUM_LEDS 90
#define DATA_PIN 4
// the built in LED uses pin 2,
#define LED_PIN 2
#define NAME "bookshelf"
const byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4B};
#endif

#ifdef DESK
#define NUM_LEDS 60
#define DATA_PIN 2
#define NAME "desk"
const byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
#endif

#ifndef BOOKSHELF 
#ifndef DESK
#define NUM_LEDS 0
#define DATA_PIN 2
#define NAME "test"
const byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4C};
#endif
#endif

#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];
CRGB ledColor(255, 255, 255);
bool enable = true;
bool animate = false;

WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);

HALight light(NAME, HALight::BrightnessFeature | HALight::RGBFeature);
HAButton restartButton("restartButton");
HASwitch animateSwitch("animateSwitch");

#ifdef DESK
HASensorNumber tempHA("temp", HABaseDeviceType::PrecisionP1);
HASensorNumber humidityHA("humidity");
Adafruit_AHTX0 aht;
Adafruit_SGP30 sgp;
unsigned long intervall = 5000;
unsigned long lastMillis = 0;
#endif

void updateLeds(CRGB newColor) {
    for (int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = newColor;
    }
}

void animateLeds() {
    // the amplitue of the wave, in range [0, 255]
    float amplitude = 127.5;
    // the period of the wave, in s
    float period = 3.0;
    // the phase velocity of the wave, in m
    float wavelength = 1.5;

    for (int i = 0; i < NUM_LEDS; ++i) {
        CRGB newColor = ledColor;
        newColor.r = qsub8(ledColor.r, int(amplitude * sin(2.0 * PI / period * (millis() / 1000.0) - (2.0 * PI / wavelength * i / 30.0)) + amplitude));
        newColor.g = qsub8(ledColor.g, int(amplitude * sin(2.0 * PI / period * (millis() / 1000.0) - (2.0 * PI / wavelength * i / 30.0)) + amplitude));
        newColor.b = qsub8(ledColor.b, int(amplitude * sin(2.0 * PI / period * (millis() / 1000.0) - (2.0 * PI / wavelength * i / 30.0)) + amplitude));
        leds[i] = newColor;
    }
}

void enableLeds(bool enable) {
    if (enable) {
        updateLeds(ledColor);
    } else {
        updateLeds(CRGB::Black);
    }
}

void onButtonCommand(HAButton* sender) {
    if (sender->getName() == restartButton.getName()) {
        ESP.restart();
    }
}

void onSwitchCommand(bool state, HASwitch* sender) {
    if (sender->getName() == animateSwitch.getName()) {
        animate = state;
        sender->setState(state);
    }
}

void onStateCommand(bool state, HALight* sender) {
    enable = state;
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

    // WiFi.persistent(true);
    Serial.println("Booting");

    device.setUniqueId(mac, sizeof(mac));
    device.enableExtendedUniqueIds();
    Serial.println("unique id");
    Serial.println(device.getUniqueId());

    device.enableSharedAvailability();
    device.enableLastWill();

    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    delay(150);
    WiFi.setPhyMode(WIFI_PHY_MODE_11G);
    delay(150);
    WiFi.begin(SSID, PWD);
    delay(1500);

    while (WiFi.status() != WL_CONNECTED) {
        /* Serial.print("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
        */
        Serial.print(".");
        delay(500);
    }

#ifdef DESK
    while (!aht.begin()) {
        Serial.println("Could not find AHT. Check wiring");
        delay(10);
    }
    Serial.println("AHTx0 found!");
#endif
#ifdef BOOKSHELF
    pinMode(LED_PIN, OUTPUT);
    // disable the built in LED (active low)
    digitalWrite(LED_PIN, HIGH);
#endif

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

    TelnetStream.begin();

    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
    FastLED.setBrightness(255);

    // set the light before connecting to the mqtt client, so that it is reported correctly
    light.setCurrentRGBColor(HALight::RGBColor(ledColor.red, ledColor.green, ledColor.blue));
    light.setCurrentBrightness(255);
    light.setCurrentState(true);

    // enable light on startup
    enableLeds(enable);

    device.setName(NAME);
    device.setSoftwareVersion("1.2");
    device.setManufacturer("Larsianer");

    // handle light states
    light.onStateCommand(onStateCommand);
    light.onBrightnessCommand(onBrightnessCommand);
    light.onRGBColorCommand(onRGBColorCommand);

    // handle restart button
    restartButton.setDeviceClass("restart");
    restartButton.setName("Restart Button");
    restartButton.onCommand(onButtonCommand);

    // handle animate button
    animateSwitch.setName("Animate LEDs");
    animateSwitch.onCommand(onSwitchCommand);
    // set the state before connecting to mqtt, thus it gets reported correctly
    animateSwitch.setCurrentState(animate);

    // handle sensor setup
#ifdef DESK
    tempHA.setDeviceClass("temperature");
    tempHA.setUnitOfMeasurement("°C");
    humidityHA.setDeviceClass("humidity");
    humidityHA.setUnitOfMeasurement("%");
    lastMillis = millis();
#endif

    mqtt.begin("lars-pi");

    Serial.println("Setup done");
}

// TODO: implement sgp30 logic for measuring and baseline handling
unsigned long int lastAnim = 0;

void loop() {
    ArduinoOTA.handle();

    mqtt.loop();
    FastLED.show();

    unsigned long currentMillis = millis();

    if (currentMillis - lastAnim > 16) {
        if (enable && animate) {
            animateLeds();
        } else if (enable) {
            enableLeds(true);
        } else {
            enableLeds(false);
        }
        lastAnim = currentMillis;
    }
#ifdef DESK
    if (currentMillis - lastMillis > intervall) {
        // get sensor values
        sensors_event_t humidity, temp;
        aht.getEvent(&humidity, &temp);
        tempHA.setValue(temp.temperature);
        humidityHA.setValue(humidity.relative_humidity);
        lastMillis = currentMillis;
    }
#endif
}
