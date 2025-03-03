// ESP32s have only 2 cores, core 0 and core 1

void printFunction(const char* &message)
{
	while(1)
	{
		Serial.println(message);
		vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
	}
}

void core0Function(void *pvParam)
{
	const char* message{"Hello \0"};	// Null-terminated string
	printFunction(message);
}

void core1Function(void *pvParam)
{
	const char* message{"World!\n\0"};	// Null-terminated string
	printFunction(message);
}

void setup()
{
	Serial.begin(115200);

	// Pin a Task to Core 0 (First core)
	xTaskCreatePinnedToCore(
		core0Function,	// Task function
		"Core 0 Task",	// Name (for debugging)
		10000,			// Stack size (bytes)
		NULL,			// Parameters
		1,				// Priority (0 to configMAX_PRIORITIES-1)
		NULL,			// Task handle (optional)
		0				// Core ID (0 or 1)
	);

	// Pin a Task to Core 1 (First core)
	xTaskCreatePinnedToCore(
		core1Function,	// Task function
		"Core 1 Task",	// Name (for debugging)
		10000,			// Stack size (bytes)
		NULL,			// Parameters
		1,				// Priority (0 to configMAX_PRIORITIES-1)
		NULL,			// Task handle (optional)
		1				// Core ID (0 or 1)
	);
}

void loop() {} // Empty if using tasks
