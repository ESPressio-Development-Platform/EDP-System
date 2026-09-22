# Composition Flag Properties Demo

This demo shows a Shared `Radio` capability whose providers advertise a typed finite feature set through `FlagPropertyValue`.

The compile-time checks demonstrate:

- `HasAllFlags` selecting the one provider which supports both acknowledgements and receive timestamps;
- `HasAnyFlags` retaining both providers when either specialised feature is acceptable;
- `HasNoFlags` filtering out the low-energy provider;
- zero runtime Composition machinery.

Targets:

- `Arduino_IDE`
- `PlatformIO_Arduino` using the maintained PIOArduino ESP32 platform
- `PlatformIO_ESP-IDF` using the maintained PIOArduino ESP32 platform

All targets require the ESPressio C++20 baseline.
