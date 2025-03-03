//Queues allow data to be sent between threads

QueueHandle_t queue = xQueueCreate(5, sizeof(int)); // Queue size=5, item=int

// Producer task
void producerTask(void *pvParam) {
	int data = 0;
	while(1)
	{
		xQueueSend(queue, &data, portMAX_DELAY);
		data++;
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}

// Consumer task
void consumerTask(void *pvParam) {
	int received;
	while(1)
	{
		if(xQueueReceive(queue, &received, portMAX_DELAY))
		{
			Serial.println(received);
		}
	}
}

void setup() {
	Serial.begin(115200);

	// Create tasks
	xTaskCreate(producerTask, "Producer", 2048, NULL, 1, NULL);
	xTaskCreate(consumerTask, "Consumer", 2048, NULL, 1, NULL);

	// Let the tasks run - delete main task
	vTaskDelete(NULL);
}

void loop() {}
