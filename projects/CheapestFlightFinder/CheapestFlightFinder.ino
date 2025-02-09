// Pre-defined
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Inkplate.h>

// User-defined
#include "env.h" // Holds definitions for WIFI_SSID and WIFI_PASSWORD

// The below must be defined in env.h
extern const char* WIFI_SSID; // Name of your WiFi
extern const char* WIFI_PASSWORD;
/*
	For the below API keys, you will need to sign up and create them on the below websites.
	AviationStack: https://aviationstack.com/dashboard
	Amadeus: https://developers.amadeus.com/my-apps

	For Amadeus, both will be displayed to you on your dashboard once you have set everything up.
	amadeusClientId is the one that is not hidden by default
	amadeusClientSecret is the one that is hidden by default
*/
const char* aviationStackApiKey;
const char* amadeusClientId; // Replace with your Amadeus Client ID
const char* amadeusClientSecret; // Replace with your Amadeus Client Secret

/*
	This is a temporary access token that must be fetched by the Inkplate2 before any data can be
	retrieved from the Amadeus API
*/
String amadeusAccessToken;

// Initialize Inkplate2 display
Inkplate display;

void setup()
{
	// Start Serial Monitor for debugging
	Serial.begin(115200);

	// Connect to WiFi
	WiFi.begin(ssid, password);
	Serial.println("Connecting to WiFi");
	while(WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
		Serial.println(".");
	}
	Serial.println("Connected to WiFi!");

	// Initialize Inkplate2 display
	display.begin();
	display.clearDisplay();
	display.setTextColor(INKPLATE2_BLACK);	// Important as the default is INKPLATE2_WHITE, which won't be visible
	display.setTextSize(2);

	// Fetch Amadeus API access token
	amadeusAccessToken = getAmadeusAccessToken();

	// Fetch and display flight status and pricing data
	fetchFlightStatus();
	delay(10000); // 10 second delay to allow user to read it
	fetchFlightPricing(amadeusAccessToken);
	delay(10000); // 10 second delay to allow user to read it
}

void loop()
{
	// Fetch and update every 60 seconds
	delay(40000); // Wait 50 seconds before next fetch
	fetchFlightStatus();
	delay(10000); // 10 second delay to allow user to read it
	fetchFlightPricing(amadeusAccessToken);
	delay(10000); // 10 second delay to allow user to read it
}

// Function to fetch and display flight status from AviationStack API
void fetchFlightStatus()
{
	// Set up HTTPClient for AviationStack
	String url = "http://api.aviationstack.com/v1/flights?access_key=" + String(aviationStackApiKey) + "&dep_iata=JNB&limit=1";
	HTTPClient http;
	http.begin(url);

	int httpCode = http.GET();
	if(httpCode > 0) // The HTTP request was OK
	{
		String payload = http.getString();
		Serial.println(payload); // Debug: print raw response

		// Parse the JSON response for flight status
		DynamicJsonDocument doc(2048);
		deserializeJson(doc, payload);

		// Extract the required data from the response array
		String flightStatus = doc["data"][0]["flight_status"].as<String>();
		String departureTime = doc["data"][0]["departure"]["estimated"].as<String>();
		String arrivalTime = doc["data"][0]["arrival"]["estimated"].as<String>();

		// Display flight status on Inkplate2
		display.clearDisplay();
		display.setCursor(0, 10);
		display.setTextSize(2);
		display.println("Flight Status: ");
		display.setTextSize(1);
		display.println(flightStatus);

		display.setCursor(0, 40);
		display.setTextSize(2);
		display.println("Departure: ");
		display.setTextSize(1);
		display.println(departureTime);

		display.setCursor(0, 70);
		display.setTextSize(2);
		display.println("Arrival: ");
		display.setTextSize(1);
		display.println(arrivalTime);

		display.display(); // Update the screen
	}
	else
	{
		// Bad HTTP response
		Serial.println("Error fetching flight status");
	}

	// Close the HTTP request object
	http.end();
}

// Function to get an Amadeus access token
// Function to get an Amadeus access token
String getAmadeusAccessToken()
{
	// This will be returned
	String accessToken = "";

	// Prepare HTTPClient
	String url = "https://test.api.amadeus.com/v1/security/oauth2/token";
	HTTPClient http;
	http.begin(url);
	http.addHeader("Content-Type", "application/x-www-form-urlencoded");

	// Set up the body with the secret key
	String body = "grant_type=client_credentials&client_id=" + String(amadeusClientId) + 
					"&client_secret=" + String(amadeusClientSecret);

	// Increased timeout
	http.setTimeout(10000);  // 10 seconds

	int httpCode = http.POST(body);
	if(httpCode == 200) // If 200 (OK), it was successful
	{
		// Data that was fetched
		String payload = http.getString();

		// Create a JSON object and check for errors
		DynamicJsonDocument doc(1024);
		DeserializationError error = deserializeJson(doc, payload);
		if(!error && doc.containsKey("access_token")) // Ensure doc contains correct data
		{
			accessToken = doc["access_token"].as<String>();
			Serial.println("Access Token Retrieved Successfully");
		}
		else
		{
			Serial.println("JSON parsing error or missing access token");
		}
	}
	else // Error
	{
		Serial.print("Token Request Failed. HTTP Code: ");
		Serial.println(httpCode);
		String errorResponse = http.getString();
		Serial.println("Error Response: " + errorResponse);
	}

	// Close HTTP object
	http.end();
	return accessToken;
}

// Function to fetch and display flight pricing from Amadeus API
void fetchFlightPricing(String accessToken) {
	if(accessToken == "") // accessToken should be valid at this point
	{
		Serial.println("Failed to get Amadeus access token");
		return;
	}

	// Set up HTTPClient
	String url = "https://test.api.amadeus.com/v2/shopping/flight-offers?originLocationCode=JNB&destinationLocationCode=PRG&departureDate=2025-02-05&adults=1&currencyCode=ZAR&max=1";
	HTTPClient http;

	// Increase timeout
	http.setTimeout(10000);  // 10 seconds timeout

	http.begin(url);
	http.addHeader("Authorization", "Bearer " + accessToken);

	int httpCode = http.GET();
	Serial.print("HTTP Code: ");
	Serial.println(httpCode);
	if(httpCode == 200) // If HTTP request was successful (OK)
	{
		// Get the data we fetched
		String payload = http.getString();
		Serial.println("Full Payload: ");
		Serial.println(payload);

		// Create JSON object and check for errors in the process
		DynamicJsonDocument doc(8192);
		DeserializationError error = deserializeJson(doc, payload);
		if(error)
		{
			Serial.print("JSON deserialization failed: ");
			Serial.println(error.c_str());
			return;
		}

		// Create a JsonArray and check its data
		JsonArray dataArray = doc["data"].as<JsonArray>();
		if(!dataArray.isNull() && dataArray.size() > 0)
		{
			// Only checking the first flight as this program only displays a single flight
			JsonObject firstFlight = dataArray[0];

			// If the data has the required fields
			if(firstFlight["price"] && firstFlight["itineraries"])
			{
				// Set up to display the data to the Inkplate
				float price = firstFlight["price"]["total"].as<float>();
				String departureTime = firstFlight["itineraries"][0]["segments"][0]["departure"]["at"].as<String>();
				String returnTime = firstFlight["itineraries"][0]["segments"][0]["arrival"]["at"].as<String>();

				display.clearDisplay();
				display.setCursor(0, 10);
				display.setTextSize(2);
				display.println("Cheapest Flight: ");
				display.setTextSize(1);
				display.println("ZAR " + String(price));

				display.setCursor(0, 40);
				display.setTextSize(2);
				display.println("Departure: ");
				display.setTextSize(1);
				display.println(departureTime);

				display.setCursor(0, 70);
				display.setTextSize(2);
				display.println("Return: ");
				display.setTextSize(1);
				display.println(returnTime);

				display.display();
			}
			else
			{
				Serial.println("Price or itinerary data missing");
			}
		}
		else
		{
			Serial.println("No flight data found");
		}
	}
	else // HTTP issue: not OK, which means it was an unsuccessful request
	{
		Serial.print("HTTP Error: ");
		Serial.println(httpCode);
		String errorPayload = http.getString();
		Serial.println("Error Response:");
		Serial.println(errorPayload);
	}

	http.end();
}
