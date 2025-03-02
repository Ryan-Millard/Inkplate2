#ifndef WEATHER_UTILS_H
#define WEATHER_UTILS_H

#include <ArduinoJson.h>

namespace WeatherUtils
{
	bool fetchWeatherData(const String& city, const String& countryCode, DynamicJsonDocument& doc);
}

#endif
