#ifndef MOTOR_H
#define MOTOR_H

namespace HardwareDevices
{
	class Motor
	{
		private:
			const byte EN8V_PIN;
			const byte EN5V_PIN;
			const byte DVR_SLEEP_PIN;

			const byte PWM_OUT_PIN;
			const byte DIRECTION_PIN;

			void initPins();
			void enableDrivers();

		public:
			Motor(const byte PWM_OUT_PIN, const byte DIRECTION_PIN, const byte EN8V_PIN, const byte EN5V_PIN, const byte DVR_SLEEP_PIN)
				: PWM_OUT_PIN(PWM_OUT_PIN), DIRECTION_PIN(DIRECTION_PIN), EN8V_PIN(EN8V_PIN), EN5V_PIN(EN5V_PIN), DVR_SLEEP_PIN(DVR_SLEEP_PIN)
			{
				initPins();
				enableDrivers();
			}
			void rotate(short SPEED);
			void stop();
	};
}

#endif
