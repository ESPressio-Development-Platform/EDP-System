# src/types/TypeLocalIdentifier.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `cb81bb1b9ecb3a28330ad753a6c2d91c571c5961`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/cb81bb1b9ecb3a28330ad753a6c2d91c571c5961/src/types/TypeLocalIdentifier.hpp)

## Purpose

`TypeLocalIdentifier` is the strong exact-width 40-bit value allocated by one `TypeAuthorityIdentifier` to a semantic Type.

## Retained state

### `_bytes`

**PRIVATE IMPLEMENTATION.** Exact five-byte canonical authority-local representation. No 64-bit decoded value is retained.

## Public compile-time vocabulary

### `Size`

**PUBLIC API.** Constant value `5`.

### `Storage`

**PUBLIC API.** Alias for the exact five-byte `std::array` representation.

## Construction

### `TypeLocalIdentifier()`

Creates the reserved all-zero Invalid/Unspecified local identity.

### `TypeLocalIdentifier(const Storage& bytes)`

Explicitly constructs from canonical bytes. Invalid zero remains observable rather than being repaired.

## Access and predicates

### `Bytes()`

Returns canonical bytes by const reference.

### `Value()`

Returns the 40-bit value in a `std::uint64_t` tooling/diagnostic view. The extra host bits are not semantic storage.

### `IsZero()`

Tests the reserved all-zero local value.

### `IsValid()`

Tests whether the local value is non-zero.

### explicit `operator bool()`

Explicit validity predicate.

## Comparison

### `operator ==` / `operator !=`

Compare canonical bytes exactly.

### `operator <`

Provides deterministic lexicographic canonical-byte ordering.

## Invariants and resources

`static_assert(sizeof(TypeLocalIdentifier) == 5U)` protects the exact retained width. The Type is allocation-free, stateless beyond its five semantic bytes, and has no synchronization or provider lifetime.
