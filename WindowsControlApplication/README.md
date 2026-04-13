# 🖥️ RaspiCar — Windows Control Application

A Qt-based desktop application for Windows that streams live video from the robot, sends drive commands via keyboard, and displays real-time speed, steering angle, and obstacle distance on an LCD-style HUD.

---

## ✨ Features

- **WASD keyboard driving** — smooth acceleration and steering with configurable ramp rate
- **Live video stream** — H.264 feed from the robot decoded via OpenCV and rendered at up to 800×600
- **Real-time HUD** — LCD displays for speed (%), steering angle (°), and distance (cm)
- **Async HTTP** — non-blocking `QNetworkAccessManager` requests keep the UI responsive
- **Frameless, rounded window** — borderless UI with a modern translucent design

---

## 📦 Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| [Qt](https://www.qt.io/) | 5 or 6 | UI framework, networking, timers |
| [OpenCV](https://opencv.org/) | 4.x+ | Video stream decoding and frame rendering |

---

## 🔨 Building

### Qt Creator (recommended)

1. Open `WindowsControlApplication.pro` (or `CMakeLists.txt`) in **Qt Creator**.
2. Select your Qt kit (MSVC or MinGW).
3. Ensure OpenCV is on your include/link path — set `OPENCV_DIR` in your environment or `.pro` file.
4. Click **Build → Build All**.

### Command line (qmake)

```bash
cd WindowsControlApplication
qmake
nmake          # MSVC
# or
mingw32-make   # MinGW
```

### Command line (CMake)

```bash
cd WindowsControlApplication
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/<version>/msvc2019_64"
cmake --build . --config Release
```

---

## ⚙️ Configuration

Before building, update the two IP constants to match your robot's network address:

**`carcontroller.h`**
```cpp
const QString IP_ADRESS = "http://192.168.137.13:18080";
```

**`videostreamwidget.cpp`**
```cpp
ipAddress = "tcp://192.168.137.25:8554";
```

> Both addresses must point to the same Raspberry Pi. They may differ if your Pi has multiple interfaces.

---

## 🎮 Controls

| Key | Action |
|-----|--------|
| `W` | Hold to accelerate forward |
| `S` | Hold to accelerate backward |
| `A` | Hold to steer left |
| `D` | Hold to steer right |
| Release any key | Return that axis to neutral |

Keys are combinable — e.g. holding `W` + `D` drives forward while steering right.

---

## 🖼️ UI Layout

```
┌──────────────────────────────────────────┐
│                                          │
│           Video Stream (OpenCV)          │  ← 7/8 of window height
│                                          │
├──────────────────────────────────────────┤
│ Angle [000]°       [00]cm      [000]% Speed │  ← HUD bar
└──────────────────────────────────────────┘
```

| HUD Element | Source | Update rate |
|-------------|--------|-------------|
| **Angle (°)** | `GET /servo` via `CarController` | 200 ms |
| **Distance (cm)** | `GET /distance` via HTTP | 1000 ms |
| **Speed (%)** | `POST /engine` value reflected locally | 200 ms |

Speed and angle are derived from the raw 0–100 API values:

```
displayed speed (%) = (engineValue − 50) × 2
displayed angle (°) = (servoValue  − 50) × 2 / 5
```

---

## 🏗️ Component Overview

### `MainWindow`
Top-level window. Captures keyboard events and delegates to `CarController`. Owns a `QTimer` that calls `refreshLCD()` every 200 ms to update the HUD displays. Runs frameless with a translucent background.

### `CarController`
Headless controller widget (not shown directly). Maintains two timers:

| Timer | Interval | Action |
|-------|----------|--------|
| `updateTimer` | 100 ms | Ramp engine/servo values, send `POST /engine` and `POST /turn` |
| `requestTimer` | 1000 ms | Send `GET /distance`, parse JSON response |

**Engine ramp behaviour:**

```
W held  → engineValue += 2 per tick (max 100)
S held  → engineValue -= 2 per tick (min 0)
neither → engineValue  = 50 (neutral)

D held  → servoValue  -= 5 per tick (min 0,  full right)
A held  → servoValue  += 5 per tick (max 100, full left)
neither → servoValue   = 50 (centre)
```

### `VideoStreamWidget`
Custom `QWidget` that opens the robot's TCP H.264 stream using `cv::VideoCapture` and renders frames via a `QLabel` updated every **30 ms** (~33 fps). Frames are converted from BGR to RGB and scaled to fit within 800×600 while preserving aspect ratio.

---

## 🐛 Troubleshooting

| Issue | Likely cause | Fix |
|-------|-------------|-----|
| "Unable to open video stream" | Wrong IP or robot server not running | Check `ipAddress` and confirm `libcamera-vid` is active on the Pi |
| HUD shows 0 / no update | Robot unreachable on port 18080 | Verify `IP_ADRESS` and that the Pi firewall allows port 18080 |
| Build error: OpenCV not found | `OPENCV_DIR` not set | Set the environment variable or add `-DOpenCV_DIR=...` to CMake |
| Black video, no error | H.264 stream not yet ready | Wait a few seconds after starting the robot server, then relaunch |
