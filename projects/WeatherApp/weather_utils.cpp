#include <HTTPClient.h>

#include "weather_utils.h"
#include "env.h"

namespace WeatherUtils
{
	bool fetchWeatherData(const String& city, const String& countryCode, DynamicJsonDocument& doc)
	{
		HTTPClient http;
		String url{"https://api.openweathermap.org/data/2.5/forecast?q=" + city + ","
				+ countryCode + "&APPID=" + env::OPEN_WEATHER_MAP_API_KEY + "&units=metric"};
		http.begin(url);

		int httpCode{http.GET()};
		if(httpCode > 0)
		{
			String payload{http.getString()};
			DeserializationError error{deserializeJson(doc, payload)};
			http.end();
			return !error;
		}

		http.end();
		return false;
	}
}
