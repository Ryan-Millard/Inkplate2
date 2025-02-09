#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h"
Inkplate display;

struct ColorCombo {
	int background;
	int textColor;
	int shadowColor;
};

ColorCombo getColorPermutation(int index) {
	static constexpr int colors[] = {INKPLATE2_BLACK, INKPLATE2_RED, INKPLATE2_WHITE};
	static constexpr int permutations[][3] = {
		{0, 1, 2},  // BLACK, RED, WHITE
		{0, 2, 1},  // BLACK, WHITE, RED
		{1, 0, 2},  // RED, BLACK, WHITE
		{1, 2, 0},  // RED, WHITE, BLACK
		{2, 0, 1},  // WHITE, BLACK, RED
		{2, 1, 0}   // WHITE, RED, BLACK
	};
	return {
		colors[permutations[index][0]],
		colors[permutations[index][1]],
		colors[permutations[index][2]]
	};
}

void setup() {
	display.begin();
	display.clearDisplay();
}

void loop() {
	static int permutationIndex = 0;
	static constexpr int totalPermutations = 6;
	static constexpr int textSize = 2;

	ColorCombo combo = getColorPermutation(permutationIndex);
	display.fillScreen(combo.background);
	display.setTextSize(textSize);

	// Calculate center position for "Hello World!"
	// Using approximate width based on text size
	const char* text = "Hello World!";
	int charWidth = 6 * textSize;  // Base character width is ~6 pixels
	int charHeight = 8 * textSize; // Base character height is ~8 pixels
	int textWidth = strlen(text) * charWidth;
	int textHeight = charHeight;

	int textX = (display.width() - textWidth) / 2;
	int textY = (display.height() - textHeight) / 2;

	// Draw centered "Hello World!"
	display.drawTextWithShadow(textX, textY, text, combo.textColor, combo.shadowColor);

	// Show permutation counter
	char buffer[32];
	sprintf(buffer, "Perm %d/%d", permutationIndex + 1, totalPermutations);

	// Position the counter at the bottom
	int bufferWidth = strlen(buffer) * charWidth;
	int bufferX = (display.width() - bufferWidth) / 2;
	int bufferY = display.height() - (2 * charHeight);  // Leave some margin at bottom

	display.drawTextWithShadow(bufferX, bufferY, buffer, combo.textColor, combo.shadowColor);

	display.display();
	delay(2000);

	permutationIndex = (permutationIndex + 1) % totalPermutations;
}
