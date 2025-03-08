#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h> // Include LittleFS library

WebServer server(80);

void setup() {
	Serial.begin(115200);

	// Initialize LittleFS
	if (!LittleFS.begin()) {
		Serial.println("Failed to mount LittleFS");
		return;
	}

	// Set up Access Point
	WiFi.softAP("ESP32_AP", "password123"); // SSID and password for the AP
	Serial.println("Access Point Started");
	Serial.print("IP Address: ");
	Serial.println(WiFi.softAPIP()); // Print the AP IP address

	// Serve HTML file from LittleFS
	server.on("/", HTTP_GET, []() {
			File file = LittleFS.open("/index.html", "r"); // Open file
			if(!file)
			{
				File file = LittleFS.open("/error.html", "r"); // Open file
				server.send(500, "text/plain", "Error loading HTML");
				return;
			}
			server.streamFile(file, "text/html"); // Stream the file to the client
			file.close(); // Close the file
		});

	server.begin(); // Start the server
}

void loop() {
	server.handleClient(); // Handle incoming requests
}
