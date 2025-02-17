// Sender ESP32 (Controller) - Sends accelerometer and touch data via OSC
#include <WiFi.h>
#include <Wire.h>
#include <math.h>
#include <OSCMessage.h>
#include <WiFiUdp.h>
#include config.h

WiFiUDP Udp;
const unsigned int outPort = 8000;

const int x_out = A0, y_out = A3, z_out = A6;
bool touchDetected[6] = {false};

struct TouchMapping {
    int index;
    const char* oscAddress;
};

TouchMapping touchMap[] = {
    {0, "/33/toggle"},
    {1, "/55/toggle"},
    {2, "/65/toggle"},
    {3, "/77/toggle"},
    {4, "/88/toggle"},
    {5, "/99/toggle"}
};

void gotTouch(int index) { touchDetected[index] = true; }

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Udp.begin(outPort);
    Serial.println("ESP32 Touch OSC Sender");

    touchAttachInterrupt(T3, [](){ gotTouch(0); }, 50);
    touchAttachInterrupt(T5, [](){ gotTouch(1); }, 0);
    touchAttachInterrupt(T6, [](){ gotTouch(2); }, 0);
    touchAttachInterrupt(T7, [](){ gotTouch(3); }, 15);
    touchAttachInterrupt(T8, [](){ gotTouch(4); }, 35);
    touchAttachInterrupt(T9, [](){ gotTouch(5); }, 35);
}

void loop() {
    for (const auto& touch : touchMap) {
        if (touchDetected[touch.index]) {
            Serial.printf("Touch %d detected\n", touch.index + 3);
            OSCMessage msg(touch.oscAddress);
            Udp.beginPacket(outIp, outPort);
            msg.send(Udp);
            Udp.endPacket();
            msg.empty();
            touchDetected[touch.index] = false;
        }
    }

    int x_adc = analogRead(x_out), y_adc = analogRead(y_out), z_adc = analogRead(z_out);
    double x_g = (((x_adc * 3.3) / 4096 - 1.65) / 0.330);
    double y_g = (((y_adc * 3.3) / 4096 - 1.65) / 0.330);
    double z_g = (((z_adc * 3.3) / 4096 - 1.80) / 0.330);

    OSCMessage xMsg("/accel/x"); xMsg.add(x_g);
    OSCMessage yMsg("/accel/y"); yMsg.add(y_g);
    OSCMessage zMsg("/accel/z"); zMsg.add(z_g);

    Udp.beginPacket(outIp, outPort); xMsg.send(Udp); Udp.endPacket(); xMsg.empty();
    Udp.beginPacket(outIp, outPort); yMsg.send(Udp); Udp.endPacket(); yMsg.empty();
    Udp.beginPacket(outIp, outPort); zMsg.send(Udp); Udp.endPacket(); zMsg.empty();
    
    delay(1000);
}

