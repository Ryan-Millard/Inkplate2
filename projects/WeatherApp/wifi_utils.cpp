#include <WiFi.h>
#include <stdint.h>
#include <WebServer.h>
#include <Preferences.h>

#include "wifi_utils.h"
#include "HTML.h"
#include "display_utils.h"

namespace WiFiUtils
{
	const char *AP_SSID{"Weather Station Setup"};
	const char *AP_PASSWORD{""}; // No password

	Preferences prefs;

	std::tuple<const String, const String> getWiFiCredentials()
	{
		prefs.begin("wifi_utils");

		String WIFI_SSID = prefs.getString("wifi_ssid", "");
		String WIFI_PASSWORD = prefs.getString("wifi_password", "");

		if(WIFI_SSID.isEmpty() || WIFI_PASSWORD.isEmpty())
		{
			Serial.println("Server starting...");
			auto credentials = captureWifiCredentials(); // Ensure this returns valid credentials
			WIFI_SSID = std::get<0>(credentials);
			WIFI_PASSWORD = std::get<1>(credentials);

			prefs.putString("wifi_ssid", WIFI_SSID);
			prefs.putString("wifi_password", WIFI_PASSWORD);
		}

		prefs.end();
		return std::make_tuple(std::move(WIFI_SSID), std::move(WIFI_PASSWORD));
	}

	std::tuple<const String, const String> captureWifiCredentials()
	{
		DisplayUtils::displayWiFiConnectionGuide(WiFiUtils::AP_SSID, WiFiUtils::AP_PASSWORD);

		prefs.begin("wifi_utils");

		String wifi_ssid{};
		String wifi_password{};
		WebServer server(80);
		bool serverStopped{false};

		WiFi.disconnect(true); // Force disconnect any existing connections
		WiFi.enableSTA(false); // Explicitly disable STA
		WiFi.mode(WIFI_OFF);   // Ensure WiFi is off before starting AP
		delay(500);

		WiFi.softAP(AP_SSID, AP_PASSWORD);
		Serial.println("Access point started");

		server.on("/", HTTP_GET, [&server]() {
			server.send(200, "text/html", HTML::index);
		});

		server.on("/submit", HTTP_POST, [&]() {
			if(server.hasArg("wifi_ssid") && server.hasArg("wifi_password")) {
				wifi_ssid = server.arg("wifi_ssid");
				wifi_password = server.arg("wifi_password");

				server.send(200, "text/html", HTML::success);
				delay(1000);
				server.stop();
				WiFi.softAPdisconnect(true);
				WiFi.enableAP(false); // Explicitly disable AP
				WiFi.mode(WIFI_OFF);
				delay(500); // Critical: Allow time for cleanup
				serverStopped = true;
			}
			else
			{
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

		prefs.putString("wifi_ssid", wifi_ssid);
		prefs.putString("wifi_password", wifi_password);
		prefs.end();

		// Return as a String tuple
		return std::make_tuple(wifi_ssid, wifi_password);
	}

	bool connectToWiFi(const char* WIFI_SSID, const char* WIFI_PASSWORD)
	{
		Serial.printf("Connecting to WiFi network: %s\n", WIFI_SSID);
		WiFi.persistent(false); // Prevent saving settings to flash
		WiFi.disconnect(true);  // Clear previous configs
		WiFi.mode(WIFI_OFF);    // Ensure WiFi is off before setting mode
		delay(100);

		WiFi.mode(WIFI_STA);    // Set to station mode
		WiFi.disconnect(true);   // Clear previous configurations
		delay(100);

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
