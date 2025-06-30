# ESP32C3 OTA & BLE Provisioning Framework

This repository provides a basic framework for ESP32C3 devices using the ESP-IDF
environment. The project demonstrates how to combine Bluetooth provisioning with
OTA (Over The Air) updates.

## Requirements
- An ESP32C3 development board
- Python 3.8+
- Git

### Setting Up ESP-IDF
Run the provided script to install the ESP-IDF toolchain and set `IDF_PATH`:

```bash
./setup_esp_idf.sh
```

After installation, remember to source the environment before building:

```bash
source "$IDF_PATH/export.sh"
```

## Building the Example
```bash
source "$IDF_PATH/export.sh"  # load ESP-IDF tools
idf.py set-target esp32c3
idf.py menuconfig    # configure Wi-Fi credentials and OTA URL
idf.py build
```

## Flashing and Monitoring
```bash
idf.py -p PORT flash monitor
```
Replace `PORT` with your serial port.

## Project Structure
- `ota_ble/`: ESP-IDF project containing the example code
  - `main/app_main.c`: skeleton implementation of BLE provisioning and OTA

The code is minimal and meant as a starting point. You can extend the
provisioning logic in `start_ble_provisioning()` and customize OTA behavior in
`perform_ota_update()`.
