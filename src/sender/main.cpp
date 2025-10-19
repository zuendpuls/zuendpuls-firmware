#include <Arduino.h>

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  digitalWrite(13, !digitalRead(13));
  delay(500);
}
