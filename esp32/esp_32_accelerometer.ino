#include <WiFi.h>
#include <Wire.h>
#include <math.h>
#include <OSCMessage.h>
#include <WiFiUdp.h>
#include "config.h"  // Import WiFi credentials and IP

WiFiUDP Udp;

double roll, pitch, yaw;
const int x_out = A0, y_out = A3, z_out = A6;
int del = 500;
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

    // Connect to WiFi using config.h credentials
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Udp.begin(OUT_PORT);
    Serial.println("ESP32 Touch OSC");

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
            Udp.beginPacket(OUT_IP, OUT_PORT);
            msg.send(Udp);
            Udp.endPacket();
            msg.empty();
            delay(del);
            touchDetected[touch.index] = false;
        }
    }

    int x_adc = analogRead(x_out), y_adc = analogRead(y_out), z_adc = analogRead(z_out);
    Serial.printf("x = %d\ty = %d\tz = %d\n", x_adc, y_adc, z_adc);

    double x_g = (((x_adc * 3.3) / 4096 - 1.65) / 0.330);
    double y_g = (((y_adc * 3.3) / 4096 - 1.65) / 0.330);
    double z_g = (((z_adc * 3.3) / 4096 - 1.80) / 0.330);

    roll = atan2(y_g, z_g) * 180 / M_PI + 180;
    pitch = atan2(z_g, x_g) * 180 / M_PI + 180;
    yaw = atan2(x_g, y_g) * 180 / M_PI + 180;
    Serial.printf("Roll = %.2f\tPitch = %.2f\tYaw = %.2f\n\n", roll, pitch, yaw);

    // Send accelerometer data separately via OSC
    OSCMessage xMsg("/accel/x");
    xMsg.add(x_g);
    Udp.beginPacket(OUT_IP, OUT_PORT);
    xMsg.send(Udp);
    Udp.endPacket();
    xMsg.empty();

    OSCMessage yMsg("/accel/y");
    yMsg.add(y_g);
    Udp.beginPacket(OUT_IP, OUT_PORT);
    yMsg.send(Udp);
    Udp.endPacket();
    yMsg.empty();

    OSCMessage zMsg("/accel/z");
    zMsg.add(z_g);
    Udp.beginPacket(OUT_IP, OUT_PORT);
    zMsg.send(Udp);
    Udp.endPacket();
    zMsg.empty();

    delay(1000);
}

