#ifndef WIFIHELPER_H
#define WIFIHELPER_H

#include <WiFi.h>
#include <Inkplate.h>

class WiFiHelper
{
	public:
		static constexpr unsigned long WIFI_TIMEOUT_MS = 10000;  // 10 seconds timeout
		static constexpr unsigned long DISPLAY_UPDATE_INTERVAL_MS = 1000; // 1 second refresh
		static constexpr int MAX_RETRY_ATTEMPTS = 3; // Only retry 3 times

		WiFiHelper(const char* ssid, const char* password);

		bool connect();
		bool setTime();
		String getLocalIP() const;
		bool isConnected() const;
		String getRSSI() const;
		void updateDisplay(Inkplate& display);
	private:
		const char* _ssid;
		const char* _password;
		unsigned long _lastDisplayUpdate;
		int _connectionAttempts;
		bool _timeInitialized;

		void handleConnectionError();
		bool initializeTime();
		String getTimeString() const;
};

#endif
