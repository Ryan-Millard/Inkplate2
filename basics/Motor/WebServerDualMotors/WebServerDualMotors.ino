#include "Motor.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <LittleFS.h>

// Define pins
#define EN8V_PIN 16
#define EN5V_PIN 17
#define DVR_SLEEP_PIN 12

// MotorA pins
#define MOTOR_A_PWM_OUT_PIN 32
#define MOTOR_A_DIRECTION_PIN 33

// MotorB pins
#define MOTOR_B_PWM_OUT_PIN 25
#define MOTOR_B_DIRECTION_PIN 26

// Network credentials
const char* ssid = "MotorAP";
const char* password = "";

// Web server and WebSocket
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Motor objects
HardwareDevices::Motor motorA(MOTOR_A_PWM_OUT_PIN, MOTOR_A_DIRECTION_PIN, EN8V_PIN, EN5V_PIN, DVR_SLEEP_PIN, true);
HardwareDevices::Motor motorB(MOTOR_B_PWM_OUT_PIN, MOTOR_B_DIRECTION_PIN, EN8V_PIN, EN5V_PIN, DVR_SLEEP_PIN);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
	switch(type) {
		case WStype_TEXT:
			{
				String message = (char*)payload;
				int commaPos = message.indexOf(',');
				if (commaPos == -1) return;

				String part1 = message.substring(0, commaPos);
				String part2 = message.substring(commaPos + 1);

				// Check if it's a motor command (A or B)
				if (part1 == "A" || part1 == "B") {
					int speed = part2.toInt();
					if (part1 == "A") {
						motorA.rotate(speed);
					} else {
						motorB.rotate(speed);
					}
				}
				// Otherwise, process as joystick data (x,y)
				else {
					float x = part1.toFloat();
					float y = part2.toFloat();

					// Calculate motor speeds for differential drive
					int leftSpeed = (y + x) * 255;
					int rightSpeed = (y - x) * 255;

					// Constrain to valid PWM range (-255 to 255)
					leftSpeed = constrain(leftSpeed, -255, 255);
					rightSpeed = constrain(rightSpeed, -255, 255);

					// Apply speeds to motors
					motorA.rotate(leftSpeed);
					motorB.rotate(rightSpeed);
				}
			}
			break;
		default:
			break;
	}
}

void setup()
{
	Serial.begin(115200);

	if (!LittleFS.begin()) {
		Serial.println("Failed to mount LittleFS");
		return;
	}

	// Initialize WiFi AP
	WiFi.softAP(ssid, password);
	IPAddress IP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(IP);

	// Initialize web server
	server.on("/", HTTP_GET, []() {
		File file = LittleFS.open("/index.html", "r"); // Open file
		if(!file)
		{
			server.send(500, "text/plain", "Error loading HTML");
			return;
		}
		server.streamFile(file, "text/html"); // Stream the file to the client
		file.close(); // Close the file
	});
	server.on("/joystick", HTTP_GET, []() {
		File file = LittleFS.open("/joystick.html", "r"); // Open file
		if(!file)
		{
			server.send(500, "text/plain", "Error loading HTML");
			return;
		}
		server.streamFile(file, "text/html"); // Stream the file to the client
		file.close(); // Close the file
	});
	server.begin();

	// Initialize WebSocket
	webSocket.begin();
	webSocket.onEvent(webSocketEvent);
}

void loop()
{
	webSocket.loop();
	server.handleClient();
}
