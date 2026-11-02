#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <Keypad.h>

// ==========================================
// 4x4 KEYPAD CONFIGURATION & SETUP
// ==========================================
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Define digital pins connected to the rows and columns of the keypad matrix
byte rowPins[ROWS] = {4, 5, 6, 7};
byte colPins[COLS] = {8, 9, 10, 11};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ==========================================
// MPU6050 SENSOR & MOUSE/KEYBOARD SETTINGS
// ==========================================
Adafruit_MPU6050 mpu;

const byte ledPin = 15;                  // Leonardo D15 / SCK status LED (lights up when mouse mode is active)
const unsigned long responseDelayMs = 2; // Short delay at the end of loop for quick response time
const float smoothingFactor = 0.5f;      // Low-pass filter weight: higher = more responsive, lower = smoother/less jitter
const float gyroPixelsPerDegree = 0.72f; // Scales degrees of tilt into cursor pixels moved
const float mouseSensitivity = 4.0f;     // Overall multiplier for cursor tracking speed

// Motion tracking and filtering variables
float filteredGyroX = 0.0f;
float filteredGyroY = 0.0f;
float mouseRemainderX = 0.0f;
float mouseRemainderY = 0.0f;
unsigned long lastMotionMillis = 0;

sensors_event_t a, g, temp;
bool accelerometerReady = false;
bool mouseIsActive = true;
bool lastMouseModeKey = false;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);

  // Initialize USB HID device emulation (allows Arduino to act as a keyboard/mouse)
  Keyboard.begin();
  Mouse.begin();

  // Attempt to initialize the MPU6050 gyroscope/accelerometer
  accelerometerReady = mpu.begin();
  if (accelerometerReady) {
    lastMotionMillis = millis();
  } else {
    Serial.println(F("MPU6050 not detected; keypad HID controls remain available."));
  }
}

// Helper function to check if a specific keypad button is currently pressed or being held down
bool keyIsDown(char wantedKey) {
  keypad.getKeys();
  for (byte i = 0; i < LIST_MAX; i++) {
    if (keypad.key[i].kchar == wantedKey &&
        (keypad.key[i].kstate == PRESSED || keypad.key[i].kstate == HOLD)) {
      return true;
    }
  }
  return false;
}

void loop() {
  // ----------------------------------------
  // 1. MOUSE MODE TOGGLE (Key '0')
  // ----------------------------------------
  // Pressing '0' toggles whether gyro motion controls the mouse cursor on/off.
  bool mouseModeKey = keyIsDown('0');
  if (mouseModeKey && !lastMouseModeKey) {
    mouseIsActive = !mouseIsActive;
    mouseRemainderX = 0.0f;
    mouseRemainderY = 0.0f;
  }
  lastMouseModeKey = mouseModeKey;
  digitalWrite(ledPin, mouseIsActive ? HIGH : LOW); // Status LED indicates mode state

  // ----------------------------------------
  // 2. GYRO MOTION PROCESSING & CURSOR MOVEMENT
  // ----------------------------------------
  if (accelerometerReady && mpu.getEvent(&a, &g, &temp)) {
    // Calculate time elapsed (dt) since the last motion update to handle smooth velocity integration
    unsigned long now = millis();
    float dt = (now - lastMotionMillis) / 1000.0f;
    lastMotionMillis = now;
    dt = constrain(dt, 0.001f, 0.050f); // Bound timing gaps to prevent erratic jumps after stalls

    // Convert raw angular velocity from radians/sec to degrees/sec
    const float radiansToDegrees = 57.2957795131f;
    float gyroXDegPerSec = g.gyro.x * radiansToDegrees;
    float gyroYDegPerSec = g.gyro.y * radiansToDegrees;

    // Apply a low-pass filter to smooth out sensor noise and prevent shaky cursor movement
    filteredGyroX = smoothingFactor * gyroXDegPerSec + (1.0f - smoothingFactor) * filteredGyroX;
    filteredGyroY = smoothingFactor * gyroYDegPerSec + (1.0f - smoothingFactor) * filteredGyroY;

    // If mouse tracking is enabled, translate gyro tilt rates into actual screen pixel movements
    if (mouseIsActive) {
      // Accumulate fractional pixels in remainders so sub-pixel movements aren't lost
      mouseRemainderX += -filteredGyroY * gyroPixelsPerDegree * mouseSensitivity * dt;
      mouseRemainderY += -filteredGyroX * gyroPixelsPerDegree * mouseSensitivity * dt;
      
      int moveX = (int)mouseRemainderX;
      int moveY = (int)mouseRemainderY;
      mouseRemainderX -= moveX;
      mouseRemainderY -= moveY;

      // Send the move command over USB HID if there is valid displacement
      if (moveX != 0 || moveY != 0) {
        Mouse.move(moveX, moveY, 0);
      }
    }
  }

  // ----------------------------------------
  // 3. KEYPAD GAME CONTROL BINDINGS
  // ----------------------------------------
  // Maps individual keypad keys to standard keyboard/mouse actions (e.g., WASD gaming controls)
  handleMouseButton(keyIsDown('1'));       // Key '1' -> Left Mouse Click
  handleKeyButton(keyIsDown('2'), 'w');    // Key '2' -> Move Forward ('W')
  handleKeyButton(keyIsDown('4'), 'a');    // Key '4' -> Move Left ('A')
  handleKeyButton(keyIsDown('5'), 's');    // Key '5' -> Move Backward ('S')
  handleKeyButton(keyIsDown('6'), 'd');    // Key '6' -> Move Right ('D')
  handleKeyButton(keyIsDown('3'), 'r');    // Key '3' -> Reload ('R')
  handleKeyButton(keyIsDown('7'), 'v');    // Key '7' -> Melee / Action ('V')
  handleKeyButton(keyIsDown('9'), 'e');    // Key '9' -> Use / Interact ('E')
  handleKeyButton(keyIsDown('A'), 'q');    // Key 'A' -> Inventory / Ability ('Q')

  delay(responseDelayMs); // Brief pause to prevent flooding the USB stack
}

// ----------------------------------------
// HELPER FUNCTIONS FOR HID OUTPUTS
// ----------------------------------------
void handleMouseButton(bool pressed) {
  if (pressed) {
    Mouse.press(MOUSE_LEFT);
  } else {
    Mouse.release(MOUSE_LEFT);
  }
}

void handleKeyButton(bool pressed, char key) {
  if (pressed) {
    Keyboard.press(key);
  } else {
    Keyboard.release(key);
  }
}