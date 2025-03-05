#include "HTML.h"

namespace HTML
{
	const char index[] = R"(
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Weather Station</title>
	<style>
		body {
			background-color: #2a2723;
			font-family: 'Courier New', Courier, monospace;
			color: #c4acac;
			margin: 0;
			padding: 20px;
			min-height: 100vh;
			position: relative;
			overflow-x: hidden;
		}

		/* Scanline overlay effect */
		body::after {
			content: "";
			position: fixed;
			top: 0;
			left: 0;
			width: 100%;
			height: 100%;
			background: repeating-linear-gradient(
				0deg,
				rgba(0,0,0,0.15) 0px,
				rgba(0,0,0,0.15) 1px,
				transparent 1px,
				transparent 2px
			);
			pointer-events: none;
			z-index: 999;
		}

		h2 {
			text-align: center;
			font-size: 24px;
			letter-spacing: 2px;
			text-transform: uppercase;
			margin: 30px 0;
			text-shadow: 2px 2px 0 rgba(0,0,0,0.3);
			position: relative;
			padding: 10px;
			background: linear-gradient(to right, #c4acac 50%, #2a2723 100%);
			-webkit-background-clip: text;
			-webkit-text-fill-color: transparent;
		}

		form {
			border: 3px solid #c4acac;
			padding: 20px;
			max-width: 320px;
			margin: 0 auto;
			box-shadow: 5px 5px 0 rgba(196,172,172,0.1);
			background-color: #3a3530;
			position: relative;
			z-index: 1;
		}

		input[type="text"],
		input[type="password"] {
			background-color: #3a3530;
			width: 100%;
			padding: 10px;
			margin: 10px 0;
			border: 2px solid #c4acac;
			font-family: 'Courier New', Courier, monospace;
			font-size: 14px;
			box-sizing: border-box;
			color: #c4acac;
			transition: all 0.3s ease;
		}

		input[type="text"]:focus,
		input[type="password"]:focus {
			outline: none;
			box-shadow: 0 0 8px rgba(196,172,172,0.4);
		}

		input[type="submit"] {
			background: #c4acac;
			color: #2a2723;
			padding: 12px 24px;
			border: 2px solid #c4acac;
			font-family: 'Courier New', Courier, monospace;
			font-size: 14px;
			cursor: pointer;
			transition: all 0.3s ease;
			display: block;
			margin: 20px auto 0;
			text-transform: uppercase;
			letter-spacing: 1px;
		}

		input[type="submit"]:hover {
			background-color: #2a2723;
			color: #c4acac;
			box-shadow: 0 0 12px rgba(196,172,172,0.4);
		}

		label {
			display: block;
			text-align: left;
			margin: 15px 0 5px;
			font-size: 14px;
			color: #908080;
		}

		.password-container {
			position: relative;
			display: block;
		}

		.toggle-password {
			position: absolute;
			right: 10px;
			top: 50%;
			transform: translateY(-50%);
			background: none;
			border: none;
			cursor: pointer;
			padding: 4px;
			width: 28px;
			height: 28px;
		}

		.icon-container {
			position: relative;
			width: 100%;
			height: 100%;
		}

		.eye-icon, .cross-icon {
			position: absolute;
			width: 100%;
			height: 100%;
			transition: all 0.3s ease;
			stroke: #c4acac;
			fill: none;
			stroke-width: 2px;
			stroke-linecap: round;
		}

		.cross-icon {
			opacity: 0;
			transform: rotate(45deg);
		}

		.toggle-password.active .eye-icon {
			opacity: 0.3;
			transform: scale(0.9);
		}

		.toggle-password.active .cross-icon {
			opacity: 1;
			transform: rotate(0deg);
		}

		.toggle-password:hover {
			background-color: rgba(196,172,172,0.1);
		}

		@media (max-width: 480px) {
			body {
				padding: 15px;
			}

			form {
				width: 100%;
				padding: 20px 15px;
			}

			h2 {
				font-size: 20px;
				margin: 20px 0;
			}
		}

		/* CRT curvature effect */
		form::before {
			content: "";
			position: absolute;
			top: -10px;
			left: -10px;
			right: -10px;
			bottom: -10px;
			border: 2px solid rgba(196,172,172,0.1);
			border-radius: 5px;
			z-index: -1;
		}
	</style>
</head>
<body>
	<h2>Connect ESP32 to WiFi</h2>
	<form action="/submit" method="POST">
		<label for="wifi_ssid">WiFi SSID (name):</label>
		<input type="text" name="wifi_ssid" id="wifi_ssid" required>

		<label for="wifi_password">WiFi password:</label>
		<div class="password-container">
			<input type="password" name="wifi_password" id="wifi_password" required>
			<button type="button" class="toggle-password" aria-label="Show password">
				<div class="icon-container">
					<svg class="eye-icon" viewBox="0 0 24 24">
						<path d="M12 9.5C10.6193 9.5 9.5 10.6193 9.5 12C9.5 13.3807 10.6193 14.5 12 14.5C13.3807 14.5 14.5 13.3807 14.5 12C14.5 10.6193 13.3807 9.5 12 9.5Z"/>
						<path d="M12 5C7 5 2.73 8.11 1 12.5C2.73 16.89 7 20 12 20C17 20 21.27 16.89 23 12.5C21.27 8.11 17 5 12 5ZM12 17.5C9.24 17.5 7 15.26 7 12.5C7 9.74 9.24 7.5 12 7.5C14.76 7.5 17 9.74 17 12.5C17 15.26 14.76 17.5 12 17.5Z"/>
					</svg>
					<svg class="cross-icon" viewBox="0 0 24 24">
						<path d="M18 6L6 18M6 6l12 12"/>
					</svg>
				</div>
			</button>
		</div>

		<input type="submit" value="Initialize Session">
	</form>

	<script>
		const toggleButton = document.querySelector('.toggle-password');
		const passwordInput = document.getElementById('wifi_password');

		// Add keyboard support
		toggleButton.addEventListener('keydown', (e) => {
			if (e.key === 'Enter' || e.key === ' ') {
				e.preventDefault();
				toggleButton.click();
			}
		});

		toggleButton.addEventListener('click', () => {
			const isVisible = passwordInput.type === 'password';
			passwordInput.type = isVisible ? 'text' : 'password';
			toggleButton.classList.toggle('active', isVisible);
			toggleButton.setAttribute('aria-label', isVisible ? 'Hide password' : 'Show password');

			// Add temporary animation effect
			passwordInput.style.transform = isVisible ? 'translateX(4px)' : 'translateX(0)';
			setTimeout(() => {
				passwordInput.style.transform = 'translateX(0)';
			}, 100);
		});
	</script>
</body>
</html>
	)";

	const char success[] = R"(
<!DOCTYPE html>
<html>
	<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<title>Weather Station</title>
		<style>
			body {
				background-color: #2a2723;
				font-family: 'Courier New', Courier, monospace;
				color: #c4acac;
				margin: 0;
				padding: 20px;
				min-height: 100vh;
				position: relative;
				overflow-x: hidden;
			}

			/* Scanline overlay effect */
			body::after {
				content: "";
				position: fixed;
				top: 0;
				left: 0;
				width: 100%;
				height: 100%;
				background: repeating-linear-gradient(
					0deg,
					rgba(0,0,0,0.15) 0px,
					rgba(0,0,0,0.15) 1px,
					transparent 1px,
					transparent 2px
				);
				pointer-events: none;
				z-index: 999;
			}

			h2 {
				text-align: center;
				font-size: 24px;
				letter-spacing: 2px;
				text-transform: uppercase;
				margin: 30px 0;
				text-shadow: 2px 2px 0 rgba(0,0,0,0.3);
				position: relative;
				padding: 10px;
				background: linear-gradient(to right, #c4acac 50%, #2a2723 100%);
				-webkit-background-clip: text;
				-webkit-text-fill-color: transparent;
			}

			label {
				display: block;
				text-align: left;
				margin: 15px 0 5px;
				font-size: 14px;
				color: #908080;
			}

			@media (max-width: 480px) {
				body {
					padding: 15px;
				}

				form {
					width: 100%;
					padding: 20px 15px;
				}

				h2 {
					font-size: 20px;
					margin: 20px 0;
				}
			}
		</style>
	</head>
	<body>
		<h2>
			<!-- textContent -->
		</h2>
	</body>
</html>
	)";
}
