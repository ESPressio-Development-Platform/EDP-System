# src/types/FieldIdentifier.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `70b3de93af178340372cad3106efb8baffeaac7d`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/70b3de93af178340372cad3106efb8baffeaac7d/src/types/FieldIdentifier.hpp)

## Purpose

Defines the strong exact one-byte Type-local Field identity used across EDP schema consumers.

## `FieldIdentifier`

**Classification:** PUBLIC API

Retains exactly one `std::uint8_t`. Every value 0..255 is valid; zero is not a sentinel. Default construction is deleted so absence cannot silently become Field zero.

### `Size`

Exact semantic width, permanently one byte.

### `Storage`

Exact unsigned storage Type, `std::uint8_t`.

### constructor

Explicitly constructs one Type-local identity from exact storage.

### `Value()`

Returns the retained numeric identity.

### comparisons

Equality/inequality compare exact values; ordering is ascending numeric ordering.

No allocation, synchronization, registry, RTTI, or runtime ownership is involved.
