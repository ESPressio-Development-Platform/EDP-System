# ESPressio System

`EDP-System` contains the foundational facilities used by the ESPressio Development Platform.

This repository is currently undergoing redevelopment from a clean foundation. Its documentation and introductory material will continue to evolve as the System library is rebuilt progressively.

## Contents

The following facilities are currently provided by the System library:

- [Composition Framework](./docs/COMPOSITION-FRAMEWORK.MD)  
  Defines compile-time domains, capabilities, providers, requirements, properties, constraints, Attributes, qualified provider resolution, and complete system compositions.

- [Composition Flag Properties](./docs/COMPOSITION-FLAG-PROPERTIES.MD)  
  Defines typed finite feature-set Properties, typed provider advertisement, and all/any/none flag constraints for compile-time provider qualification.

- [Qualified Composition Resolution](./docs/COMPOSITION-QUALIFIED-RESOLUTION.MD)  
  Defines open compile-time provider Attributes, qualified SharedCapability resolution, cross-domain `DependsOn<>` dependencies, and multi-domain `Architecture<>` validation.

- [Flag Set](./docs/FLAG-SET.MD)  
  Defines the allocation-free, strongly typed compact runtime flag representation, state-aware construction, mutation/predicate operations, raw representation access, and storage guarantees.

- [C++20 Toolchain Baseline](./docs/CPP20-TOOLCHAIN.MD)  
  Defines the required C++20 baseline, recommends PIOArduino for ESP32 Arduino builds, and documents compatible PlatformIO configuration.

- [System Identity](./docs/IDENTITY.MD)  
  Defines the memory-bounded permanent device identity and durable runtime-incarnation model, together with Composition capabilities for platform device identity and durable runtime-incarnation establishment.

## Dependencies

```text
EDP-System
└── no ESPressio library dependencies
```

`EDP-System` has no mandatory or optional ESPressio library dependencies.

See [DEPENDENCY_GRAPH.MD](./DEPENDENCY_GRAPH.MD) for the maintained dependency graph and dependency classification.

## Demos

Buildable demonstrations are provided under [`./demos`](./demos):

- `composition-basic`
- `composition-flag-properties`
- `flag-set-basic`
- `identity-basic`

Each demonstration contains the policy-required Arduino IDE, PlatformIO-compatible Arduino, and PlatformIO-compatible ESP-IDF targets.

ESPressio requires C++20. The PlatformIO-compatible demo configurations use the maintained PIOArduino ESP32 platform package rather than the older Arduino-ESP32 package bundled by the official PlatformIO Espressif32 platform. See [C++20 Toolchain Baseline](./docs/CPP20-TOOLCHAIN.MD).

## Development Status

The System library is being rebuilt progressively, beginning with the facilities upon which higher-level ESPressio libraries will depend.

Each System facility will be added to this README as it is introduced, with a relative link to its consumer documentation under [`./docs`](./docs).

The final public-release README format, complete introductory material, package-management instructions, compatibility information, and release guidance will be formalised before the redeveloped platform is made public.
