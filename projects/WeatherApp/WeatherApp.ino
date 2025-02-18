#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Inkplate.h>

#include "env.h"
#include "wifi_utils.h"
#include "display_utils.h"
#include "weather_utils.h"
#include "location_utils.h"

// NTP Server settings
constexpr char* NTP_SERVER{"pool.ntp.org"};
constexpr long GMT_OFFSET_SEC{7200};  // GMT+2 for Johannesburg
constexpr int DAYLIGHT_OFFSET_SEC{0};

// Sleep timer settings
constexpr unsigned int SLEEP_TIME{3 * 60 * 60}; // Time to sleep for 3 hours (in seconds)

void setup()
{
	Serial.begin(115200);

	DisplayUtils::initializeDisplay();

	if(!WiFiUtils::connectToWiFi())
	{
		DisplayUtils::display.clearDisplay();
		DisplayUtils::display.setCursor(10, 10);
		DisplayUtils::display.println("WiFi Connection Failed!");
		DisplayUtils::display.display();
		return;
	}

	Serial.println("Fetching location...");
	String city, countryCode;
	if(LocationUtils::fetchLocationData(city, countryCode))
	{
		Serial.println("City: " + city);
		Serial.println("Country Code: " + countryCode);
		Serial.println("Displaying location...");
		DisplayUtils::displayLocation(city, countryCode);
	}

	Serial.println("Configuring Time...");
	configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

	Serial.println("Fetching Weather Data...");
	DynamicJsonDocument weatherDoc(1024);
	if(WeatherUtils::fetchWeatherData(city, countryCode, weatherDoc))
	{
		Serial.println("Displaying Weather Data...");
		DisplayUtils::displayWeather(weatherDoc);
	}

	Serial.println("Going to sleep...");
	esp_sleep_enable_timer_wakeup(SLEEP_TIME * 1000000);
	esp_deep_sleep_start();
}

void loop() {}
