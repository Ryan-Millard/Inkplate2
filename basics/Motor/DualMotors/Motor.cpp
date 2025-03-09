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
	void Motor::rotate(short SPEED) const
	{
		SPEED = constrain(SPEED, -255, 255);  // Limit to valid PWM range

		// Determine if the rotation is "forward" based on inversion state and direction of speed (sign)
		const bool isForwardRotation{
			(SPEED > 0 && !isInverted) // If not inverted, speed is positive for forward direction
				||
				(SPEED < 0 && isInverted) // If inverted, speed is negative for forward direction
		};
		digitalWrite(DIRECTION_PIN, isForwardRotation ? LOW : HIGH); // LOW if forward rotation, HIGH for reverse

		analogWrite(PWM_OUT_PIN, abs(SPEED)); // Ensure value is not negative
	}

	void Motor::stop() const
	{
		digitalWrite(DIRECTION_PIN, LOW);
		analogWrite(PWM_OUT_PIN, 0);
	}
}
