SemaphoreHandle_t interruptSem;
volatile bool printEnabled = true; // Controls the print loop

// Timer ISR (triggers every 5 seconds)
void IRAM_ATTR timerISR()
{
	xSemaphoreGiveFromISR(interruptSem, NULL);
}

// Interrupt handling task
void interruptTask(void *pvParam)
{
	while(1)
	{
		if(xSemaphoreTake(interruptSem, portMAX_DELAY))
		{
			printEnabled = !printEnabled; // Toggle print state
			Serial.println(printEnabled ? "Resuming prints" : "Pausing prints");
		}
	}
}

// Main print task
void printTask(void *pvParam)
{
	while(1)
	{
		if(printEnabled)
		{
			Serial.println("Printing...");
		}
		vTaskDelay(pdMS_TO_TICKS(500)); // Print every 500ms
	}
}

void setup()
{
	Serial.begin(115200);
	interruptSem = xSemaphoreCreateBinary();

	// Configure timer interrupt (ESP32-specific)
	hw_timer_t *timer = timerBegin(0, 80, true); // Timer 0, prescaler 80 (1MHz clock)
	timerAttachInterrupt(timer, &timerISR, true);
	timerAlarmWrite(timer, 5000000, true); // 5-second interval (5,000,000 μs)
	timerAlarmEnable(timer);

	// Create tasks
	xTaskCreate(interruptTask, "ISR Task", 2048, NULL, 2, NULL);
	xTaskCreate(printTask, "Print Task", 2048, NULL, 1, NULL);
}

void loop() {
	// Empty - FreeRTOS handles tasks
}
