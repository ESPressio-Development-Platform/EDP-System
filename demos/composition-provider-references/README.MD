# Composition Provider References Demo

This demo shows the complete plural-provider flow:

~~~text
Broadcast Need
    |
    v
ProvidersSatisfying<BroadcastRequirement>
    |
    v
ProviderList<TimestampedRadio, BasicBroadcastRadio>
    |
    v
Bootstrap-owned provider instances
    |
    v
ProviderReferences<...>
    |
    v
BroadcastConsumer::BroadcastAll()
~~~

A third `LowEnergyOnlyRadio` remains owned by Bootstrap but is excluded from the injected view because it does not advertise Broadcast support.

The demo performs no runtime provider discovery or capability check.

Targets:

- `Arduino_IDE`
- `PlatformIO_Arduino` using the maintained PIOArduino ESP32 platform
- `PlatformIO_ESP-IDF` using the maintained PIOArduino ESP32 platform

All targets require the ESPressio C++20 baseline.
