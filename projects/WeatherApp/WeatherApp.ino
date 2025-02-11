// Pre-defined
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Inkplate.h>

// User-defined
#include "env.h" // Holds definitions for WIFI_SSID, WIFI_PASSWORD & OPEN_WEATHER_MAP_API_KEY
#include "lib/Fonts/WeatherIcon.h"

// The below must be defined in env.h
extern const char* WIFI_SSID; // Name of your WiFi
extern const char* WIFI_PASSWORD;
extern const char* OPEN_WEATHER_MAP_API_KEY; // OpenWeatherMap API key, which can be acquired from https://home.openweathermap.org/api_keys

// NTP Server settings
constexpr char* NTP_SERVER = "pool.ntp.org";
constexpr long GMT_OFFSET_SEC = 7200;  // GMT+2 for Johannesburg
constexpr int DAYLIGHT_OFFSET_SEC = 0;

// Sleep timer settings
constexpr unsigned int SLEEP_TIME = 3 * 60 * 60; // Time to sleep for 3 hours (in seconds)

// Geolocation API endpoint
constexpr char* GEO_LOCATION_URL = "https://ipinfo.io/json";
// City and Country Code fetched using GEO_LOCATION_URL
String city = "";
String countryCode = "";

// Create Inkplate object
Inkplate display;

enum WeatherChar : char {
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
// Helper function for WeatherChar
char getWeatherChar(const char* description) {
	String descriptionUpper = String(description); // Convert to String
	descriptionUpper.toUpperCase(); // Convert to uppercase

	if(descriptionUpper == "CLEAR") { return WeatherChar::CLEAR; }
	else if(descriptionUpper == "CLOUDS") { return WeatherChar::CLOUDS; }
	else if(descriptionUpper == "RAIN") { return WeatherChar::RAIN; }
	else if(descriptionUpper == "DRIZZLE") { return WeatherChar::DRIZZLE; }
	else if(descriptionUpper == "THUNDERSTORM") { return WeatherChar::THUNDERSTORM; }
	else if(descriptionUpper == "SNOW") { return WeatherChar::SNOW; }
	else if(descriptionUpper == "MIST") { return WeatherChar::MIST; }
	else { return WeatherChar::UNKNOWN; }
}

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
		Serial.println("Connection Attempt: " + String(connectionAttempts));
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
	esp_sleep_enable_timer_wakeup(SLEEP_TIME * 1000000);  // Set the timer to wake up after 3 hours (in microseconds)
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

	Serial.println("Fetching weather information...");
	HTTPClient http;
	String url = "https://api.openweathermap.org/data/2.5/forecast?q=" + 
		city + "," + countryCode + "&APPID=" + OPEN_WEATHER_MAP_API_KEY + "&units=metric";

	http.begin(url);
	int httpCode = http.GET();
	Serial.printf("Weather API HTTP Code: %d\n", httpCode);

	if (httpCode > 0) {
		String payload = http.getString();
		Serial.println("Payload size: " + String(payload.length()));
		Serial.println("Weather API response: " + payload);

		DynamicJsonDocument doc(1024);
		DeserializationError error = deserializeJson(doc, payload);

		if (!error) {
			if (doc["list"].is<JsonArray>() && doc["list"].size() > 0 && doc["list"][0]["main"].is<JsonObject>()) {
				displayWeather(doc);
			}
		} else {
			Serial.println("Weather JSON parsing failed: " + String(error.c_str()));
		}
	} else {
		Serial.println("Weather API request failed");
	}
	http.end();
}

void displayWeather(DynamicJsonDocument doc) {
	float temp = doc["list"][0]["main"]["temp"];
	float humidity = doc["list"][0]["main"]["humidity"];
	const char* description = doc["list"][0]["weather"][0]["main"];
	float windSpeed = doc["list"][0]["wind"]["speed"];

	Serial.printf("Temperature: %.1f°C\n", temp);
	Serial.printf("Humidity: %.0f%%\n", humidity);
	Serial.printf("Description: %s\n", description);
	Serial.printf("Wind Speed: %.1f m/s\n", windSpeed);

	Serial.println("Updating display with weather information...");

	display.clearDisplay();
	display.drawRect(0, 0, display.width(), display.height(), INKPLATE2_BLACK);

	// Heading
	display.setCursor(12, 5);
	display.setTextColor(INKPLATE2_BLACK);
	display.print("Weather Today");
	// Last time updated
	struct tm timeinfo;
	if (getLocalTime(&timeinfo)) {
		char timeString[9];
		strftime(timeString, 9, "%H:%M", &timeinfo);
		display.setCursor(135, 5);
		display.print("Time: " + String(timeString));
	}
	display.drawFastHLine(10, 15, 192, INKPLATE2_BLACK);

	// Weather Image Description
	if (description != nullptr) {
		display.setCursor(5, 76);
		display.setTextSize(3);
		display.setFont(&WeatherIcon); // Change to weather icon font
		display.write( getWeatherChar(description) );
		display.setFont(nullptr); // Return to normal font (use nullptr instead of null)
	}

	// Temperature
	display.setTextSize(3);
	display.setCursor(105, 20);
	display.print(temp, 1);
	display.setCursor(160, 56);
	display.setFont(&WeatherIcon); // Change to weather icon font
	display.print((char)WeatherChar::CELSIUS);
	display.setFont(nullptr); // Return to normal font (use nullptr instead of null)

	// Humidity
	display.setTextSize(1);
	display.setCursor(5, 89);
	display.print("Humidity: ");
	display.print(humidity, 0);
	display.print("%");

	// Wind Speed
	display.setCursor(90, 89);
	display.print("Wind Speed: ");
	display.print(windSpeed, 1);
	display.print(" m/s");

	// Next forecasts for the rest of the day (in 3-hour intervals)
	display.fillRect(75, 45, 128, 40, INKPLATE2_BLACK);
	const int gap = 2;
	const int innerRectWidth = 40;
	const int innerRectHeight = 36;
	int xPos = 77;
	const int yPos = 47;
	const int numForecasts = 3; // Number of forecast blocks
								// Start at index 1 to get the next forecast (not the one for right now)
	for (int i = 1; i <= numForecasts; i++) {
		float temp = doc["list"][i]["main"]["temp"];
		const char* description = doc["list"][i]["weather"][0]["main"];
		int xCursor = xPos + gap;
		int yCursor = yPos + gap;

		display.fillRect(xPos, yPos, innerRectWidth, innerRectHeight, INKPLATE2_WHITE);

		// Temperature
		display.setCursor(xCursor, yCursor);
		display.println(temp, 1);
		display.setCursor(xCursor + 20, yCursor + 7);
		display.setFont(&WeatherIcon); // Change to weather icon font
		display.print((char)WeatherChar::CELSIUS);

		// Weather Image description
		yCursor += 30;
		display.setCursor(xCursor, yCursor);
		display.write( getWeatherChar(description) );

		// Interval
		xCursor += 20;
		display.setCursor(xCursor, yCursor);
		display.setFont(nullptr); // Return to normal font (use nullptr instead of null)
		display.println("+" + String(i*3) + "h");

		xPos += gap;
		xPos += innerRectWidth;
	}

	display.display();
}
