# EDP-System Dependency Graph

> Revalidated: 2026-09-25 — universal Field schema adds only one-byte value identity plus compile-time member-binding/schema metadata. No mandatory or optional ESPressio-library dependency edge is introduced.

> Revalidated: 2026-09-24 — universal Type identity adds only fixed-width C++20 standard-library value Types and compile-time validation. No mandatory or optional ESPressio-library dependency edge is introduced.

> Revalidated: 2026-09-23 — Composition Framework enhancement migration plus the validated PIOArduino Core v6.2.0 / platform-espressif32 55.03.312-1 demo-toolchain baseline change compile/build metadata only; no ESPressio-library dependency edge changed.

> Revalidated: 2026-09-22 — plural provider references use only the C++20 standard library plus existing local Composition types. They introduce no mandatory or optional ESPressio-library dependency. Existing Composition, Identity, FlagSet, and C++20 toolchain facilities remain dependency-neutral.

## Library Dependency Tree

```text
EDP-System
└── no ESPressio library dependencies
```

## Dependency Classification

`EDP-System` is the root foundation for the currently implemented EDP libraries and has no mandatory or optional ESPressio library dependencies.

The C++20 standard library/toolchain is a build/runtime facility rather than an ESPressio library dependency. The Composition Framework's structural compile-time strings, Attribute resolution, typed flag-valued Properties, and allocation-free plural provider-reference bindings, System Identity values, universal Type identity values/readers, universal Field identity/schema metadata, and the `FlagSet` fixed-width value abstraction do not introduce an ESPressio dependency.
