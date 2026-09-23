# src/identity/IdentityComposition.hpp

**Primary classification:** PUBLIC COMPOSITION API

**Source baseline:** `4a8ef43713f998d96ca0601770a0a735cd138c11`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/4a8ef43713f998d96ca0601770a0a735cd138c11/src/identity/IdentityComposition.hpp)

## Direct includes

- `../composition/Capability.hpp`

## Documented declarations

### `Domain`

**Classification:** PUBLIC COMPOSITION API

Composition domain containing System Identity bootstrap capabilities.

```cpp
struct Domain final : CompositionFramework::Domain {};
```

### `DeviceIdentity`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Exclusive capability that supplies the permanent DeviceIdentifier used by SystemIdentity.

The concrete provider belongs at the platform or hardware layer best able to establish a
stable, immutable, globally suitable device identity. System deliberately does not prescribe
whether that identity originates from hardware fuses, a factory identifier, a secure device,
provisioned storage, or another platform-appropriate source.

```cpp
struct DeviceIdentity final : CompositionFramework::ExclusiveCapability<Domain> {};
```

### `RuntimeIncarnationIdentity`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Exclusive capability that supplies the durable RuntimeIncarnationId used by SystemIdentity.

A conforming provider must establish a non-zero incarnation that is exactly one greater than
the previously committed incarnation for the same device. The advancement must be durably
committed before the provider reports success or exposes the new value. Zero is never valid,
values must never be reused, and exhaustion must fail rather than wrap. The mechanism used to
satisfy those guarantees is intentionally outside System and may be provided by Persistence,
monotonic hardware, a secure element, or another future implementation.

```cpp
struct RuntimeIncarnationIdentity final : CompositionFramework::ExclusiveCapability<Domain> {};
```

