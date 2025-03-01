#include <Preferences.h>

Preferences preferences;

void setup() {
	Serial.begin(115200);

	delay(30000);

	// Open the preferences with a namespace
	preferences.begin("my-app", false);

	// Read a value (default to 0 if not found)
	int counter = preferences.getInt("counter", 0);

	// Print the value
	Serial.println("Old counter value : " + String(counter));

	// Increment the counter
	counter++;

	// Save the updated value
	preferences.putInt("counter", counter);

	// Print the value
	Serial.println("New counter value : " + String(counter));

	// Close the preferences
	preferences.end();
}

void loop() {
	// Your main code here
}
