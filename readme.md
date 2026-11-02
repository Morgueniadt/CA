
##  System Architecture & Hardware Components

### Required Hardware
1. **Microcontroller:** Arduino Leonardo / Micro (ATmega32U4)
2. **Motion Sensor:** MPU6050 6-axis Gyroscope & Accelerometer Module (I2C)
3. **Input Matrix:** 4x4 Membrane Keypad
4. **Visual Indicator:** Onboard LED on Digital Pin 15 (SCK Status)
5. **Connecting Hardware:** Breadboard, Jumper Wires, Micro-USB Cable

### Hardware Connection Table & Pinout Diagram

```
+-------------------+---------------------+----------------------+
| Component         | Component Pin       | Microcontroller Pin  |
+-------------------+---------------------+----------------------+
| MPU6050 Gyro      | VCC                 | 5V                   |
|                   | GND                 | GND                  |
|                   | SDA                 | Pin 2 (SDA)          |
|                   | SCL                 | Pin 3 (SCL)          |
+-------------------+---------------------+----------------------+
| 4x4 Keypad        | Row 1               | Pin 4                |
|                   | Row 2               | Pin 5                |
|                   | Row 3               | Pin 6                |
|                   | Row 4               | Pin 7                |
|                   | Col 1               | Pin 8                |
|                   | Col 2               | Pin 9                |
|                   | Col 3               | Pin 10               |
|                   | Col 4               | Pin 11               |
+-------------------+---------------------+----------------------+
| Visual LED System | Positive / Anode    | Pin 15 (Built-in)    |
+-------------------+---------------------+----------------------+
```

---

## 💻 Microcontroller Selection Rationale

This system uses a **Native USB (HID) approach** (Arduino Leonardo). 

* **Why Native USB (HID)?** Unlike the Arduino Uno R3 (which lacks native USB support and requires an external serial bridge application running on the PC), boards with ATmega32U4 chips communicate directly with host operating systems using native USB Human Interface Device (HID) drivers. 
* **Benefits:** This eliminates latency, reduces system complexity by eliminating host-side translation scripts (e.g., Python or Processing bridges), and makes the controller plug-and-play across any host platform without special software installations.

---

## ⚙️ Firmware Setup & Installation

### Required Libraries
Ensure the following libraries are installed via the Arduino IDE Library Manager (`Sketch` -> `Include Library` -> `Manage Libraries`):
1. `Adafruit MPU6050`
2. `Adafruit Unified Sensor`
3. `Keypad` by Mark Stanley, Alexander Brevig
4. `Keyboard` (Built-in for ATmega32U4)
5. `Mouse` (Built-in for ATmega32U4)

### Flashing Steps
1. Connect your ATmega32U4 board via USB.
2. Open `Source/controller_firmware/main.ino` in Arduino IDE.
3. Select **Tools > Board > Arduino Leonardo** (or Arduino Micro).
4. Select the correct COM Port under **Tools > Port**.
5. Click **Upload**.

---

## 🎮 Gameplay Controls & Input Mapping

The controller is mapped out for Standard First-Person / Third-Person PC games (e.g., *DOOM*, *Half-Life*, *FPS micro-games*):

| Controller Input | Hardware Element | In-Game Mapping / Action |
| :--- | :--- | :--- |
| **Physical Tilting** | MPU6050 Gyroscope | Mouse Aiming / Camera Look |
| **Key '1'** | Keypad (R1, C1) | Left Mouse Click (Primary Fire) |
| **Key '2'** | Keypad (R1, C2) | `W` Key (Move Forward) |
| **Key '3'** | Keypad (R1, C3) | `R` Key (Reload) |
| **Key '4'** | Keypad (R2, C1) | `A` Key (Strafe Left) |
| **Key '5'** | Keypad (R2, C2) | `S` Key (Move Backward) |
| **Key '6'** | Keypad (R2, C3) | `D` Key (Strafe Right) |
| **Key '7'** | Keypad (R3, C1) | `V` Key (Melee Attack) |
| **Key '9'** | Keypad (R3, C3) | `E` Key (Interact) |
| **Key '*'** | Keypad (R4, C1) | `Q` Key (Quick Ability) |
| **Key '0'** | Keypad (R4, C2) | **Motion Tracking Toggle (ON / OFF)** |

---

## 💡 Visual Feedback System Operation

* **Visual Status Indicator:** Digital Pin 15 (Onboard LED).
* **Operation:** Pressing key **`0`** on the matrix keypad toggles the motion-aiming sub-routine on and off.
* **Debugging & Gameplay Context:** 
  * When LED is **ON**: Gyroscope mouse tracking is active.
  * When LED is **OFF**: Gyroscope mouse tracking is paused (allowing unobtrusive UI navigation or resting hands without moving the camera).

---

## 📦 Zip Structure Verification

```
PHYSICAL_INTERACTION_CA2.zip
├── README.md
├── Source/
│   └── controller_firmware/
│       └── main.ino
└── Docs/
    └── Physical_Interaction_CA2_Report.pdf
```