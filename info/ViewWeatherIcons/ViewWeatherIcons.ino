#include <Inkplate.h>
#include "WeatherIcon.h"

Inkplate display;

void setup() {
	display.begin();                // Initialize the display
	display.clearDisplay();         // Clear the screen
	display.setTextColor(INKPLATE2_BLACK); // Set text color
	display.setFont(&WeatherIcon);  // Set custom weather icon font
	display.setTextSize(1);         // Set font size to 1 for readability
}

void loop() {
	// Font metrics configuration
	const int startASCII = 32;
	const int endASCII = 90;
	const int cols = 8;             // Number of columns
	const int xSpacing = 50;        // Horizontal spacing between icons (larger to fit numbers)
	const int ySpacing = 20;        // Vertical spacing between rows
	const int startX = 20;          // Initial X position
	const int startY = 20;          // Initial Y position

	int x = startX;
	int y = startY;

	// Show first half of characters with their numbers
	for (char c = startASCII; c <= (startASCII + endASCII) / 2; c++) {
		// Set cursor position and draw character
		display.setCursor(x, y);
		display.write(c);  // Write the character (weather icon)

		// Move to next column for the number
		x += xSpacing;

		// Display the number (character code) next to the icon
		display.setCursor(x, y);
		display.setFont(nullptr); // Use default font to print the number
		display.print((int)c);  // Print the integer value (ASCII code)
		display.setFont(&WeatherIcon);  // Set custom weather icon font again

		// Move to next column for the next character and number
		x += xSpacing;

		// Move to next row when column limit reached
		if ((c - startASCII) % cols == (cols - 1)) {
			x = startX;
			y += ySpacing;
		}
	}

	// Update only the part of the display that changed
	display.partialUpdate(true, false);
	delay(10000);  // Wait for 10 seconds

	// Clear screen for second half
	display.clearDisplay();

	// Show second half of characters with their numbers
	x = startX;
	y = startY;
	for (int c = (startASCII + endASCII) / 2 + 1; c <= endASCII; c++) {
		// Set cursor position and draw character
		display.setCursor(x, y);
		display.write(c);  // Write the character (weather icon)

		// Move to next column for the number
		x += xSpacing;

		// Display the number (character code) next to the icon
		display.setCursor(x, y);
		display.setFont(nullptr); // Use default font to print the number
		display.print((int)c);  // Print the integer value (ASCII code)
		display.setFont(&WeatherIcon);  // Set custom weather icon font again

		// Move to next column for the next character and number
		x += xSpacing;

		// Move to next row when column limit reached
		if ((c - startASCII) % cols == (cols - 1)) {
			x = startX;
			y += ySpacing;
		}
	}

	// Update only the part of the display that changed
	display.partialUpdate(true, false);
	delay(10000);  // Wait for 10 seconds
}
