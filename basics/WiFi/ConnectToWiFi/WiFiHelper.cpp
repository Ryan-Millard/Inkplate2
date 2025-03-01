#include "WiFiHelper.h"
#include <time.h>

WiFiHelper::WiFiHelper(const char* ssid, const char* password)
	: _ssid(ssid),
	_password(password),
	_lastDisplayUpdate(0),
	_connectionAttempts(0),
	_timeInitialized(false)
{
}

bool WiFiHelper::connect()
{
	if(isConnected())
	{
		_connectionAttempts = 0;
		return true;
	}

	if(_connectionAttempts >= MAX_RETRY_ATTEMPTS)
	{
		handleConnectionError();
		return false;
	}

	WiFi.mode(WIFI_STA);
	WiFi.begin(_ssid, _password);

	const unsigned long startAttemptTime = millis();
	while(!isConnected() && (millis() - startAttemptTime <= WIFI_TIMEOUT_MS))
	{
		delay(500);
	}

	if(isConnected())
	{
		return true;
	}

	_connectionAttempts++;
	return false;
}

void WiFiHelper::handleConnectionError()
{
	WiFi.disconnect();
	delay(1000);
	_connectionAttempts = 0;
}

bool WiFiHelper::setTime()
{
	if(!isConnected()) { return false; }
	if(!_timeInitialized) { return initializeTime(); }
	return true;
}

bool WiFiHelper::initializeTime()
{
	configTime(0, 0, "pool.ntp.org", "time.nist.gov");

	struct tm timeinfo;
	unsigned long startAttempt = millis();
	while(!getLocalTime(&timeinfo) && (millis() - startAttempt < 5000))
	{
		delay(100);
	}

	if(getLocalTime(&timeinfo))
	{
		_timeInitialized = true;
		return true;
	}

	return false;
}

String WiFiHelper::getTimeString() const
{
	struct tm timeinfo;
	if(!getLocalTime(&timeinfo)) { return "Time sync failed"; }

	char timeString[50];
	strftime(timeString, sizeof(timeString), "%H:%M:%S", &timeinfo);

	return String(timeString);
}

String WiFiHelper::getLocalIP() const
{
	return isConnected() ? WiFi.localIP().toString() : "Not Connected";
}

bool WiFiHelper::isConnected() const
{
	return WiFi.status() == WL_CONNECTED;
}

String WiFiHelper::getRSSI() const
{
	return isConnected() ? String(WiFi.RSSI()) + " dBm" : "N/A";
}

void WiFiHelper::updateDisplay(Inkplate& display)
{
	unsigned long currentMillis = millis();
	if(currentMillis - _lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL_MS)
	{
		display.clearDisplay();
		// Display connection status
		display.setTextSize(1);
		display.setCursor(10, 10);
		display.println("WiFi: " + String(isConnected() ? "Connected" : "Disconnected"));

		// Display time in larger text
		display.setTextSize(2);
		display.setCursor(10, 30);
		display.print("Time: ");
		display.println(getTimeString());

		// Display network info in smaller text
		display.setTextSize(1);
		display.setCursor(10, 60);
		display.println("Signal: " + getRSSI());
		display.setCursor(10, 90);
		display.println("IP: " + getLocalIP());
		display.display();
		_lastDisplayUpdate = currentMillis;
	}
}
