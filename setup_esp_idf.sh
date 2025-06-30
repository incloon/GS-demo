#!/bin/bash
# Simple helper to install ESP-IDF for ESP32C3
set -e
IDF_VERSION="v5.2"

if [ -z "$IDF_PATH" ]; then
    export IDF_PATH="$PWD/esp-idf"
    echo "IDF_PATH not set, using $IDF_PATH"
fi

if [ ! -d "$IDF_PATH" ]; then
    echo "Cloning ESP-IDF $IDF_VERSION..."
    git clone --branch "$IDF_VERSION" --depth 1 https://github.com/espressif/esp-idf.git "$IDF_PATH"
fi

cd "$IDF_PATH"
./install.sh esp32c3
. ./export.sh

echo "ESP-IDF installed. Run 'source $IDF_PATH/export.sh' before building."

