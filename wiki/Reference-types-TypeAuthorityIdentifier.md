# src/types/TypeAuthorityIdentifier.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `cb81bb1b9ecb3a28330ad753a6c2d91c571c5961`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/cb81bb1b9ecb3a28330ad753a6c2d91c571c5961/src/types/TypeAuthorityIdentifier.hpp)

## Purpose

`TypeAuthorityIdentifier` is the strong exact-width identity of one globally governed Type namespace authority.

## Retained state

### `_bytes`

**PRIVATE IMPLEMENTATION.** Exact three-byte canonical authority representation. It is the sole retained state; no decoded integer cache is stored.

## Public compile-time vocabulary

### `Size`

**PUBLIC API.** Constant value `3`, defining the exact retained/canonical width.

### `Storage`

**PUBLIC API.** Alias for `std::array<std::uint8_t, Size>`, used when constructing or exchanging canonical authority bytes.

## Construction

### `TypeAuthorityIdentifier()`

Creates the all-zero Invalid/Unspecified authority.

### `TypeAuthorityIdentifier(const Storage& bytes)`

Explicitly constructs from the complete canonical three-byte representation. The constructor does not allocate, normalize, repair, or reject zero; validity remains observable through predicates.

## Access and predicates

### `Bytes()`

Returns the retained Storage by const reference without a copy.

### `Value()`

Returns an explicit `std::uint32_t` big-endian numeric view of the three canonical bytes. This is a tooling/diagnostic view and not the retained representation.

### `IsZero()`

Returns true only for the reserved all-zero Authority.

### `IsValid()`

Returns true only when the Authority is non-zero.

### explicit `operator bool()`

Equivalent validity predicate for explicit Boolean contexts.

## Comparison

### `operator ==` / `operator !=`

Compare exact canonical bytes.

### `operator <`

Provides deterministic lexicographic canonical-byte ordering.

## Invariants and resources

`static_assert(sizeof(TypeAuthorityIdentifier) == 3U)` protects exact retained size. The Type is allocation-free, has no synchronization/lifecycle machinery, and does not claim special ISR safety.
