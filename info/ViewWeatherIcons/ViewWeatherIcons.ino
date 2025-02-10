#include <Inkplate.h>
#include "WeatherIcon.h"

Inkplate display;

void setup() {
	display.begin();                // Initialize the display
	display.clearDisplay();         // Clear the screen
	display.setTextColor(INKPLATE2_BLACK); // Set text color
	display.setFont(&WeatherIcon);  // Set custom weather icon font
	display.setTextSize(0.15);

	// Font metrics configuration
	const int startASCII = 32;      // First character (space)
	const int endASCII = 90;        // Last character (Z)
	const int cols = 10;             // Number of columns
	const int xSpacing = 20;        // Horizontal spacing between icons
	const int ySpacing = 20;        // Vertical spacing between rows
	const int startX = 0;          // Initial X position
	const int startY = 0;          // Initial Y position

	int x = startX;
	int y = startY;

	// Only iterate through defined characters (32-90)
	for (int c = startASCII; c <= endASCII; c++) {
		// Set cursor position and draw character
		display.setCursor(x, y);
		display.write(c);  // Write the character

		// Move to next column
		x += xSpacing;

		// Move to next row when column limit reached
		if ((c - startASCII) % cols == (cols - 1)) {
			x = startX;
			y += ySpacing;
		}
	}

	display.display();  // Send content to the screen
}

void loop() {
	// Empty loop
}
