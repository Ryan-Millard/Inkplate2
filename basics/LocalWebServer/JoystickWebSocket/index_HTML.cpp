#include "HTML.h"

namespace HTML
{
	const char index[] = R"delimiter(
<!DOCTYPE html>
<html lang="en">
	<head>
		<meta charset="UTF-8">
		<title>Reusable Joystick Widget</title>
		<style>
			/* Container for the joystick */
			.joystick-container {
				width: 30vw;
				height: 30vw;
				background: #f0f0f0;
				border: 2px solid #ccc;
				border-radius: 50%;
				position: relative;
				touch-action: none; /* Prevent default touch behaviors */
			}
			/* Center marker to show the center of the container */
			.joystick-center-marker {
				width: 5%;
				height: 5%;
				background: red;
				border-radius: 50%;
				position: absolute;
				left: 50%;
				top: 50%;
				transform: translate(-50%, -50%);
				pointer-events: none;
			}
			/* The draggable knob */
			.joystick-knob {
				width: 20%;
				height: 20%;
				background: #333;
				border-radius: 50%;
				position: absolute;
				left: 50%;
				top: 50%;
				transform: translate(-50%, -50%);
				cursor: pointer;
			}
			/* Coordinates display styling */
			#coords {
				margin-top: 20px;
				font-size: 16px;
				font-family: sans-serif;
			}
		</style>
	</head>
	<body>

		<!-- Joystick widget container -->
		<div id="myJoystick" class="joystick-container">
			<div class="joystick-center-marker"></div>
			<div class="joystick-knob"></div>
		</div>
		<!-- Area to display joystick coordinates -->
		<div id="coords">x: 0.00, y: 0.00</div>

		<script>
			const ws = new WebSocket("ws://192.168.4.1:81"); // IP of the ESP32 AP, port 81

			// Send joystick data to the ESP32
			function sendJoystickData(x, y) {
				const data = JSON.stringify({ x, y });
				ws.send(data);
			}

			// Joystick class to encapsulate the widget functionality
			class Joystick {
				constructor(container, coordsDisplay) {
					this.container = container;
					this.knob = container.querySelector('.joystick-knob');
					this.coordsDisplay = coordsDisplay;

					// Center coordinates of the container
					this.centerX = container.clientWidth / 2;
					this.centerY = container.clientHeight / 2;
					// Maximum distance the knob can move from center
					this.maxDistance = container.clientWidth / 2 - this.knob.clientWidth / 2;
					this.dragging = false;

					// Bind event listener to the container so any touch will reposition the knob.
					this.container.addEventListener('pointerdown', this.onPointerDown.bind(this));
				}

				onPointerDown(e) {
					e.preventDefault();
					this.dragging = true;
					// Attach move and up listeners on the document.
					document.addEventListener('pointermove', this.onPointerMoveBound = this.onPointerMove.bind(this));
					document.addEventListener('pointerup', this.onPointerUpBound = this.onPointerUp.bind(this));
					// Immediately reposition the knob to where the pointer is.
					this.moveKnob(e);
				}

				onPointerMove(e) {
					if (!this.dragging) return;
					this.moveKnob(e);
				}

				onPointerUp(e) {
					this.dragging = false;
					// Reset the knob to the center position when the pointer is released.
					this.knob.style.left = '50%';
					this.knob.style.top = '50%';
					this.knob.style.transform = 'translate(-50%, -50%)';
					this.updateCoords(this.centerX, this.centerY);
					// Remove move and up listeners.
					document.removeEventListener('pointermove', this.onPointerMoveBound);
					document.removeEventListener('pointerup', this.onPointerUpBound);
				}

				moveKnob(e) {
					// Calculate the pointer position relative to the container.
					const rect = this.container.getBoundingClientRect();
					let x = e.clientX - rect.left;
					let y = e.clientY - rect.top;

					// Calculate offset from the container's center.
					const deltaX = x - this.centerX;
					const deltaY = y - this.centerY;
					const distance = Math.sqrt(deltaX * deltaX + deltaY * deltaY);

					// If the pointer is outside the allowed radius, clamp its position.
					let clampedX = deltaX;
					let clampedY = deltaY;
					if (distance > this.maxDistance) {
						const ratio = this.maxDistance / distance;
						clampedX *= ratio;
						clampedY *= ratio;
					}

					// Set the knob's position.
					this.knob.style.left = (this.centerX + clampedX) + 'px';
					this.knob.style.top = (this.centerY + clampedY) + 'px';
					this.knob.style.transform = 'translate(-50%, -50%)';

					// Update the coordinates display (normalized to a range from -1 to 1).
					this.updateCoords(this.centerX + clampedX, this.centerY + clampedY);
				}

				updateCoords(x, y) {
					const normX = ((x - this.centerX) / this.maxDistance).toFixed(2);
					const normY = ((y - this.centerY) / this.maxDistance).toFixed(2);
					this.coordsDisplay.textContent = 'x: ' + normX + ', y: ' + normY;

					// Send the joystick data to the ESP32 via WebSocket
					sendJoystickData(normX, normY);
				}
			}

			// Instantiate the joystick by providing the container and the coordinates display elements.
			const joystickContainer = document.getElementById('myJoystick');
			const coordsDisplay = document.getElementById('coords');
			const joystick = new Joystick(joystickContainer, coordsDisplay);
		</script>
	</body>
</html>
			)delimiter";
}
