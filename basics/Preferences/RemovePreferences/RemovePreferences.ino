#include <Preferences.h>

Preferences preferences;

void setup() {
	preferences.begin("my-app", false); // Open namespace in read/write mode
	preferences.clear(); // Delete all keys in "my-app"
	preferences.end(); // Close the namespace (keeps it intact)
}

void loop() {}
