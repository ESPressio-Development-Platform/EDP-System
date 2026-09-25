# src/types/FieldSet.hpp

**Primary classification:** PUBLIC API / PRIVATE IMPLEMENTATION

**Source baseline:** `70b3de93af178340372cad3106efb8baffeaac7d`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/70b3de93af178340372cad3106efb8baffeaac7d/src/types/FieldSet.hpp)

## `FieldSet<TFields...>`

**Classification:** PUBLIC API

Canonical compile-time enumerable Field schema for one semantic Type.

Each template argument must be an exact `FieldBinding`. The set validates one common owner, unique numeric FieldIdentifier values, and unique member bindings.

### `Owner`

Common owning Type for non-empty sets, or `void` for an empty FieldSet because no owner can be inferred from members.

### `Count`

Compile-time number of represented Fields.

### `ForEach(callable)`

Invokes a templated nullary callable once per FieldBinding in declaration order. Traversal creates no container or registry.

## Detail helpers

**Classification:** PRIVATE IMPLEMENTATION

`IsFieldBinding`, member/identifier token Types, `AreUniqueTypes`, and `FieldSetOwner` provide deterministic compile-time validation and carry no runtime state.
