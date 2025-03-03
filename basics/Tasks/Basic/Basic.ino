void taskFunction(void *pvParam) {
	while(1) {
		Serial.println("Task running...");
		vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
	}
}

void setup() {
	Serial.begin(115200);
	// Create a new Task
	xTaskCreate(
		taskFunction,   // Task function
		"Task Name",    // Name (for debugging)
		10000,          // Stack size (bytes)
		NULL,           // Parameters
		1,              // Priority (0 to configMAX_PRIORITIES-1)
		NULL            // Task handle (optional)
	);
}

void loop() {} // Empty if using tasks
