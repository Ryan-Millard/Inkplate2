#include <WiFi.h>
#include <stdint.h>
#include <WebServer.h>

#include "wifi_utils.h"
#include "HTML.h"

namespace WiFiUtils
{
	const char *AP_SSID{"ESP32-AccessPoint"};
	const char *AP_PASSWORD{"123456789"}; // Make it null if you don't want a password

	std::tuple<const char*, const char*> captureWifiCredentials() {
		String wifi_ssid{};
		String wifi_password{};
		WebServer server(80);
		bool serverStopped{false};

		WiFi.softAP(AP_SSID, AP_PASSWORD);
		Serial.println("Access point started");

		server.on("/", HTTP_GET, [&server]() {
			server.send(200, "text/html", HTML::index);
		});

		server.on("/submit", HTTP_POST, [&]() {
			if(server.hasArg("wifi_ssid") && server.hasArg("wifi_password")) {
			wifi_ssid = server.arg("wifi_ssid");
			wifi_password = server.arg("wifi_password");

			Serial.print("WiFi SSID: ");
			Serial.println(wifi_ssid);
			Serial.print("Password: ");
			Serial.println(wifi_password);

			server.send(200, "text/html", HTML::success);
			delay(1000);
			server.stop();
			WiFi.softAPdisconnect(true);
			serverStopped = true;
			} else {
			server.send(400, "text/html", "Invalid Input");
			}
		});

		server.begin();
		while (!serverStopped) {
			server.handleClient();
			delay(10);
		}

		Serial.println("WiFi SSID inside captureWifiCredentials: " + wifi_ssid);
		Serial.println("WiFi Password inside captureWifiCredentials: " + wifi_password);

		// Return as a tuple of const char* pointers
		return std::make_tuple(wifi_ssid.c_str(), wifi_password.c_str());
	}

	bool connectToWiFi(const char* WIFI_SSID, const char* WIFI_PASSWORD)
	{
		Serial.printf("Connecting to WiFi network: %s\n", WIFI_SSID);
		WiFi.mode(WIFI_STA); // Ensure ESP32 is in station mode
		WiFi.disconnect(true); // Clears previous Wi-Fi credentials
		WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

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
