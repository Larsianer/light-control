#include "ESP8266WiFi.h"
#incluse "index.h"

***REMOVED***
***REMOVED***

WiFiServer server(80);

void setup() {
    Serial.begin(9600);

    WiFi.begin(SSID, PWD);

    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    server.begin();

    Serial.println("Setup done");
}

void loop() {
    WiFiClient client = server.accept();
    if (client) {
        while (client.connected()) {
            if (client.available()) {
                String header = client.readStringUntil('\n');

                if (header.equals("\r")) {
                    break; // end of http request
                }

                Serial.print("<< ");
                Serial.println(header);
            }
        }

        // send the http response 
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();   // seperator between header and response

        String html = String(HTML_CONTENT);
        client.println(html);
        client.flush();

        // give the web browser time to receive the data
        delay(10);

        // close the connection
        client.stop();
    }
}
