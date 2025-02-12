#include <WiFi.h>
#include <WebServer.h>

// Access Point credentials
const char* SSID = "ESP32_AP";
const char* PASSWORD = "123456789";

// Web server on port 80
WebServer server(80);

void setup() {
	Serial.begin(115200);

	// Configure ESP32 as Access Point
	WiFi.softAP(SSID, PASSWORD);

	Serial.println("Access Point Started");
	Serial.print("IP Address: ");
	Serial.println(WiFi.softAPIP());

	// Define server routes
	server.on("/", HTTP_GET, handleRoot);
	server.on("/submit", HTTP_POST, handlePost);

	// Start server
	server.begin();
	Serial.println("HTTP server started");
}

void loop() {
	server.handleClient();
}

// Serve HTML form
void handleRoot() {
	String html = 
		"<!DOCTYPE html>"
		"<html>"
		"	<head>"
		"		<title>ESP32 Text Input</title>"
		"		<meta name='viewport' content='width=device-width, initial-scale=1'>"
		"		<style>"
		"			body { font-family: Arial, sans-serif; margin: 20px; }"
		"			form { max-width: 400px; margin: 0 auto; }"
		"			input[type=text] { width: 100%; padding: 12px 20px; margin: 8px 0; }"
		"			input[type=submit] { background-color: #4CAF50; color: white; padding: 14px 20px; "
		"			border: none; cursor: pointer; width: 100%; }"
		"			input[type=submit]:hover { background-color: #45a049; }"
		"		</style>"
		"	</head>"
		"	<body>"
		"		<form action='/submit' method='POST'>"
		"			<h2>Enter Text:</h2>"
		"			<input type='text' name='message' placeholder='Enter your message'>"
		"			<input type='submit' value='Submit'>"
		"		</form>"
		"	</body>"
		"</html>";
	server.send(200, "text/html", html);
}

// Handle POST request
void handlePost() {
	if (server.hasArg("message")) {
		String message = server.arg("message");
		Serial.println("Received message: " + message);

		// Send response back to client
		String response = 
			"<!DOCTYPE html>"
			"<html>"
			"	<head>"
			"		<meta http-equiv='refresh' content='3;url=/'>"
			"		<style>"
			"			body { font-family: Arial, sans-serif; margin: 20px; text-align: center; }"
			"		</style>"
			"	</head>"
			"	<body>"
			"		<h2>Your message:</h2>"
			"		<p>" + message + "</p>"
			"	</body>"
			"</html>";
		server.send(200, "text/html", response);
	} else {
		server.send(400, "text/plain", "No message received");
	}
}
