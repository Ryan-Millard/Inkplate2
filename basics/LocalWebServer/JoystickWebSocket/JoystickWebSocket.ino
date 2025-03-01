#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ESPAsyncWebServer.h>  // Include the web server library

#include "HTML.h"

const char *ssid = "ESP32-Joystick";  // SSID for the AP
const char *password = "12345678";    // Password for the AP (optional)

WebSocketsServer webSocket = WebSocketsServer(81); // WebSocket server on port 81
AsyncWebServer server(80);  // HTTP server on port 80

// Setup the WiFi AP and WebSocket server
void setup() {
	Serial.begin(115200);

	// Set up the ESP32 as an Access Point
	WiFi.softAP(ssid, password);
	Serial.println("ESP32 is now an Access Point");

	// Start WebSocket server
	webSocket.begin();
	webSocket.onEvent(webSocketEvent);

	// Serve the HTML content when the client accesses the root ("/")
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send_P(200, "text/html", HTML::index);  // Send the HTML content
	});

	// Start HTTP server
	server.begin();

	Serial.print("IP Address: ");
	Serial.println(WiFi.softAPIP());
}

// WebSocket event handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
	String message;

	switch (type) {
		case WStype_TEXT:
			// Handle joystick data here
			message = String((char *)payload);
			Serial.println("Received message: " + message);
			break;
		case WStype_CONNECTED:
			Serial.println("New client connected");
			break;
		case WStype_DISCONNECTED:
			Serial.println("Client disconnected");
			break;
		default:
			break;
	}
}

void loop() {
	// Listen for WebSocket events
	webSocket.loop();
}
