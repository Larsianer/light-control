#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include "ESP8266mDNS.h"
#include "HardwareSerial.h"
#include "Updater.h"
#include "index.h"
#include "ArduinoOTA.h"
#include "FastLED.h"
#include <string>

#define DATA_PIN 5
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 90
CRGB leds[NUM_LEDS];
CHSV color(90, 255, 255);
bool status = true;

std::string script(R"""(
<script>
    document.getElementById("enable").checked = rEnable;
    document.getElementById("hue").value = rHue;
    document.getElementById("sat").value = rSat;
    document.getElementById("val").value = rVal;
</script>
)""");


***REMOVED***
***REMOVED***

ESP8266WebServer server(80);

void enable_leds(bool enable) {
    if (enable) {
        for (int i = 0; i < NUM_LEDS; ++i) {
            leds[i] = color;
        }
    } else {
        for (int i = 0; i < NUM_LEDS; ++i) {
            leds[i] = CRGB::Black;
        }
    }
}

std::string prepare_html() {
    std::string ret = HTML_CONTENT;
    ret.append(script);
    int enable_pos = ret.find("rEnable");
    ret.replace(enable_pos, 7, status ? "true" : "false"); 
    int hue_pos = ret.find("rHue");
    ret.replace(hue_pos, 4, std::to_string(color.hue)); 
    int sat_pos = ret.find("rSat");
    ret.replace(sat_pos, 4, std::to_string(color.sat)); 
    int val_pos = ret.find("rVal");
    ret.replace(val_pos, 4, std::to_string(color.val)); 
    return ret;
} 

void handle_post() {
    int count = server.args();
    status = false;
    int hue, sat, val;
    for (int i = 0; i < count; ++i) {
        if (server.argName(i) == "enable") {
            status = true;
        } else if (server.argName(i) == "hue") {
            hue = server.arg(i).toInt();
        } else if (server.argName(i) == "sat") {
            sat = server.arg(i).toInt();
        } else if (server.argName(i) == "val") {
            val = server.arg(i).toInt();
        }
    }

    color.setHSV(hue, sat, val);

    enable_leds(status);
    server.send(200, "text/html", prepare_html().c_str());
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

    server.on("/", HTTP_GET, [](){ server.send(200, "text/html", prepare_html().c_str()); });
    server.on("/", HTTP_POST, handle_post);

    server.begin();

    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(255);

    enable_leds(status);

    Serial.println("Setup done");
}

void loop() {
    MDNS.update();
    ArduinoOTA.handle();
    server.handleClient();

    FastLED.show();
}
