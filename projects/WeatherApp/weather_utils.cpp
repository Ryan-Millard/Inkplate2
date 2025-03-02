#include <HTTPClient.h>

#include "weather_utils.h"
#include "env.h"

namespace WeatherUtils
{
	bool fetchWeatherData(const String& city, const String& countryCode, DynamicJsonDocument& doc)
	{
		HTTPClient http{};
		const String url{"https://api.openweathermap.org/data/2.5/forecast?q=" + city + "," + countryCode + "&APPID=" + env::OPEN_WEATHER_MAP_API_KEY + "&units=metric"};
		http.begin(url);

		int httpCode{http.GET()};
		Serial.println("HTTP Code: " + String(httpCode));
		if(httpCode <= 0)
		{
			http.end();
			return false;
		}

		String payload{http.getString()};
		DeserializationError error{deserializeJson(doc, payload)};
		if(error) {
			Serial.println("Error during HTTP payload deserialization.");
		}
		http.end();
		return !error;
	}
}
