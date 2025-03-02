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

template<typename Func>
bool retryOperation(Func operation, const String& initialMessage, const String& retryMessage, const String& finalFailureMessage, int maxAttempts = 20, int delayMs = 500)
{
	const int lastAttempt{maxAttempts - 1};

	for(int i{0}; i < maxAttempts; ++i)
	{
		Serial.println("Attempt " + String(i + 1) + " of " + String(maxAttempts));

		if(operation())
		{
			return true; // Success
		}

		// Handle failure messages
		if(i == 0)
		{
			DisplayUtils::displayMessage(initialMessage, retryMessage);
		}
		else if(i == lastAttempt)
		{
			DisplayUtils::displayMessage(finalFailureMessage, "");
		}

		const int maxDelayMs{10000}; // Maximum delay of 10 seconds
		delay(min(delayMs * (1 << i), maxDelayMs); // Exponential backoff capped at 10 seconds
	}
	return false; // All attempts failed
}

void setup()
{
	Serial.begin(115200);

	DisplayUtils::initializeDisplay();

	DisplayUtils::displayWiFiConnectionGuide(WiFiUtils::AP_SSID, WiFiUtils::AP_PASSWORD);

	Serial.println("Server starting...");
	auto [WIFI_SSID, WIFI_PASSWORD] = WiFiUtils::captureWifiCredentials();

	const bool wifiConnected{retryOperation(
		[&]() { return WiFiUtils::connectToWiFi(WIFI_SSID.c_str(), WIFI_PASSWORD.c_str()); },
		"Failed to connect to WiFi",
		"Trying again, please stand by.",
		"Failed to connect to WiFi\nPlease try again later.",
		20
	)};
	if(!wifiConnected)
	{
		return;
	}

	Serial.println("Fetching location...");
	String city, countryCode;
	const bool locationFetched{retryOperation(
		[&]() { return LocationUtils::fetchLocationData(city, countryCode); },
		"Failed to fetch\nlocation.",
		"Trying again, please stand by.",
		"Failed to fetch\nlocation.\nPlease try again later.",
		20
	)};
	if(!locationFetched)
	{
		// Handle location fetch failure
		return;
	}
	Serial.println("City: " + city);
	Serial.println("Country Code: " + countryCode);
	Serial.println("Displaying location...");
	DisplayUtils::displayMessage("Location found:", city + ", " + countryCode);

	Serial.println("Configuring Time...");
	configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

	Serial.println("Fetching Weather Data...");
	DynamicJsonDocument weatherDoc(1024);
	const bool weatherFetched{retryOperation(
		[&]() { return WeatherUtils::fetchWeatherData(city, countryCode, weatherDoc); },
		"Failed to fetch\nweather forecasts",
		"Trying again, please stand by.",
		"Failed to fetch\nweather forecasts\nPlease try again later.",
		20
	)};
	if(!weatherFetched) {
		// Handle weather fetch failure
		return;
	}
	Serial.println("Displaying Weather Data...");
	DisplayUtils::displayWeather(weatherDoc);

	Serial.println("Going to sleep for " + String(SLEEP_TIME / 3600) + " hours...");
	esp_sleep_enable_timer_wakeup(SLEEP_TIME * 1000000);
	esp_deep_sleep_start();
}

void loop() {}
