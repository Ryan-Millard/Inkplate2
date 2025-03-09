#include "Motor.h"

// Define pins
#define EN8V_PIN 16
#define EN5V_PIN 17
#define DVR_SLEEP_PIN 12

// MotorA pins
#define MOTOR_A_PWM_OUT_PIN 32 // PWM output pin
#define MOTOR_A_DIRECTION_PIN 33 // Direction control pin

// MotorB pins
#define MOTOR_B_PWM_OUT_PIN 25 // PWM output pin
#define MOTOR_B_DIRECTION_PIN 26 // Direction control pin

void setup() {
	Serial.begin(115200);
	Serial.println("Motor test starting...");

	HardwareDevices::Motor motorA{MOTOR_A_PWM_OUT_PIN, MOTOR_A_DIRECTION_PIN, EN8V_PIN, EN5V_PIN, DVR_SLEEP_PIN, true /* Inverted */};
	HardwareDevices::Motor motorB{MOTOR_B_PWM_OUT_PIN, MOTOR_B_DIRECTION_PIN, EN8V_PIN, EN5V_PIN, DVR_SLEEP_PIN};

	motorA.rotate(128); // Forwards
	motorB.rotate(128); // Forwards
	delay(1000);
	motorA.stop();
	motorB.stop();
	delay(500);

	motorA.rotate(-128); // Reverse
	motorB.rotate(-128); // Reverse
	delay(1000);
	motorA.stop();
	motorB.stop();
	delay(500);
}

void loop() {
	// Empty loop
}
