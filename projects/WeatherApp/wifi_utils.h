#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include <WebServer.h>
#include <tuple>

namespace WiFiUtils
{
	// Access Point credentials
	extern const char *AP_SSID; // Name of AP
	extern const char *AP_PASSWORD; // Password to connect to it (set to null if no password is wanted)

	std::tuple<const String, const String> getWiFiCredentials();
	std::tuple<const String, const String> captureWifiCredentials();
	void handleSubmit();
	void handleRoot();

	bool connectToWiFi(const char* WIFI_SSID, const char* WIFI_PASSWORD);
}

#endif
