# src/types/TypeIdentifier.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `cb81bb1b9ecb3a28330ad753a6c2d91c571c5961`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/cb81bb1b9ecb3a28330ad753a6c2d91c571c5961/src/types/TypeIdentifier.hpp)

## Purpose

`TypeIdentifier` is the universal exact eight-byte identity of one EDP semantic Type.

Bytes 0..2 hold `TypeAuthorityIdentifier`; bytes 3..7 hold `TypeLocalIdentifier`.

## Retained state

### `_bytes`

**PRIVATE IMPLEMENTATION.** Sole retained canonical eight-byte identity representation. Components are reconstructed from this storage rather than duplicated.

## Public compile-time vocabulary

### `Size`

**PUBLIC API.** Constant value `8`.

### `Storage`

**PUBLIC API.** Alias for the canonical eight-byte `std::array` representation.

## Construction

### `TypeIdentifier()`

Creates the all-zero Invalid/Unspecified complete identity.

### `TypeIdentifier(const Storage& bytes)`

Explicitly constructs from complete canonical bytes. This is the lossless byte boundary used by schema/tooling integrations.

### `TypeIdentifier(const TypeAuthorityIdentifier& authority, const TypeLocalIdentifier& localIdentifier)`

Constructs the same canonical eight bytes from the two strong identity components without retaining extra state.

## Access and predicates

### `Bytes()`

Returns the exact retained eight-byte representation by const reference.

### `Authority()`

Returns a strong three-byte Authority reconstructed from bytes 0..2.

### `LocalIdentifier()`

Returns a strong five-byte local identity reconstructed from bytes 3..7.

### `IsZero()`

Tests whether the entire eight-byte representation is zero.

### `IsValid()`

Requires both the Authority and local component to be independently valid/non-zero. A partially-zero identity is invalid even when the complete eight bytes are not all zero.

### explicit `operator bool()`

Explicit validity predicate.

## Comparison

### `operator ==` / `operator !=`

Compare exact canonical identity bytes.

### `operator <`

Provides deterministic lexicographic ordering over the eight canonical bytes.

## Invariants and resources

`static_assert(sizeof(TypeIdentifier) == 8U)` protects the published width. There is no heap, runtime registry, Type allocation mechanism, provider dependency, synchronization primitive or platform SDK coupling.
