# Smart-Traffic-System-ESP32

This project implements a Smart Traffic Light Management System using an ESP32, LED indicators, push-button inputs, and a 16x2 LCD in 4-bit mode. The system dynamically adjusts green-light duration based on vehicle count detected through button presses, simulating real-world adaptive traffic control.

## Features
- Adaptive green time based on vehicle count.
- Real-time vehicle counting using push buttons (active LOW).
- Non-blocking timing using millis() (no delay()).
- 4-way traffic control using red and green LEDs.
- LCD output showing vehicle counts, active path, and countdown timer.
- Automatic rotation between paths with simulated vehicle clearing.

## Hardware Requirements
- ESP32 Development Board
- 4x Push Buttons (INPUT_PULLUP)
- 4x Green LEDs
- 4x Red LEDs
- 8x 220Ω Resistors
- 16x2 LCD (4-bit parallel mode)
- Breadboard and jumper wires

## Pin Mapping
LCD RS  -> 33  
LCD E   -> 32  
LCD D4  -> 21  
LCD D5  -> 22  
LCD D6  -> 13  
LCD D7  -> 15  

Buttons -> 12, 14, 27, 26  
Green LEDs -> 5, 18, 19, 23  
Red LEDs   -> 2, 4, 16, 17  

## How It Works
1. Each button press increments the vehicle count for that lane.
2. Green time is calculated as: minGreen + (vehicles / 2), capped between 5–30 seconds.
3. The ESP32 cycles across 4 paths automatically.
4. LCD displays:
   - Vehicle count of all 4 lanes
   - Active lane number
   - Remaining green time
5. When a lane finishes, some vehicles are cleared (simulated) based on green duration.

## Project Purpose
- Real-time scheduling
- Sensor simulation
- LCD interfacing
- Adaptive control logic
- Non-blocking embedded programming techniques

## File Included
- esd.ino → Complete ESP32 smart traffic light system code

