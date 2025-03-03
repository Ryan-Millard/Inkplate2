// Mutexes protect shared resources, which allows tasks (threads) to safely share data

// Mutex handle and shared resource
SemaphoreHandle_t mutex;
int shared_counter = 0;

// Task 1 function
void task1(void *params) {
	while(1)
	{
		// Wait indefinitely for mutex
		if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE)
		{
			// Critical section start
			int current = shared_counter;
			current++;
			shared_counter = current;
			Serial.println("Task1: counter = " + String(current));
			// Critical section end

			xSemaphoreGive(mutex); // Release mutex
		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

// Task 2 function
void task2(void *params) {
	while(1)
	{
		// Wait indefinitely for mutex
		if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE)
		{
			// Critical section start
			int current = shared_counter;
			current++;
			shared_counter = current;
			Serial.println("Task2: counter = " + String(current));
			// Critical section end

			xSemaphoreGive(mutex); // Release mutex
		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void setup() {
	Serial.begin(115200);

	// Create mutex
	mutex = xSemaphoreCreateMutex();
	if(mutex == NULL)
	{
		Serial.println("Failed to create mutex\n");
		return;
	}

	// Create tasks
	xTaskCreate(task1, "Task 1", 2048, NULL, 1, NULL);
	xTaskCreate(task2, "Task 2", 2048, NULL, 1, NULL);

	// Let the tasks run - delete main task
	vTaskDelete(NULL);
}

void loop() {}
