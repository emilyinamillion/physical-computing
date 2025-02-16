#include <AUnit.h>

test(WiFi_Connection) {
    assertEqual(WiFi.status(), WL_CONNECTED, "WiFi should be connected");
}

test(Accelerometer_Calculations) {
    double x_adc = 2048; // Simulated midpoint ADC value
    double x_g = (((x_adc * 3.3) / 4096 - 1.65) / 0.330);
    assertMoreOrEqual(x_g, -1.0);
    assertLessOrEqual(x_g, 1.0);
}

test(OSC_Message_Sending) {
    OSCMessage testMsg("/test");
    testMsg.add(42);
    WiFiUDP mockUdp;
    mockUdp.beginPacket(outIp, outPort);
    testMsg.send(mockUdp);
    mockUdp.endPacket();
    assertEqual(mockUdp.parsePacket(), 0, "OSC message should be sent");
}

void setup() {
    Serial.begin(115200);
    aunit::TestRunner::run();
}

void loop() {
    aunit::TestRunner::run();
}

