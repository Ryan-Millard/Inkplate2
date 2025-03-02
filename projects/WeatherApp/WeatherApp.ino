#include <WiFi.h>
#include <HTTPClient.h>
#include <Inkplate.h>

#include "wifi_utils.h"
#include "display_utils.h"
#include "weather_utils.h"
#include "location_utils.h"

// NTP Server settings
constexpr char* NTP_SERVER{"pool.ntp.org"};
constexpr long GMT_OFFSET_SEC{7200};  // GMT+2 for Johannesburg
constexpr int DAYLIGHT_OFFSET_SEC{0}; // No daylight saving in Johannesburg

// Sleep timer settings
constexpr unsigned int SLEEP_TIME{3 * 60 * 60}; // Time to sleep for 3 hours (in seconds)

void setup()
{
	Serial.begin(115200);

	DisplayUtils::initializeDisplay();

	DisplayUtils::displayWiFiConnectionGuide(WiFiUtils::AP_SSID, WiFiUtils::AP_PASSWORD);

	Serial.println("Server starting...");
	auto [WIFI_SSID, WIFI_PASSWORD] = WiFiUtils::captureWifiCredentials();

	if(!WiFiUtils::connectToWiFi(WIFI_SSID.c_str(), WIFI_PASSWORD.c_str()))
	{
		DisplayUtils::displayMessage("WiFi Connection Failed!");
		return;
	}

	Serial.println("Fetching location...");
	String city, countryCode;
	if(LocationUtils::fetchLocationData(city, countryCode))
	{
		Serial.println("City: " + city);
		Serial.println("Country Code: " + countryCode);
		Serial.println("Displaying location...");
		DisplayUtils::displayMessage("Location found:", city + ", " + countryCode);
	}

	Serial.println("Configuring Time...");
	configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

	Serial.println("Fetching Weather Data...");
	DynamicJsonDocument weatherDoc(1024);
	for(byte i = 0; !WeatherUtils::fetchWeatherData(city, countryCode, weatherDoc) && i < 20; i++)
	{
		Serial.println("fetchWeatherData attempt: " + String(i));
	}

	Serial.println("Displaying Weather Data...");
	DisplayUtils::displayWeather(weatherDoc);

	Serial.println("Going to sleep for " + String(SLEEP_TIME / 3600) + " hours...");
	esp_sleep_enable_timer_wakeup(SLEEP_TIME * 1000000);
	esp_deep_sleep_start();
}

void loop() {}
