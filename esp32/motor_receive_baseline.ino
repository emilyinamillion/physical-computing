const int motorPin1 = 5;  // Motor direction pin 1
const int motorPin2 = 18; // Motor direction pin 2
const int pwmPin = 19;    // PWM speed control pin

void setup() {
    Serial.begin(115200);
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(pwmPin, OUTPUT);
}

void loop() {
    // Move motor forward at 50% speed
    Serial.println("Motor moving forward...");
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    analogWrite(pwmPin, 128);  // 50% PWM speed
    delay(2000);

    // Stop motor
    Serial.println("Motor stopping...");
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    analogWrite(pwmPin, 0);
    delay(1000);

    // Move motor backward at 75% speed
    Serial.println("Motor moving backward...");
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    analogWrite(pwmPin, 192);  // 75% PWM speed
    delay(2000);

    // Stop motor again
    Serial.println("Motor stopping...");
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    analogWrite(pwmPin, 0);
    delay(1000);
}

