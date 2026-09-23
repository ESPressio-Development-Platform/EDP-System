# EDP-System Dependency Graph

> Revalidated: 2026-09-23 — Composition Framework enhancement migration and the reproducible PIOArduino 55.03.312-1 demo-toolchain pin change compile/build metadata only; no ESPressio-library dependency edge changed.

> Revalidated: 2026-09-22 — plural provider references use only the C++20 standard library plus existing local Composition types. They introduce no mandatory or optional ESPressio-library dependency. Existing Composition, Identity, FlagSet, and C++20 toolchain facilities remain dependency-neutral.

## Library Dependency Tree

```text
EDP-System
└── no ESPressio library dependencies
```

## Dependency Classification

`EDP-System` is the root foundation for the currently implemented EDP libraries and has no mandatory or optional ESPressio library dependencies.

The C++20 standard library/toolchain is a build/runtime facility rather than an ESPressio library dependency. The Composition Framework's structural compile-time strings, Attribute resolution, typed flag-valued Properties, and allocation-free plural provider-reference bindings, System Identity values, and the `FlagSet` fixed-width value abstraction do not introduce an ESPressio dependency.
