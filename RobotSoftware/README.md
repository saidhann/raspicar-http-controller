# 🤖 RaspiCar — Robot Software

C++ HTTP server running on the Raspberry Pi. Exposes a REST API for real-time motor and servo control, polls an HC-SR04 ultrasonic sensor for collision avoidance, and streams live video via `libcamera-vid`.

---

## 🔧 Hardware Requirements

| Component | GPIO Pin | Notes |
|-----------|----------|-------|
| DC Motor / ESC | **5** | PWM at 50 Hz |
| Steering Servo | **18** | PWM at 50 Hz |
| Ultrasonic TRIG | **20** | HC-SR04 trigger |
| Ultrasonic ECHO | **19** | HC-SR04 echo |
| Camera module | — | libcamera compatible |

> Pin assignments are defined as `#define` constants at the top of `main.cpp` and can be changed without touching any other code.

---

## 📦 Dependencies

| Library | Purpose | Install |
|---------|---------|---------|
| [pigpio](https://abyz.me.uk/rpi/pigpio/) | GPIO and PWM control | `sudo apt install pigpio libpigpio-dev` |
| [Crow](https://crowcpp.org/) | Lightweight C++ HTTP server | Header-only, see below |
| `sonar.h` | HC-SR04 distance measurement | Included in repo |

### Install Crow (header-only)

```bash
git clone https://github.com/CrowCpp/Crow.git
# Copy include/crow.h into your include path, or install system-wide
```

---

## 🔨 Building

```bash
cd RobotSoftware
mkdir build && cd build
cmake ..
make
```

> Requires a C++17 compiler and CMake ≥ 3.10.

---

## 🚀 Running

```bash
sudo ./raspicar-server
```

`sudo` is required for pigpio to access GPIO hardware. On launch the server will:

1. Initialise pigpio and configure PWM on engine and servo pins
2. Start the camera stream: `libcamera-vid -t 0 --inline --listen -o tcp://0.0.0.0:8554`
3. Begin polling the ultrasonic sensor in a background thread
4. Start the Crow HTTP server on port **18080**

---

## 🌐 REST API

Base URL: `http://<ROBOT_IP>:18080`

---

### `POST /engine`
Set the motor throttle.

**Request**
```json
{ "value": 80 }
```

| Value | Meaning |
|-------|---------|
| `0–49` | Reverse (lower = faster reverse) |
| `50` | Neutral / stop |
| `51–100` | Forward (higher = faster forward) |

The duty cycle is computed as `value / 5 + 70`, giving a range of **70–90** out of 1024 at 50 Hz — compatible with standard RC ESCs.

**Response**
```
200 OK — "PWM set to 80% (Duty Cycle: 86)."
400     — Invalid value or missing field
500     — Internal server error
```

---

### `POST /turn`
Set the steering servo angle.

**Request**
```json
{ "value": 50 }
```

| Value | Meaning |
|-------|---------|
| `0` | Full left |
| `50` | Centre (straight) |
| `100` | Full right |

Duty cycle computed as `value / 5 + 74`, giving a range of **74–94**.

---

### `GET /engine`
Returns the current throttle percentage.

**Response**
```json
{ "value": 80 }
```

---

### `GET /servo`
Returns the current steering percentage.

**Response**
```json
{ "value": 50 }
```

---

### `GET /distance`
Returns the latest ultrasonic sensor reading.

**Response**
```json
{ "value": 42.3 }
```

Distance is in **centimetres**. A value of `-1` indicates a failed measurement (timeout exceeded 30 ms).

---

## 🛡️ Collision Avoidance

The ultrasonic sensor is polled every **250 ms** in a dedicated background thread. Three protective actions are available based on the current speed and measured distance:

| Distance | Engine value | Action |
|----------|-------------|--------|
| < 30 cm | > 90% | **Emergency back-off** — reverses until distance recovers past the initial reading |
| < 30 cm | 70–90% | **Emergency brake-turn** — brief throttle spike then neutral, helps the car pivot away |
| < 30 cm | ≤ 70% | **Normal stop** — throttle cut to neutral (duty cycle 80) |

### Adaptive speed limiter

A rolling counter tracks how long the sensor has read under 200 cm. Once the counter exceeds a threshold, the maximum engine value sent via `/engine` is capped to:

```
max_speed = min(50 + distance_cm / 4, 100)
```

This smoothly reduces top speed as the car approaches obstacles, and restores full speed once open space is detected again.

---

## 📡 Video Stream

The server automatically launches a raw H.264 stream:

```
tcp://0.0.0.0:8554
```

View it with VLC:

```bash
vlc tcp/h264://<ROBOT_IP>:8554
```

Or connect with OpenCV (as the Windows client does):

```cpp
cv::VideoCapture cap("tcp://<ROBOT_IP>:8554");
```

---

## ⚙️ Configuration Reference

All key constants in `main.cpp`:

```cpp
#define GPIO_FREQUENCY  50    // PWM frequency (Hz)
#define ENGINE_PIN       5    // Motor ESC PWM pin
#define SERVO_PIN       18    // Steering servo PWM pin
#define TRIG_PIN        20    // Ultrasonic trigger
#define ECHO_PIN        19    // Ultrasonic echo
#define TIMEOUT_US   30000    // Sensor timeout (microseconds)
```

---

## 🔄 Thread Safety

The server uses `std::mutex` and `std::shared_mutex` to protect all shared state:

| Variable | Guard |
|----------|-------|
| `enginePercentage` | `engineMutex` |
| `servoPercentage` | `servoMutex` |
| `currentDistance` | `ultrasonicMutex` |
| `currentLimit` | `limitMutex` |

HTTP handler threads acquire shared locks for reads and exclusive locks for writes, ensuring safe concurrent access from the multithreaded Crow server.
