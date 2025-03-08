#include <LittleFS.h>

void setup()
{
	Serial.begin(115200);
	LittleFS.begin();
	File file = LittleFS.open("/test.txt", "r");
	if(file)
	{
		String content = file.readString();
		while(1){
			Serial.println(content);
			Serial.println("Hello");
			file.close();
		}
	}
	else
	{
		while(1)
		{
			Serial.println("Not found.");
		}
	}
}

void loop() {}
