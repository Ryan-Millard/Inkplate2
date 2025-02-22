#include <stdint.h>
#include <cmath>
#include "display_utils.h"
#include "lib/Fonts/WeatherIcon.h"

namespace DisplayUtils
{
	Inkplate display;

	char getWeatherChar(const char* description)
	{
		String descriptionUpper{String(description)}; // Convert to String
		descriptionUpper.toUpperCase(); // Convert to uppercase

		if (descriptionUpper == "CLEAR") { return WeatherChar::CLEAR; }
		else if (descriptionUpper == "CLOUDS") { return WeatherChar::CLOUDS; }
		else if (descriptionUpper == "RAIN") { return WeatherChar::RAIN; }
		else if (descriptionUpper == "DRIZZLE") { return WeatherChar::DRIZZLE; }
		else if (descriptionUpper == "THUNDERSTORM") { return WeatherChar::THUNDERSTORM; }
		else if (descriptionUpper == "SNOW") { return WeatherChar::SNOW; }
		else if (descriptionUpper == "MIST") { return WeatherChar::MIST; }
		else { return WeatherChar::UNKNOWN; }
	}

	void initializeDisplay()
	{
		Serial.println("Initializing display...");
		if(!display.begin())
		{
			Serial.println("Display initialization failed!");
			delay(5000);
			ESP.restart();
		}

		Serial.println("Display initialized successfully");
		display.setTextColor(INKPLATE2_BLACK);
		display.clearDisplay();
		display.display();
	}

	void displayLocation(const String& city, const String& countryCode)
	{
		display.clearDisplay();
		display.setCursor(10, 10);
		display.println("Location found:");
		display.setCursor(10, 40);
		display.println(city + ", " + countryCode);
		display.display();
		delay(2000);
	}

	void displayWeather(const DynamicJsonDocument& doc)
	{
		float temp{doc["list"][0]["main"]["temp"]};
		float humidity{doc["list"][0]["main"]["humidity"]};
		const char* description{doc["list"][0]["weather"][0]["main"]};
		float windSpeed{doc["list"][0]["wind"]["speed"]};

		display.clearDisplay();
		display.drawRect(0, 0, display.width(), display.height(), INKPLATE2_BLACK);

		// Heading
		// Last time updated
		struct tm timeinfo;
		if(getLocalTime(&timeinfo))
		{
			// Current Date
			display.setCursor(12, 5);
			char dateString[20];
			strftime(dateString, sizeof(dateString), "%d %B %Y", &timeinfo);
			display.print(String(dateString));

			// Current time
			char timeString[9];
			strftime(timeString, 9, "%H:%M", &timeinfo);
			display.setCursor(135, 5);
			display.print("Time: " + String(timeString));
		}
		display.drawFastHLine(10, 15, 192, INKPLATE2_BLACK);

		// Weather Image Description
		if(description != nullptr)
		{
			display.setCursor(5, 76);
			display.setTextSize(3);
			display.setFont(&WeatherIcon);
			display.write(getWeatherChar(description));
			display.setFont(nullptr);
		}

		// Temperature
		display.setTextSize(3);
		display.setCursor(105, 20);
		display.print(temp, 1);
		display.setCursor(160, 56);
		display.setFont(&WeatherIcon);
		display.print((char)WeatherChar::CELSIUS);
		display.setFont(nullptr);

		// Humidity
		display.setTextSize(1.2);
		display.setCursor(5, 90);
		display.print(humidity, 0);
		display.print("%");

		// Wind Speed
		display.setCursor(30, 90);
		display.print(windSpeed, 1);
		display.print("m/s");
		display.setTextSize(1); // Reset text size

		// Forecasts for next 3 days
		display.fillRect(75, 45, 128, 55, INKPLATE2_BLACK);
		const uint8_t gap{2};
		const uint8_t innerRectWidth{40};
		const uint8_t innerRectHeight{51};
		uint8_t xPos{77};
		const uint8_t yPos{47};
		const uint8_t numForecastsToDisplay{3};
		const uint8_t numDailyForecasts{8}; // 24 Hours with a forecast every 3 hours -> 24 / 3 = 8

		for(uint8_t count = 0; count < numForecastsToDisplay; count++)
		{
			uint8_t i{numDailyForecasts * (count + 1)}; // 8, 16, 24 for 3 days

			const char* dt_txt = doc["list"][0]["dt_txt"];
			const char day[3] = {dt_txt[8], dt_txt[9], '\0'}; // "22"
			const char month[3] = {dt_txt[5], dt_txt[6], '\0'}; // "02"
			char formatted_date[6]; // DD/MM + null terminator
			sprintf(formatted_date, "%s/%s", day, month);

			const float temp{doc["list"][i]["main"]["temp"]};
			const char* description{doc["list"][i]["weather"][0]["main"]};

			int xCursor{xPos + gap};
			int yCursor{yPos + gap};

			display.fillRect(xPos, yPos, innerRectWidth, innerRectHeight, INKPLATE2_WHITE);

			// Date
			display.setCursor(xCursor + 3, yCursor);
			display.setTextSize(1);
			display.println(formatted_date);

			// Weather Image description
			yCursor += 27;
			display.setCursor(xCursor + 7, yCursor);
			display.setFont(&WeatherIcon);
			display.write(getWeatherChar(description));
			display.setFont(nullptr); // Reset the font

			// Temperature
			yCursor += 10;
			display.setCursor(xCursor + 1, yCursor);
			display.println(temp, 1);
			display.setCursor(xCursor + 20, yCursor + 7);
			display.setFont(&WeatherIcon);
			display.print((char)WeatherChar::CELSIUS); // Use weather font for celsius symbol
			display.setFont(nullptr); // Reset the font

			xPos += gap;
			xPos += innerRectWidth;
		}

		display.display();
	}
}
