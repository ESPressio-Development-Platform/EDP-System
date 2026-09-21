# Demos

Buildable demonstrations are provided for each currently exposed System feature.

- `composition-basic` — compile-time capability/provider selection, open Attributes, qualified SharedCapability resolution, and cross-domain `Architecture<>` validation.
- `identity-basic` — Device Identity, Runtime Incarnation Identity, and `SystemIdentity` bootstrap.

Each demo contains the policy-required `Arduino_IDE`, `PlatformIO_Arduino`, and `PlatformIO_ESP-IDF` targets.

ESPressio requires C++20. The PlatformIO-compatible configurations use the maintained PIOArduino ESP32 platform package so the Arduino and ESP-IDF surfaces share a current C++20-capable baseline.
