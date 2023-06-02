# Wireless NodeMCU MIDI Piano

This project enables you to build a wireless MIDI piano using a NodeMCU board and control it remotely from a MIDI controller or software. The NodeMCU board acts as a MIDI device and communicates with the MIDI controller over Wi-Fi.

## Features

- Wireless MIDI communication between the NodeMCU board and MIDI software.
- Support for multiple MIDI channels and note velocity.
- Customizable mappings between MIDI notes and GPIO pins.
- Easy integration with popular MIDI software and controllers.

## Hardware Requirements

To set up the wireless MIDI piano, you will need the following hardware components:

- NodeMCU ESP8266 development board
- MIDI controller or software
- Breadboard and jumper wires
- Resistors and capacitors (as required)
- Power supply for NodeMCU board (USB cable or external power source)
- Wi-Fi network for communication between NodeMCU and MIDI controller

## Software Requirements

The software requirements for this project are as follows:

- Arduino IDE (version 1.8.13 or later)
- ESP8266 core for Arduino (installation instructions provided below)
- Required libraries (MIDI and ESP8266WiFi)

## Installation and Setup

1. Clone or download the project from the [wireless-nodemcu-midi-piano](https://github.com/durveshodi/wireless-nodemcu-midi-piano) repository.

2. Install the Arduino IDE from the official Arduino website: [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software).

3. Install the ESP8266 core for Arduino by following the instructions provided in the official Arduino ESP8266 core repository: [https://github.com/esp8266/Arduino#installing-with-boards-manager](https://github.com/esp8266/Arduino#installing-with-boards-manager).

4. Install the required libraries by navigating to **Sketch > Include Library > Manage Libraries** in the Arduino IDE. Search for and install the following libraries:
   - MIDI library by Francois Best
   - ESP8266WiFi library by Arduino

5. Connect the NodeMCU board to your computer using a USB cable.

6. Open the `main.ino` file in the Arduino IDE.

7. In the Arduino IDE, select the appropriate board by navigating to **Tools > Board** and choosing the NodeMCU 1.0 (ESP-12E Module) option.

8. Select the correct port by navigating to **Tools > Port** and choosing the port that corresponds to your NodeMCU board.

9. Customize the MIDI note-to-GPIO pin mappings and Wi-Fi credentials in the code, if required.

10. Click the **Upload** button in the Arduino IDE to compile and upload the code to the NodeMCU board.

11. Once the upload is complete, open the Serial Monitor in the Arduino IDE to view the IP address assigned to the NodeMCU board.

12. Connect your MIDI controller or software to the same Wi-Fi network as the NodeMCU board.

13. Use your MIDI controller or software to send MIDI messages to the IP address displayed in the Serial Monitor. The NodeMCU board will interpret the messages and control the corresponding GPIO pins.

14. Enjoy playing the wireless MIDI piano!

## Troubleshooting

- If you encounter any issues during installation or setup, refer to the documentation and troubleshooting guides provided by the Arduino IDE, ESP8266 core, and the MIDI library.

- Ensure that the Wi-Fi network is functioning properly and that the NodeMCU board is connected to the correct network.

- Verify the GPIO pin connections and ensure that they are properly wired
