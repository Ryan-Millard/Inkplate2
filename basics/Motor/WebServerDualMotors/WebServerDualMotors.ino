#include "Motor.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

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

// HTML page
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Motor Control</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .slider-container { margin: 20px 0; width: 100%; }
        input[type="range"] { width: 100%; }
		#motorA { margin-bottom: 60vh; }
    </style>
</head>
<body>
    <h1>Motor Control</h1>
    <div class="slider-container">
        <label for="motorA">Motor A:</label>
        <input type="range" id="motorA" min="-255" max="255" value="0"
               oninput="updateMotor('A', this.value)"
               onmouseup="this.value=0; updateMotor('A', 0)"
               ontouchend="this.value=0; updateMotor('A', 0)">
        <span id="motorAValue">0</span>
    </div>
    <div class="slider-container">
        <label for="motorB">Motor B:</label>
        <input type="range" id="motorB" min="-255" max="255" value="0"
               oninput="updateMotor('B', this.value)"
               onmouseup="this.value=0; updateMotor('B', 0)"
               ontouchend="this.value=0; updateMotor('B', 0)">
        <span id="motorBValue">0</span>
    </div>
    <script>
        var ws = new WebSocket('ws://' + window.location.hostname + ':81/');

        function updateMotor(motor, value) {
            document.getElementById('motor' + motor + 'Value').textContent = value;
            if (ws.readyState === WebSocket.OPEN) {
                ws.send(motor + ',' + value);
            }
        }
    </script>
</body>
</html>
)rawliteral";

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
	switch(type)
	{
		case WStype_TEXT:
			{
				String message = (char*)payload;
				int commaPos = message.indexOf(',');
				if (commaPos == -1) return;

				String motor = message.substring(0, commaPos);
				int speed = message.substring(commaPos + 1).toInt();

				Serial.print("Speed: ");
				Serial.println(speed);
				if (motor == "A") {
					motorA.rotate(speed);
				} else if (motor == "B") {
					motorB.rotate(speed);
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

	// Initialize WiFi AP
	WiFi.softAP(ssid, password);
	IPAddress IP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(IP);

	// Initialize web server
	server.on("/", []() {
			server.send(200, "text/html", htmlPage);
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
