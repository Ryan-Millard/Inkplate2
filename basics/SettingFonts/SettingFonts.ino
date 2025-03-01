#include <Inkplate.h>

// Custom font file or file from Fonts folder in your InkplateLibrary installation
// If you want to use an Inkplate font, you will need to copy the file to the lib/Fonts directory
#include "lib/Fonts/Tiny3x3a2pt7b.h" // 2.66px (Quite small)
#include "lib/Fonts/FreeMono12pt7b.h" // 15.96px
#include "lib/Fonts/Org_01.h"
#include "lib/Fonts/WeatherIcon.h"

Inkplate display;  // Initialize the Inkplate 2 display

void setup() {
	display.begin(); // Initialize the display
	display.clearDisplay(); // Clear the screen before drawing
	display.setTextColor(INKPLATE2_BLACK); // Set text color (black on white for e-paper displays)

	display.setCursor(5, 5);
	display.setFont(&Tiny3x3a2pt7b); // GFXfont that is created in lib/Fonts/Tiny3x3a2pt7b.h
	display.println("Tiny3x3a2pt7b");	// Notice that this is a very small font
										// The "2pt" in the name shows that it will be a smaller font
										// One px is 0.75 pt, or a pt is 1.33 px

	display.setCursor(5, 35);
	display.setFont(&FreeMono12pt7b); // GFXfont that is created in lib/Fonts/FreeMono12pt7b.h
	display.println("FreeMono12pt7b");

	display.setCursor(5, 50);
	display.setFont(&Org_01); // GFXfont that is created in lib/Fonts/Org_01.h
	display.println("Org_01");

	display.setCursor(5, 80);
	display.setFont(&WeatherIcon); // GFXfont that is created in lib/Fonts/Org_01.h
	display.println("WeatherIcon");

	// Update the display with the new content
	display.display();

	// Program is done and will never need to update again
	esp_deep_sleep_start();  // Enter deep sleep with no wake-up
}

// Nothing to do in the loop for this simple test
void loop() {}
