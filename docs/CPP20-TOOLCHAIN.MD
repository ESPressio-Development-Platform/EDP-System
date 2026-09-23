# C++20 Toolchain Baseline

## Requirement

ESPressio requires a C++20-capable toolchain.

The Composition Framework uses C++20 structural non-type template parameters for open compile-time descriptors, so C++17 is no longer a supported language baseline.

This is a language/toolchain requirement. It does not change ESPressio's embedded optimisation rules and does not imply use of memory-expensive Standard Library facilities.

---

## Recommended ESP32 Arduino Environment: PIOArduino

For PlatformIO-compatible ESP32 Arduino development, ESPressio recommends the actively maintained PIOArduino platform.

A minimal `platformio.ini` is:

```ini
[env:esp32dev]
platform = https://github.com/pioarduino/platform-espressif32/releases/download/55.03.312-1/platform-espressif32.zip
board = esp32dev
framework = arduino

build_unflags =
    -std=gnu++11
    -std=gnu++14
    -std=gnu++17

build_flags =
    -std=gnu++20
```

The repository demos pin PIOArduino release `55.03.312-1`, the release validated with PlatformIO Core 6.2.0 and its SCons 4.11.1 build engine.

Applications may choose a newer validated release, but should pin an exact PIOArduino release rather than relying on a moving alias when reproducible embedded builds are required.

---

## Using PlatformIO Core or the PlatformIO IDE

Developers do not need to abandon the PlatformIO project format or PlatformIO Core.

A stock PlatformIO installation can consume the maintained PIOArduino platform package by using its package URL in the project's `platform` setting:

```ini
platform = https://github.com/pioarduino/platform-espressif32/releases/download/55.03.312-1/platform-espressif32.zip
```

This avoids relying on the older Arduino-ESP32 framework version bundled by the official PlatformIO Espressif32 platform.

The C++20 build flags should still be made explicit:

```ini
build_unflags =
    -std=gnu++11
    -std=gnu++14
    -std=gnu++17

build_flags =
    -std=gnu++20
```

---

## PlatformIO-Compatible ESP-IDF Environment

The same maintained platform package can be used with ESP-IDF:

```ini
[env:esp32dev]
platform = https://github.com/pioarduino/platform-espressif32/releases/download/55.03.312-1/platform-espressif32.zip
board = esp32dev
framework = espidf

build_unflags =
    -std=gnu++11
    -std=gnu++14
    -std=gnu++17

build_flags =
    -std=gnu++20
```

A different current ESP-IDF toolchain may also be used provided the complete application build is C++20-capable.

---

## Arduino IDE

Arduino IDE users should use a current Arduino-ESP32 3.x core with its modern Espressif toolchain.

The EDP library headers require C++20. An older Arduino-ESP32 installation whose compiler or build recipe cannot compile C++20 is not a supported ESPressio environment.

---

## Library Build Metadata

`EDP-System/library.json` explicitly requests `-std=gnu++20` and removes older explicit GNU C++ standard flags when PlatformIO-compatible library metadata is honored.

Application build configuration remains authoritative for the complete translation unit. An application should therefore declare its C++20 baseline explicitly rather than assuming a dependency can upgrade an otherwise C++17 application build.

---

## Why C++20

The immediate architectural requirement is compile-time Composition metadata.

For example:

```cpp
Framework::Attribute<
    "Frequency",
    24917U
>

Framework::TextAttribute<
    "StorageType",
    "SD"
>
```

The string descriptors are structural compile-time values used by template resolution.

They are not runtime strings, a runtime registry, or a reason to adopt expensive runtime Standard Library facilities.

The embedded design rules remain unchanged:

- deterministic resource use;
- no hidden allocation;
- conservative RAM consumption;
- compile-time selection wherever architecture is known at build time.
