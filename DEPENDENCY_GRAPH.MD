# EDP-System Dependency Graph

> Revalidated: 2026-09-22 — `FlagSet` uses only C++20 language/standard-library facilities and introduces no mandatory or optional ESPressio-library dependency. Existing Composition, Identity, and C++20 toolchain facilities remain dependency-neutral.

## Library Dependency Tree

```text
EDP-System
└── no ESPressio library dependencies
```

## Dependency Classification

`EDP-System` is the root foundation for the currently implemented EDP libraries and has no mandatory or optional ESPressio library dependencies.

The C++20 standard library/toolchain is a build/runtime facility rather than an ESPressio library dependency. The Composition Framework's structural compile-time strings and Attribute resolution, System Identity values, and the `FlagSet` fixed-width value abstraction do not introduce an ESPressio dependency.
