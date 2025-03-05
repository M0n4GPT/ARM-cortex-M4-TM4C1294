# Peripheral Test Modules

This folder contains test modules for several hardware peripherals, primarily focusing on **AD9850**, **MAX262**, **SPI Displays**, **TM1638**, and **DA (Digital-to-Analog)** module. The purpose of these test modules is to validate the functionality of these peripherals when integrated with the **ARM Cortex-M4 (TM4C1294)** platform.

## Folder Structure:
The folder includes the following subdirectories, each corresponding to a specific module or version of the tests:

- **AD9850/**: Contains test code for the AD9850 waveform generator module. It includes a basic setup for initializing the module and generating waveforms.
- **MAX262/**: Test code for the MAX262 voltage-controlled oscillator (VCO) module, verifying its frequency modulation and control features.
- **SPISoftwareDisplay/**: Contains code for testing an SPI-based display, using software-driven SPI communication to display information/image on the screen.
- **TM1638/**: Includes test code for the TM1638, an 8-digit 7-segment display module, focusing on driving the display and setting up communication.
- **DA/**: Test code for Digital-to-Analog conversion functionality, validating the DA module’s ability to output analog signals from digital data.
- **MAX262_v2/**: Test code for a revised version of the MAX262 module (version 2), with updated functionality and performance improvements.
- **SPISoftwareDisplay_old/**: Legacy code for SPI-based display using software-driven SPI communication (older version), maintained for backward compatibility with previous versions of the test system.

## Usage:
Ensure that your **ARM Cortex-M4 (TM4C1294)** development environment is properly configured to run these tests.

## Dependencies:
- **ARM Cortex-M4 (TM4C1294)** development board.
- Appropriate libraries and drivers for interfacing with the specified modules (Have already been included inside the folders).
- **Keil 5** for compiling and uploading code to the development board.

## Contributions:
If you have suggestions, improvements, or bug fixes, feel free to open an issue or submit a pull request.
