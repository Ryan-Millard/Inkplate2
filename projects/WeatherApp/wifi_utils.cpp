#include <WiFi.h>
#include <stdint.h>

#include "wifi_utils.h"
#include "env.h"

namespace WiFiUtils
{
	bool connectToWiFi()
	{
		Serial.printf("Connecting to WiFi network: %s\n", env::WIFI_SSID);
		WiFi.begin(env::WIFI_SSID, env::WIFI_PASSWORD);

		for(uint8_t connectionAttempts{0}; WiFi.status() != WL_CONNECTED && connectionAttempts < 20; ++connectionAttempts)
		{
			delay(500);
			Serial.println("Connection Attempt: " + String(connectionAttempts));
		}

		if(WiFi.status() != WL_CONNECTED)
		{
			Serial.println("\nWiFi connection failed!");
			return false;
		}

		Serial.println("\nWiFi connected!");
		Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
		return true;
	}
}
