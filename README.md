# RFID Attendance System

A dual-microcontroller RFID-based attendance tracking system using Arduino Nano and ESP32 with real-time clock, OLED display, and SD card storage.

## Overview

This system uses two microcontrollers working together:
- **Arduino Nano**: Handles RFID card reading and RTC timekeeping
- **ESP32**: Manages display, SD card storage, and attendance logic

## Features

- ✅ RFID card scanning for attendance
- ⏰ Real-time clock (RTC DS3231) for accurate timestamps
- 📊 OLED display (128x64 SSD1306) for visual feedback
- 💾 SD card storage for student and attendance records
- 🔔 Audio feedback with buzzer
- 💡 RGB LED status indicators
- 🔄 Auto-reset attendance after 12 hours
- ➕ Add new users via button interface
- 🔒 Duplicate attendance prevention

## Hardware Requirements

### Arduino Nano Components
- Arduino Nano
- MFRC522 RFID Reader Module
- DS3231 RTC Module
- Connecting wires

### ESP32 Components
- ESP32 Development Board
- SSD1306 OLED Display (128x64, I2C)
- SD Card Module
- RGB LED (Common Cathode)
- Buzzer
- Push Button
- Resistors (as needed)
- SD Card

## Pin Configuration

### Arduino Nano
```
MFRC522 RFID:
- SS_PIN: D10
- RST_PIN: D9
- SPI: MOSI, MISO, SCK (default SPI pins)

Serial to ESP32:
- RX: D2
- TX: D3

RTC DS3231:
- I2C: SDA (A4), SCL (A5)
```

### ESP32
```
OLED Display (I2C):
- SDA: GPIO 21
- SCL: GPIO 22

SD Card Module:
- CS: GPIO 13

Serial from Arduino:
- RX: GPIO 16
- TX: GPIO 17

RGB LED:
- RED: GPIO 25
- GREEN: GPIO 26
- BLUE: GPIO 27

Button: GPIO 14
Buzzer: GPIO 33
```

## Installation

### 1. Arduino IDE Setup
Install required libraries via Arduino Library Manager:
- MFRC522 by GithubCommunity
- RTClib by Adafruit
- Adafruit GFX Library
- Adafruit SSD1306

### 2. Upload Code
1. Upload `arduinonano.ino` to Arduino Nano
2. Upload `esp32.ino` to ESP32

### 3. RTC Configuration
**Important**: On first upload to Arduino Nano, uncomment this line in `setup()`:
```cpp
rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
```
Upload once, then comment it back and re-upload to prevent time reset on every restart.

### 4. SD Card Setup
- Format SD card as FAT32
- Insert into SD card module
- System will auto-create required CSV files

## Usage

### Normal Attendance Mode
1. Power on the system
2. Scan RFID card
3. System checks if user is registered
4. If registered and not already marked, attendance is recorded
5. Visual and audio feedback confirms the action

### Adding New Users
1. Press the button **3 times quickly** (within 3 seconds)
2. System enters "ADD USER MODE" (blue LED)
3. Scan the new RFID card
4. User is added to the system
5. System returns to normal mode

### System Reset
1. Press and hold the button for **7 seconds**
2. All student and attendance data will be erased
3. System resets to initial state

## Data Storage

### Files on SD Card
- `/student8py.csv`: Stores registered user UIDs
- `/attendance8py.csv`: Stores attendance records with timestamps

### CSV Format
**student8py.csv**:
```
UID
A1B2C3D4
E5F6G7H8
```

**attendance8py.csv**:
```
UID,DATE,TIME
A1B2C3D4,10/2/2026,9:30:15
E5F6G7H8,10/2/2026,9:35:42
```

## LED Status Indicators

- 🟢 **Green**: Attendance marked successfully
- 🔵 **Blue**: Add user mode active / New user added
- 🔴 **Red**: Error (not registered, already marked, or system reset)
- ⚫ **Off**: Idle state

## Audio Feedback

- **Success Melody**: Two ascending tones (attendance marked)
- **Add User Melody**: Two equal tones (user added)
- **Error Melody**: Low tone (error occurred)

## Auto-Reset Feature

The system automatically clears attendance records after 12 hours from the first scan of the day, allowing for multiple sessions (e.g., morning and afternoon classes).

## Communication Protocol

Arduino Nano sends data to ESP32 via serial in CSV format:
```
UID,DATE,TIME
```
Example: `A1B2C3D4,10/2/2026,9:30:15`

## Troubleshooting

**OLED not displaying**: Check I2C address (default 0x3C) and connections

**RFID not reading**: Verify SPI connections and MFRC522 power supply

**Time incorrect**: Re-run RTC adjustment code on Arduino Nano

**SD card not working**: Ensure FAT32 format and check CS pin connection

**Serial communication issues**: Verify TX/RX cross-connection between boards

## License

This project is open source and available for educational purposes.

## Contributing

Feel free to submit issues and pull requests for improvements.
