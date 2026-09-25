# src/types/FieldBinding.hpp

**Primary classification:** PUBLIC API / PRIVATE IMPLEMENTATION

**Source baseline:** `70b3de93af178340372cad3106efb8baffeaac7d`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/70b3de93af178340372cad3106efb8baffeaac7d/src/types/FieldBinding.hpp)

## `FieldBinding<TMember, TIdentifier>`

**Classification:** PUBLIC API

Compile-time association between one concrete non-static data member and its stable Type-local FieldIdentifier.

- `TMember` — pointer to the represented member.
- `TIdentifier` — exact one-byte numeric identity within the owner.

The binding exposes `Owner`, `Value`, `Member`, and strong `Identifier`. It has no runtime state.

## `Detail::MemberObjectPointerTraits`

**Classification:** PRIVATE IMPLEMENTATION

Extracts owner/value Types from a member-object pointer while keeping invalid candidates diagnostically well-formed. It is implementation machinery, not an extension registry.
