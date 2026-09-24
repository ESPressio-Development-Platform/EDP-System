# Private Implementation

Private implementation details are documented because they are essential to safe maintenance even though they are not APIs.

## Composition

Implementation machinery evaluates requirements from Types only; it must not introduce runtime registries, allocation, mutable global state, or hidden provider ownership. Provider matching must remain deterministic and compile-time diagnosable.

## FlagSet

The implementation retains exactly the selected integer value. Changes must preserve representability checks and must not introduce auxiliary heap-backed state.

## Universal Type identity

The public identifier Types retain only canonical byte arrays of their exact semantic width. Component extraction reconstructs strong values from those bytes rather than retaining duplicate decoded integers.

The private `System::Detail` helpers in `IdentifiedType.hpp` perform detection and focused diagnostics. They must remain readers/validators only: they must not become an external trait-specialisation identity registry.

## System Identity

Identity representations are exact-width values. Device identity and runtime-incarnation identity have distinct semantic roles and must not be conflated. Durable incarnation state must be committed before an incarnation is exposed as current system identity.

Implementation changes should be checked against `docs/`, compile-time tests and downstream Composition/Type-identity users because EDP-System is foundational.
