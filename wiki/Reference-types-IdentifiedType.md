# src/types/IdentifiedType.hpp

**Primary classification:** PUBLIC API with PRIVATE IMPLEMENTATION validation helpers

**Source baseline:** `cb81bb1b9ecb3a28330ad753a6c2d91c571c5961`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/cb81bb1b9ecb3a28330ad753a6c2d91c571c5961/src/types/IdentifiedType.hpp)

## Public declarations

### `IdentifiedType<TType>`

**PUBLIC API.** C++20 concept identifying semantic Types which directly declare a valid universal identity.

The template parameter `TType` is the semantic Type being validated.

The concept requires a directly readable static `TType::Identifier`, the exact `System::TypeIdentifier` semantic Type, a constant-expression identity, and valid non-zero Authority/local components.

It is a predicate only and never supplies metadata.

### `TypeIdentifierOf<TType>`

**PUBLIC API.** Inline constexpr variable template returning the semantic Type's directly declared universal Type identity.

The template parameter `TType` is the semantic Type whose identity is requested.

Malformed declarations trigger focused static assertions rather than falling back to a registry or trait specialisation.

## Private validation machinery

The following declarations live in `ESPressio::System::Detail` and are implementation machinery, not extension points.

### `HasTypeIdentifierMember<TType>`

**PRIVATE IMPLEMENTATION.** Detection concept which determines whether the static member expression `TType::Identifier` exists.

### `HasExactTypeIdentifier<TType>()`

**PRIVATE IMPLEMENTATION.** consteval predicate requiring the declared member Type, after cv removal, to be exactly `System::TypeIdentifier`.

### `HasConstantTypeIdentifier<TType>()`

**PRIVATE IMPLEMENTATION.** consteval predicate verifying that validity of the exact identifier can be evaluated as a constant expression.

### `IsIdentifiedType<TType>()`

**PRIVATE IMPLEMENTATION.** consteval predicate composing presence, exact-Type, constant-expression and validity requirements for the public concept.

### `ReadTypeIdentifier<TType>()`

**PRIVATE IMPLEMENTATION.** consteval reader used by `TypeIdentifierOf`. It emits distinct diagnostics for missing member, wrong strong Type, non-constant declaration and reserved invalid identity before returning the direct declaration.

## Extension boundary

No helper in `Detail` may become an external specialisation point. Identity authority remains the semantic Type's own `Identifier` declaration.
