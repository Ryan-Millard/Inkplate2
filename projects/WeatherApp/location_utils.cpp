#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "location_utils.h"

namespace LocationUtils
{
	bool fetchLocationData(String& city, String& countryCode)
	{
		HTTPClient http;
		http.begin("https://ipinfo.io/json");

		int httpCode{http.GET()};
		if(httpCode > 0)
		{
			String payload{http.getString()};
			DynamicJsonDocument doc(2048);
			DeserializationError error{deserializeJson(doc, payload)};

			if(!error)
			{
				if(doc.containsKey("city"))
				{
					city = doc["city"].as<String>();
				}
				if(doc.containsKey("country"))
				{
					countryCode = doc["country"].as<String>();
				}
				http.end();
				return true;
			}
		}

		http.end();
		return false;
	}
}
