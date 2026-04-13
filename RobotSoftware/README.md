# 🚗 RaspiCar

A Raspberry Pi–based smart car controlled via an embedded HTTP server. The firmware is written in **C/C++** and cross-compiled for the **aarch64** (ARM Cortex-A53) architecture. GPIO control is handled through the **pigpio** library, making this a fully native, low-latency solution for remote car control.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
  - [Cross-Compilation Setup](#cross-compilation-setup)
  - [Building the Project](#building-the-project)
  - [Deploying to the Raspberry Pi](#deploying-to-the-raspberry-pi)
- [Running the Server](#running-the-server)
- [Usage](#usage)
- [Configuration](#configuration)
- [Contributing](#contributing)
- [License](#license)

---

## Overview

RaspiCar is an embedded systems project that turns a Raspberry Pi into the brain of a remote-controlled car. A lightweight HTTP server runs directly on the Pi, accepting control commands over a local network. The project is entirely native C/C++, cross-compiled on a host machine targeting the Pi's ARM Cortex-A53 processor.

---

## Features

- 🌐 **Embedded HTTP server** — control the car from any device on your network via HTTP requests
- ⚡ **Native C/C++ firmware** — high performance with minimal overhead
- 🔧 **Cross-compilation** — build on your development machine, deploy to the Pi
- 🎮 **GPIO control via pigpio** — reliable, low-level hardware control
- 📦 **CMake build system** — clean, reproducible builds

---

## Hardware Requirements

| Component | Notes |
|---|---|
| Raspberry Pi 3 / 4 | ARM Cortex-A53, aarch64 |
| DC motors + motor driver | e.g., L298N or similar |
| RC car chassis | 2WD or 4WD |
| Power supply / battery pack | Sufficient for motors + Pi |
| MicroSD card | 8 GB+ with Raspberry Pi OS |
| Wi-Fi / Ethernet | For HTTP control |

---

## Software Requirements

### On the Development Machine (Host)

- CMake ≥ 3.10
- `aarch64-linux-gnu-gcc` / `aarch64-linux-gnu-g++` (ARM cross-compiler)
- Make or Ninja

Install the cross-compiler on Ubuntu/Debian:

```bash
sudo apt update
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu cmake
```

### On the Raspberry Pi (Target)

- Raspberry Pi OS (64-bit recommended)
- `pigpio` library

Install pigpio on the Pi:

```bash
sudo apt update
sudo apt install pigpio
```

---

## Project Structure

```
RaspiCar/
├── src/              # Main application source (main.cpp)
├── inc/              # Project header files
├── pifiles/          # Pi-specific files (libpigpio.so, etc.)
├── cmake/            # CMake helper modules
├── build/            # Build output directory (generated)
├── .vscode/          # VS Code workspace settings
└── CMakeLists.txt    # Build configuration
```

---

## Getting Started

### Cross-Compilation Setup

The project is configured to cross-compile from an x86_64 host to `aarch64` (Raspberry Pi). Ensure the following compilers are available on your host machine:

- `aarch64-linux-gnu-gcc`
- `aarch64-linux-gnu-g++`

The `CMakeLists.txt` already sets these up for you along with the `-mcpu=cortex-a53` optimization flag.

### Building the Project

Clone the repository and build using CMake:

```bash
git clone https://github.com/saidhann/RaspiCar.git
cd RaspiCar

mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

The compiled binary `MyHttpServer` will be placed in the `build/` directory.

### Deploying to the Raspberry Pi

Copy the binary and any required shared libraries to the Pi over SSH:

```bash
# Copy the binary
scp build/MyHttpServer pi@<raspberry-pi-ip>:/home/pi/

# Copy pigpio shared library (if not already installed on Pi)
scp pifiles/libpigpio.so pi@<raspberry-pi-ip>:/home/pi/
```

Replace `<raspberry-pi-ip>` with your Pi's actual IP address.

---

## Running the Server

SSH into the Raspberry Pi and start the HTTP server:

```bash
ssh pi@<raspberry-pi-ip>

# pigpio requires root privileges for GPIO access
sudo ./MyHttpServer
```

The HTTP server will start and listen for incoming control commands on the local network.

---

## Usage

Once the server is running, you can send HTTP requests from any device on the same network to control the car. For example, using `curl` from your host machine:

```bash
# Example control command
curl http://<raspberry-pi-ip>:<port>/command
```

> **Note:** Replace `<port>` and `/command` with the actual endpoint paths defined in `src/main.cpp`.

You can also build a simple web interface or mobile app to send requests to the car from a browser.

---

## Configuration

The build system is configured in `CMakeLists.txt`. Key settings:

| Setting | Value | Description |
|---|---|---|
| `CMAKE_SYSTEM_PROCESSOR` | `aarch64` | Target architecture |
| `CMAKE_C/CXX_COMPILER` | `aarch64-linux-gnu-gcc/g++` | Cross-compiler |
| `CMAKE_CXX_FLAGS` | `-mcpu=cortex-a53` | CPU-specific optimization |
| `CMAKE_CXX_STANDARD` | `17` | C++ standard |

To change the target architecture or compiler, edit the relevant lines at the top of `CMakeLists.txt`.

---

## Contributing

Contributions are welcome! To get started:

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Commit your changes: `git commit -m "Add my feature"`
4. Push to the branch: `git push origin feature/my-feature`
5. Open a Pull Request

Please make sure your code compiles cleanly before submitting a PR.

---

## License

This project does not currently specify a license. If you plan to use or distribute this code, please reach out to the author.

---

> Built with ❤️ using C++, CMake, and a Raspberry Pi.
