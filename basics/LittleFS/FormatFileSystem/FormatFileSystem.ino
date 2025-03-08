#include <LittleFS.h>

void setup()
{
	Serial.begin();
	delay(10000);

	Serial.println("Formatting LittleFS...");

	if(LittleFS.format())
	{
		Serial.println("Success");
	}
	else
	{
		Serial.println("Failure");
	}
}

void loop() {}
