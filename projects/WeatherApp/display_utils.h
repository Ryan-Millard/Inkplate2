#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include <Inkplate.h>
#include <ArduinoJson.h>

namespace DisplayUtils
{
	extern Inkplate display;

	enum WeatherChar : char
	{
		CELSIUS = 42,
		CLEAR = 66,
		CLOUDS = 78,
		RAIN = 88,
		DRIZZLE = 82,
		THUNDERSTORM = 90,
		SNOW = 87,
		MIST = 77,
		UNKNOWN = 64
	};

	char getWeatherChar(const char* description);
	void initializeDisplay();
	void displayWiFiConnectionGuide(const char* AP_SSID, const char* AP_PASSWORD);
	void displayMessage(const String& heading, const String& text = "");
	void displayWeather(const DynamicJsonDocument& doc);
}

#endif
