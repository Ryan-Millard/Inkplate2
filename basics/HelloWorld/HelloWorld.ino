// The below checks the board definition, if the wrong one is selected, it displays an error
// The code should function without the following 3 lines
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h"

Inkplate display; 	// Initialize a global display object

// setup() is the Arduino version of the main() method as main() is abstracted
void setup()
{
	// Only call this function once per program it initializes the Inkplate
	display.begin();
	/* Clear any pre-existing data in the buffer */
	display.clearDisplay();

	// Set the cursor to be at point (x=10, y=10)
	display.setCursor(10, 10);
	// Set the text size to be double the normal size
	display.setTextSize(2);
	// Add the string to the buffer in the Arduino
	display.drawTextWithShadow(0, 30, "Hello World!", INKPLATE2_RED, INKPLATE2_BLACK);

	// Display the content in the buffer
	display.display();
}

// similar to a while(true) loop
void loop()
{
	// Nothing to be done in the loop...
}
