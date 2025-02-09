// Pre-defined
#include <Inkplate.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// User-defined
#include "env.h" // Holds definitions for WIFI_SSID and WIFI_PASSWORD

// The below must be defined in env.h
extern const char* WIFI_SSID; // Name of your WiFi
extern const char* WIFI_PASSWORD;

const char* apiURL = "http://ip-api.com/json/";  // Free, no API key required

Inkplate display;

void setup() {
	Serial.begin(115200);
	display.begin();
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(INKPLATE2_BLACK);
	display.setCursor(0, 0);

	Serial.println("Attempting to connect to WiFi");
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	unsigned long startTime = millis();
	while (WiFi.status() != WL_CONNECTED && millis() - startTime < 60000) {  // Timeout after 10 seconds
		Serial.print(".");
		delay(500);
	}

	if (WiFi.status() != WL_CONNECTED) {
		Serial.println("\nFailed to connect to WiFi.");
		display.println("WiFi connection failed");
		display.display();
		return;  // Exit setup if WiFi fails
	}

	Serial.println("\nConnected to WiFi");
	Serial.print("IP Address: ");
	Serial.println(WiFi.localIP());

	HTTPClient http;
	http.begin(apiURL);
	int httpCode = http.GET();

	if (httpCode != HTTP_CODE_OK) {
		Serial.println("HTTP request failed");
		display.println("HTTP request failed");
		display.display();
		http.end();
		return;
	}

	Serial.println("HTTP request OK");
	String payload = http.getString();
	StaticJsonDocument<512> doc;
	DeserializationError error = deserializeJson(doc, payload);

	if (error) {
		Serial.print("JSON parsing failed: ");
		Serial.println(error.c_str());
		display.println("JSON parsing failed");
		display.display();
		http.end();
		return;
	}

	display.clearDisplay();
	display.print("City: ");
	display.println(doc["city"].as<String>());
	display.print("Latitude: ");
	display.println(doc["lat"].as<float>(), 6); // Display latitude with 6 decimal places
	display.print("Longitude: ");
	display.println(doc["lon"].as<float>(), 6); // Display longitude with 6 decimal places
	display.display();

	Serial.print("City: ");
	Serial.println(doc["city"].as<String>());
	Serial.print("Latitude: ");
	Serial.println(doc["lat"].as<float>(), 6);
	Serial.print("Longitude: ");
	Serial.println(doc["lon"].as<float>(), 6);

	http.end();
}

void loop() {}

