// Pre-defined
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Inkplate.h>

// User-defined
#include "env.h" // Holds definitions for WIFI_SSID, WIFI_PASSWORD & OPEN_WEATHER_MAP_API_KEY

// The below must be defined in env.h
extern const char* WIFI_SSID; // Name of your WiFi
extern const char* WIFI_PASSWORD;
extern const char* OPEN_WEATHER_MAP_API_KEY; // OpenWeatherMap API key, which can be acquired from https://home.openweathermap.org/api_keys

// NTP Server settings
constexpr char* NTP_SERVER = "pool.ntp.org";
constexpr long GMT_OFFSET_SEC = 7200;  // GMT+2 for Johannesburg
constexpr int DAYLIGHT_OFFSET_SEC = 0;

// Sleep timer settings
constexpr unsigned int SLEEP_TIME = 24 * 60 * 60; // Time to sleep for 24 hours (in seconds)

// Geolocation API endpoint
constexpr char* GEO_LOCATION_URL = "https://ipinfo.io/json";
// City and Country Code fetched using GEO_LOCATION_URL
String city = "";
String countryCode = "";

// Create Inkplate object
Inkplate display;

void setup() {
	Serial.begin(115200);

	// Initialize display with error checking
	Serial.println("Initializing display...");
	if(!display.begin())
	{
		Serial.println("Display initialization failed!");
		delay(5000);
		ESP.restart(); // soft restart the Inkplate
		return;
	}
	Serial.println("Display initialized successfully");

	// Connect to WiFi
	Serial.printf("Connecting to WiFi network: %s\n", WIFI_SSID);

	// Connect to the WiFi and retry for 10 seconds
	// See below for if it fails
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	for(byte connectionAttempts = 0; WiFi.status() != WL_CONNECTED && connectionAttempts < 20; ++connectionAttempts)
	{
		delay(500); // Max time before giving up: 20 * 500 = 10 000ms = 10 seconds
		Serial.print("Connection Attempt: " + String(connectionAttempts));
	}

	// WiFi failed, end app
	if(WiFi.status() != WL_CONNECTED)
	{
		Serial.println("\nWiFi connection failed!");

		display.clearDisplay();
		display.setCursor(10, 10);
		display.println("WiFi Connection Failed!");
		display.display();

		return;
	}

	// WiFi should be connected at this point, so proceed
	Serial.println("\nWiFi connected!");
	Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());

	getLocationData(); // Get location data from API

	configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER); // Initialize time using NTP

	// Update weather immediately
	if(!city.isEmpty() && !countryCode.isEmpty())
	{
		updateWeather();
	}

	
	// Enter deep sleep
	Serial.println("Going to sleep...");
	esp_sleep_enable_timer_wakeup(SLEEP_TIME * 1000000);  // Set the timer to wake up after 24 hours (in microseconds)
	esp_deep_sleep_start(); // Go to deep sleep
}

void loop() {} // Should never execute due to deep sleep

void getLocationData() {
	Serial.println("Getting location data...");

	HTTPClient http;
	http.begin(GEO_LOCATION_URL);

	int httpCode = http.GET();
	Serial.printf("Location API HTTP Code: %d\n", httpCode);

	if (httpCode > 0) {
		String payload = http.getString();
		Serial.println("Location API response: " + payload);

		DynamicJsonDocument doc(2048);
		DeserializationError error = deserializeJson(doc, payload);

		if (!error) {
			if (doc.containsKey("city")) {
				city = doc["city"].as<String>();
				Serial.println("City: " + city);
			}
			if (doc.containsKey("country")) {
				countryCode = doc["country"].as<String>();
				Serial.println("Country: " + countryCode);
			}

			// Show location on display
			display.clearDisplay();
			display.setCursor(10, 10);
			display.println("Location found:");
			display.setCursor(10, 40);
			display.println(city + ", " + countryCode);
			display.display();
			delay(2000);
		} else {
			Serial.println("JSON parsing failed: " + String(error.c_str()));
		}
	}
	http.end();
}

void updateWeather() {
	if (city.isEmpty() || countryCode.isEmpty()) {
		Serial.println("Location data missing");
		return;
	}

	Serial.println("Updating weather information...");
	HTTPClient http;
	String url = "https://api.openweathermap.org/data/2.5/weather?q=" + 
		city + "," + countryCode + "&APPID=" + OPEN_WEATHER_MAP_API_KEY + "&units=metric";

	http.begin(url);
	int httpCode = http.GET();
	Serial.printf("Weather API HTTP Code: %d\n", httpCode);

	if (httpCode > 0) {
		String payload = http.getString();
		Serial.println("Weather API response: " + payload);

		DynamicJsonDocument doc(2048);
		DeserializationError error = deserializeJson(doc, payload);

		if (!error) {
			float temp = doc["main"]["temp"];
			float humidity = doc["main"]["humidity"];
			const char* description = doc["weather"][0]["main"];
			float windSpeed = doc["wind"]["speed"];

			Serial.printf("Temperature: %.1f°C\n", temp);
			Serial.printf("Humidity: %.0f%%\n", humidity);
			Serial.printf("Description: %s\n", description);
			Serial.printf("Wind Speed: %.1f m/s\n", windSpeed);

			displayWeather(temp, humidity, description, windSpeed);
		} else {
			Serial.println("Weather JSON parsing failed: " + String(error.c_str()));
		}
	} else {
		Serial.println("Weather API request failed");
	}
	http.end();
}

void displayWeather(float temp, float humidity, const char* description, float windSpeed) {
	Serial.println("Updating display with weather information...");

	display.clearDisplay();
	display.drawRect(0, 0, display.width(), display.height(), INKPLATE2_BLACK);

	display.setCursor(5, 5);
	display.setTextColor(INKPLATE2_BLACK); // Using black only in 1-bit mode
	display.print(temp, 1);
	display.print("°C");

	display.setCursor(5, 15);
	display.print("Humidity: ");
	display.print(humidity, 0);
	display.print("%");

	display.setCursor(5, 25);
	display.print("Wind: ");
	display.print(windSpeed, 1);
	display.print(" m/s");

	if (description != nullptr) {
		display.setCursor(5, 35);
		display.print(description);
	}

	struct tm timeinfo;
	if (getLocalTime(&timeinfo)) {
		char timeString[9];
		strftime(timeString, 9, "%H:%M:%S", &timeinfo);
		display.setCursor(2, 85);
		display.print("Updated at: " + String(timeString));
	}

	display.display();
}
