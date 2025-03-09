#include "Motor.h"

// Define pins
#define EN8V_PIN 16
#define EN5V_PIN 17
#define DVR_SLEEP_PIN 12
#define PWM_OUT_PIN 32  // PWM output pin
#define DIRECTION_PIN 33  // Direction control pin

void setup() {
	Serial.begin(115200);
	Serial.println("Motor test starting...");

	HardwareDevices::Motor motor{PWM_OUT_PIN, DIRECTION_PIN, EN8V_PIN, EN5V_PIN, DVR_SLEEP_PIN};

	motor.rotate(128); // Forwards
	delay(1000);
	motor.stop();
	delay(500);

	motor.rotate(-128); // Reverse
	delay(1000);
	motor.stop();
	delay(500);
}

void loop() {
	// Empty loop
}
