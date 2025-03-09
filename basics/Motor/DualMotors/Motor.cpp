#include <Arduino.h>

#include "Motor.h"

namespace HardwareDevices
{
	// Private
	void Motor::initPins()
	{
		// Initialize pins
		pinMode(EN8V_PIN, OUTPUT);
		pinMode(EN5V_PIN, OUTPUT);
		pinMode(DVR_SLEEP_PIN, OUTPUT);
		pinMode(PWM_OUT_PIN, OUTPUT);
		pinMode(DIRECTION_PIN, OUTPUT);
	}

	void Motor::enableDrivers()
	{
		// Enable power and wake up driver
		digitalWrite(EN8V_PIN, HIGH);
		digitalWrite(EN5V_PIN, HIGH);
		digitalWrite(DVR_SLEEP_PIN, HIGH);
	}

	// Public
	void Motor::rotate(short SPEED) {
		SPEED = constrain(SPEED, -255, 255);  // Limit to valid PWM range

		if(SPEED > 0)
		{
			// Forwards
			Serial.println("Forwards");
			digitalWrite(DIRECTION_PIN, LOW);
		}
		else
		{
			// Reverse
			Serial.println("Reverse");
			digitalWrite(DIRECTION_PIN, HIGH);
		}

		analogWrite(PWM_OUT_PIN, abs(SPEED)); // Ensure value is not negative
	}

	void Motor::stop()
	{
		digitalWrite(DIRECTION_PIN, LOW);
		analogWrite(PWM_OUT_PIN, 0);
	}
}
