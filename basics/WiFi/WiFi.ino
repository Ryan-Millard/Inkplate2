// Pre-defined
#include <Inkplate.h>

// User-defined
#include "WiFiHelper.h"
#include "env.h" // Holds definitions for WIFI_SSID and WIFI_PASSWORD

// The below must be defined in env.h
extern const char* WIFI_SSID; // Name of your WiFi
extern const char* WIFI_PASSWORD;

constexpr unsigned short UPDATE_INTERVAL = 60000; // 60 seconds in milliseconds

Inkplate display;
WiFiHelper wifiHelper(WIFI_SSID, WIFI_PASSWORD);

void setup()
{
	Serial.begin(115200);

	// Initialize display
	display.begin();
	display.clearDisplay();
	display.setTextColor(INKPLATE2_BLACK);

	// Attempt WiFi connection
	Serial.println("Connecting to WiFi...");
	if(!wifiHelper.connect())
	{
		Serial.println("WiFi connection failed");
		return;
	}
	Serial.println("WiFi connected successfully");
	wifiHelper.setTime();
}

void loop()
{
	static unsigned long lastUpdate = 0;

	// Check if it's time for an update
	if(millis() - lastUpdate >= UPDATE_INTERVAL)
	{
		if(!wifiHelper.isConnected())
		{
			Serial.println("WiFi disconnected, attempting reconnection...");
			wifiHelper.connect();
		}

		wifiHelper.updateDisplay(display);
		lastUpdate = millis();
	}
}
