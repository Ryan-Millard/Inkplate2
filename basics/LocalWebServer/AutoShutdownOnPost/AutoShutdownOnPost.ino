#include <WiFi.h>
#include <WebServer.h>
#include "HTML.h"

// Access point credentials
const char *ssid = "ESP32-AccessPoint";
const char *password = "123456789";

// WebServer object on port 80
WebServer server(80);

// Variables to store the submitted username and password
String username = "";
String userPassword = "";

// Flag to check if the server is stopped
bool serverStopped = false;

// Handles GET request to root ("/")
void handleRoot() {
    server.send(200, "text/html", HTML::index);
}

// Handles POST request to "/submit"
void handleSubmit() {
    if (server.hasArg("username") && server.hasArg("password")) {
        username = server.arg("username");
        userPassword = server.arg("password");

        Serial.println("Username: " + username);
        Serial.println("Password: " + userPassword);

        server.send(200, "text/html", HTML::success);

        delay(1000);
        server.stop();
        delay(200);
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_OFF);
        serverStopped = true; // Set flag to stop handling clients
    } else {
        server.send(400, "text/html", "Invalid Input");
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.softAP(ssid, password);
    Serial.println("Access point started");

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    server.on("/", HTTP_GET, handleRoot);
    server.on("/submit", HTTP_POST, handleSubmit);

    server.begin();
    Serial.println("Web server started");
}

void loop() {
    if (!serverStopped) {
        server.handleClient();
    }
}
