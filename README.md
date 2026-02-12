# 🔄 System Working Process

This section describes the operational workflow of the Arduino-based Smart Electronic Lock System.

---

## 1️⃣ System Initialization

Upon power-up, the Arduino performs full system initialization:

- Initializes the SSD1306 OLED display via I2C communication.
- Configures input buttons (A, B, C, D) using internal pull-up resistors.
- Sets the lockout output pin (Digital Pin 8) to LOW.
- Resets the wrong attempt counter to zero.
- Displays the default system state: **"Enter Code"** on the OLED.

After initialization, the system enters idle mode and waits for user input.

---

## 2️⃣ User Input Acquisition

- The system continuously monitors all input buttons.
- When a button is pressed, its corresponding character (A, B, C, or D) is appended to a temporary input buffer.
- The entered sequence is displayed in real time on the OLED.
- The system waits until exactly four characters are entered before proceeding to validation.

This enforces a fixed-length password mechanism.

---

## 3️⃣ Password Verification

After receiving four input characters:

- The entered sequence is compared with the predefined password (`ABDC`).

### ✔ If the Password is Correct

- The OLED displays **"Correct Code"**.
- The system monitors the relay status input.
- When the relay signal becomes HIGH, the OLED displays **"LOCK OPEN"**.
- The input buffer is cleared for subsequent entries.

### ❌ If the Password is Incorrect

- The wrong attempt counter increments by one.
- The OLED displays **"Wrong Code"** along with the attempt number.
- The input buffer resets to allow a new attempt.

---

## 4️⃣ Wrong Attempt Control Logic

- The system permits a maximum of four consecutive incorrect attempts.
- Each failed attempt is tracked internally.
- Normal operation continues until the maximum threshold is reached.

This mechanism prevents unlimited brute-force attempts.

---

## 5️⃣ Lockout Mode Activation

When the wrong attempt counter reaches four:

- Lockout mode is activated.
- Digital Pin 8 outputs a HIGH signal.
- All user inputs are temporarily disabled.
- The OLED displays **"LOCKED OUT"**.
- A 60-second countdown timer is initiated.

---

## 6️⃣ Lockout Timing Mechanism

- The countdown timer is implemented using the `millis()` function (non-blocking timing approach).
- Remaining time is dynamically displayed on the OLED.
- During lockout:
  - No password input is processed.
  - The lockout output signal remains HIGH.

---

## 7️⃣ Automatic Recovery

After the 60-second lockout duration:

- Lockout mode is deactivated.
- The wrong attempt counter resets to zero.
- Digital Pin 8 returns to LOW.
- The OLED reverts to the default state: **"Enter Code"**.
- Normal system operation resumes.

---

## 8️⃣ Continuous Relay Monitoring

Outside lockout mode:

- The relay status input is continuously monitored.
- If the relay signal is HIGH:
  - The OLED displays **"LOCK OPEN"**.
- When the relay signal returns LOW:
  - The system transitions back to password entry mode.

---



---
