# ESP32C3 OTA & BLE Provisioning Framework

This repository provides a basic framework for ESP32C3 devices using the
ESP‑IDF environment. It demonstrates how to combine Bluetooth provisioning with
OTA (Over‑The‑Air) updates.

## Requirements
- An ESP32C3 development board
- Python 3.8+
- Git

### Setting Up ESP‑IDF
Run the provided script to install the ESP‑IDF toolchain and set `IDF_PATH`:

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

During the first boot the device enters BLE provisioning mode. Use Espressif's
"ESP Provisioning" mobile app to connect to the service name printed on the
serial console (e.g. `PROV_XXXXXX`) and provide your Wi‑Fi credentials. After a
successful connection the firmware automatically checks the OTA URL for an
update.

## Project Structure
- `ota_ble/`: ESP‑IDF project containing the example code
  - `main/app_main.c`: demonstrates BLE provisioning using `wifi_prov_mgr` and
    performs an OTA update once Wi‑Fi is connected

Customize the OTA URL in `perform_ota_update()` and extend the provisioning
callbacks as needed for your application.
