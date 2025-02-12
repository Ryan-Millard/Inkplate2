#include <Inkplate.h>
#include <WiFi.h>

// Define your Access Point credentials
const char *SSID = "ESP32_AP";
const char *PASSWORD = "123456789";

// Set custom IP, Gateway, and Subnet mask
// Now the site can be accessed from 10.0.0.1 instead
IPAddress local_ip(10, 0, 0, 1);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);

Inkplate display;

void setup() {
	display.begin();
	display.clearDisplay();
	display.println("Setting things up...");
	display.display();

	Serial.begin(115200);

	// Set up the ESP32 as an Access Point with the custom IP
	WiFi.softAPConfig(local_ip, gateway, subnet);
	WiFi.softAP(SSID, PASSWORD);

	String ipAddress = WiFi.softAPIP();
	String accessPointInfo = "Access Point \"" + SSID + "\" started";
	String ipAddressInfo = "IP address: " + ipAddress;
	String howToConnectInfo = "Find \"" + SSID + "\" in the list of available networks under WiFi and type \""
				+ PASSWORD + "\" as your password to connect to it. Next, open your browser and type \""
				+ ipAddress + "\" in the search bar to open the website.";

	Serial.println(accessPointInfo);
	Serial.println(ipAddressInfo);
	Serial.println(howToConnectInfo);

	display.clearDisplay();
	display.println("Basic ESP32 Server");
	display.println(accessPointInfo);
	display.println(ipAddressInfo);
	display.println(howToConnectInfo);
	display.display();

	server.begin();
}

void loop() {
	WiFiClient client = server.available();
	if (client)
	{
		String request = client.readStringUntil('\r');

		Serial.println("New Client connected");
		Serial.println(request);

		client.flush();

		// Extract the path from the request
		int pathStart = request.indexOf("GET ") + 4;  // Skip "GET "
		int pathEnd = request.indexOf(" HTTP");       // Find end of path
		String path = request.substring(pathStart, pathEnd);


		String html = "<!DOCTYPE html><html><body>"; // start of HTML doc
		display.clearDisplay();
		display.setTextSize(2);
		// Improved routing logic
		if(path == "/" || path == "")
		{
			// Serve the index page
			html = "<h1>Welcome to the ESP32!</h1>";
			html += "<p>This is the home page served by the ESP32.</p>";
			html += "<a href=\"/about\">Go to About Page</a><br>";
			html += "<a href=\"/contact\">Go to Contact Page</a><br>";

			display.println("Home Page");
		}
		else if(path == "/about")
		{
			// Serve the about page
			html = "<h1>About the ESP32</h1>";
			html += "<p>This is an About page served by the ESP32.</p>";
			html += "<a href=\"/\">Back to Home Page</a><br>";
			html += "<a href=\"/contact\">Go to Contact Page</a><br>";

			display.println("About Page");
		}
		else if(path == "/contact")
		{
			// Serve the contact page
			html = "<h1>Contact Us</h1>";
			html += "<p>This is a Contact page served by the ESP32.</p>";
			html += "<a href=\"/\">Back to Home Page</a><br>";
			html += "<a href=\"/about\">Go to About Page</a><br>";

			display.println("Contact Page");
		}
		else
		{
			// Handle 404 Not Found
			html = "<h1>404 Not Found</h1>";
			html += "<p>The requested page was not found.</p>";
			html += "<a href=\"/\">Back to Home Page</a><br>";

			display.println("404 Not Found Page");
		}
		html += "</body></html>"; // end of HTML doc
		display.setTextSize(1);
		display.println(html);
		display.display();

		// Send the HTML page to the client
		client.println("HTTP/1.1 200 OK");
		client.println("Content-Type: text/html");
		client.println("Connection: close");
		client.println();
		client.println(html);

		client.stop();
		Serial.println("Client disconnected");
	}
}
