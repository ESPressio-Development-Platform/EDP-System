# src/identity/SystemIdentity.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `4a8ef43713f998d96ca0601770a0a735cd138c11`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/4a8ef43713f998d96ca0601770a0a735cd138c11/src/identity/SystemIdentity.hpp)

## Direct includes

- `type_traits`
- `DeviceIdentifier.hpp`
- `RuntimeIncarnationId.hpp`

## Documented declarations

### `SystemIdentity`

**Classification:** PUBLIC API

Immutable, address-stable identity of the currently running application instance.

SystemIdentity owns exactly one DeviceIdentifier and one RuntimeIncarnationId.
Bootstrap constructs it only after both values have been established successfully.
The object is intentionally non-copyable and non-movable so references and pointers
borrowed by later-initialized consumers remain valid for its application lifetime.

```cpp
class SystemIdentity final
```

### `_device`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Permanent identity of this device.

```cpp
DeviceIdentifier _device;
```

### `_incarnation`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Durable identity of this running incarnation.

```cpp
RuntimeIncarnationId _incarnation;
```

### `SystemIdentity`

**Classification:** PUBLIC API · source access: `public`

Prevents creation of an identity before its required values exist.

```cpp
SystemIdentity() = delete;
```

### `SystemIdentity`

**Classification:** PUBLIC API · source access: `public`

Creates the immutable identity of the current application runtime.

Bootstrap must provide a valid DeviceIdentifier and a RuntimeIncarnationId whose
advancement has already been made durable by the RuntimeIncarnationIdentity provider.

- **Parameter `device`:** Permanent device identity.
- **Parameter `incarnation`:** Durably established current runtime incarnation.

```cpp
constexpr SystemIdentity(
            const DeviceIdentifier& device,
            const RuntimeIncarnationId& incarnation
        ) noexcept :
            _device(device),
```

### `SystemIdentity`

**Classification:** PUBLIC API · source access: `public`

Prevents creation of a second facade by copying the lifetime identity.

```cpp
SystemIdentity(const SystemIdentity&) = delete;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Prevents replacement of the lifetime identity by copy assignment.

```cpp
SystemIdentity& operator =(const SystemIdentity&) = delete;
```

### `SystemIdentity`

**Classification:** PUBLIC API · source access: `public`

Prevents relocation of the lifetime identity after construction.

```cpp
SystemIdentity(SystemIdentity&&) = delete;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Prevents relocation of the lifetime identity by move assignment.

```cpp
SystemIdentity& operator =(SystemIdentity&&) = delete;
```

### `Device`

**Classification:** PUBLIC API · source access: `public`

Returns the permanent device identity by immutable reference without copying it.

```cpp
constexpr const DeviceIdentifier& Device() const noexcept
```

### `Incarnation`

**Classification:** PUBLIC API · source access: `public`

Returns the current runtime incarnation by immutable reference without copying it.

```cpp
constexpr const RuntimeIncarnationId& Incarnation() const noexcept
```

### `IsValid`

**Classification:** PUBLIC API · source access: `public`

Indicates whether both constituent identity values are valid.

```cpp
constexpr bool IsValid() const noexcept
```

### `bool`

**Classification:** PUBLIC API · source access: `public`

Indicates whether both constituent identity values are valid.

```cpp
constexpr explicit operator bool() const noexcept
```

