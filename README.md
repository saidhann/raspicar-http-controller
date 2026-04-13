# 🚗 RaspiCar HTTP Controller

A two-component system for remotely controlling a Raspberry Pi RC car over a local Wi-Fi network. The robot runs a C++ HTTP server exposing a REST API for motor and servo control, while a Windows Qt desktop application serves as the operator station — streaming live video and sending drive commands via keyboard.

---

## 📁 Repository Structure

```
raspicar-http-controller/
├── RobotSoftware/              # C++ REST server running on the Raspberry Pi
│   └── README.md
└── WindowsControlApplication/  # Qt desktop controller for Windows
    └── README.md
```

---

## 🏗️ System Architecture

```
┌─────────────────────────────┐         Wi-Fi / LAN          ┌──────────────────────────────┐
│   Windows Control App (Qt)  │ ◄──── HTTP REST (port 18080) ──► │   Raspberry Pi Robot Server  │
│                             │                               │                              │
│  • WASD keyboard input      │ ─── POST /engine ──────────► │  • PWM motor control         │
│  • Live video display       │ ─── POST /turn ────────────► │  • PWM servo steering        │
│  • LCD speed/angle/distance │ ◄─── GET  /distance ──────── │  • Ultrasonic sensor polling │
│                             │                               │  • Collision avoidance       │
└─────────────────────────────┘                               └──────────────────────────────┘
                                                                           │
                                                              TCP H.264 stream (port 8554)
                                                                           │
                                                              ┌────────────▼─────────────────┐
                                                              │   VideoStreamWidget (OpenCV)  │
                                                              └──────────────────────────────┘
```

---

## ⚡ Quick Start

### 1 — Start the robot server (on the Raspberry Pi)

```bash
sudo ./raspicar-server
```

The server binds to `http://192.168.137.13:18080` and starts the camera stream on TCP port `8554`.

### 2 — Launch the Windows controller

Build and run the Qt application. The controller connects automatically to the robot IP.

### 3 — Drive

| Key | Action |
|-----|--------|
| `W` | Accelerate forward |
| `S` | Accelerate backward |
| `A` | Steer left |
| `D` | Steer right |
| Release | Return to neutral |

---

## 🌐 REST API Summary

| Method | Endpoint | Description |
|--------|----------|-------------|
| `POST` | `/engine` | Set motor throttle (0–100) |
| `POST` | `/turn` | Set steering angle (0–100) |
| `GET` | `/engine` | Read current throttle |
| `GET` | `/servo` | Read current steering |
| `GET` | `/distance` | Read ultrasonic distance (cm) |

All endpoints accept and return JSON: `{ "value": <int> }`

---

## 📡 Network Configuration

| Service | Address |
|---------|---------|
| Robot HTTP API | `http://192.168.137.13:18080` |
| Video stream | `tcp://192.168.137.25:8554` |

> Update the IP addresses in `CarController` (`IP_ADRESS`) and `VideoStreamWidget` (`ipAddress`) to match your network.

---

## 📄 Component READMEs

- [`RobotSoftware/README.md`](RobotSoftware/README.md) — build instructions, hardware wiring, collision avoidance details
- [`WindowsControlApplication/README.md`](WindowsControlApplication/README.md) — Qt build instructions, UI reference, controls

---

## 📝 License

This project is open source. See [LICENSE](LICENSE) for details.
