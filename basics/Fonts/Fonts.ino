#include <Inkplate.h>
#include "OpenSansLightFont.h"  // Include your custom font file

const char ALL_SUPPORTED_CHARS[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

Inkplate display;  // Initialize the Inkplate 2 display

void setup() {
    // Initialize the display
    display.begin();
    display.setRotation(1);  // Set the display rotation (optional)

    // Clear the screen before drawing
    display.clearDisplay();

    // Set the custom font
    display.setFont(&Open_Sans_Light_11);  // Use the exact font variable name

    // Set text color (black on white for e-paper displays)
    display.setTextColor(BLACK, WHITE);

    // Set the cursor position for the text
    display.setCursor(10, 30);  // Position of the text
	display.setTextSize(2);
	display.print("Open Sans Font");
	// Draw a black horizontal line at (10, 20) with width 100
	display.drawFastHLine(10, 35, 192, BLACK);

    // Print "Temp:" followed by a temperature value and the degree symbol
    display.setCursor(0, 50);  // Position of the text
	display.setTextSize(1);
    display.print(ALL_SUPPORTED_CHARS);

    // Update the display with the new content
    display.display();
}

void loop() {
    // Nothing to do in the loop for this simple test
}
