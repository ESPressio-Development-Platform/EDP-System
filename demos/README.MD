# Demos

Buildable demonstrations are provided for each currently exposed System feature.

- `composition-basic` — compile-time capability/provider selection, open Attributes, qualified SharedCapability resolution, and cross-domain `Architecture<>` validation.
- `composition-flag-properties` — typed finite feature-set Properties, typed provider advertisement, and all/any/none flag constraints used for compile-time provider qualification.
- `composition-provider-references` — non-owning Bootstrap binding of plural provider-resolution results and deterministic traversal across all selected runtime provider instances.
- `flag-set-basic` — strongly typed compact flag storage, explicit per-flag state construction, flag mutation/predicates, and masked raw assignment.
- `identity-basic` — Device Identity, Runtime Incarnation Identity, and `SystemIdentity` bootstrap.
- `type-identity-basic` — canonical universal Type identity, strong 24/40 components, direct semantic-Type declaration, and compile-time identification readers.

Each demo contains the policy-required `Arduino_IDE`, `PlatformIO_Arduino`, and `PlatformIO_ESP-IDF` targets.

ESPressio requires C++20. The PlatformIO-compatible configurations use the maintained PIOArduino ESP32 platform package so the Arduino and ESP-IDF surfaces share a current C++20-capable baseline.
