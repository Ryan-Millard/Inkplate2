#include "WiFi.h"

void setup()
{
	Serial.begin(115200);

	// Set ESP32 to Wi-Fi Station mode and disconnect from any AP
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(100); // Short delay to ensure mode change

	Serial.println("\nScanning nearby networks...");

	// Perform a synchronous network scan
	int networkCount = WiFi.scanNetworks();

	if(networkCount == 0)
	{
		Serial.println("No networks found.");
		return;
	}

	Serial.printf("%d networks found:\n", networkCount);
	for(int i = 0; i < networkCount; i++)
	{
		// Print network details
		Serial.printf("%2d: %-32s (RSSI: %4d) %s\n", i+1, WiFi.SSID(i).c_str(), WiFi.RSSI(i),
							(WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Secured");
	}
}

// Empty loop (scan runs once at startup)
void loop() {}
