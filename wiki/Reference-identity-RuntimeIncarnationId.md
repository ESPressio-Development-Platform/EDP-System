# src/identity/RuntimeIncarnationId.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `4a8ef43713f998d96ca0601770a0a735cd138c11`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/4a8ef43713f998d96ca0601770a0a735cd138c11/src/identity/RuntimeIncarnationId.hpp)

## Direct includes

- `cstdint`

## Documented declarations

### `RuntimeIncarnationId`

**Classification:** PUBLIC API

Durable 32-bit identity of one running incarnation of a device.

Zero is Invalid/Unspecified. A RuntimeIncarnationIdentity provider is
responsible for durably advancing this value exactly once for each new
application runtime before the value is exposed to SystemIdentity.

```cpp
class RuntimeIncarnationId final
```

### `std::uint32_t _value{};`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Exact durable incarnation scalar.

```cpp
std::uint32_t _value{};
```

### `RuntimeIncarnationId`

**Classification:** PUBLIC API · source access: `public`

Creates the Invalid/Unspecified zero incarnation.

```cpp
constexpr RuntimeIncarnationId() noexcept = default;
```

### `RuntimeIncarnationId`

**Classification:** PUBLIC API · source access: `public`

Creates an incarnation from a provider-established durable value.

- **Parameter `value`:** Non-zero durable incarnation scalar.

```cpp
constexpr explicit RuntimeIncarnationId(std::uint32_t value) noexcept :
            _value(value) {}
```

### `Value`

**Classification:** PUBLIC API · source access: `public`

Returns the exact incarnation scalar.

```cpp
constexpr std::uint32_t Value() const noexcept
```

### `IsValid`

**Classification:** PUBLIC API · source access: `public`

Indicates whether this incarnation contains a valid non-zero value.

```cpp
constexpr bool IsValid() const noexcept
```

### `bool`

**Classification:** PUBLIC API · source access: `public`

Indicates whether this incarnation contains a valid non-zero value.

```cpp
constexpr explicit operator bool() const noexcept
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Compares incarnation values for equality.

- **Parameter `other`:** Incarnation to compare with this value.

```cpp
constexpr bool operator ==(const RuntimeIncarnationId& other) const noexcept
```

### `constexpr bool operator !=(const RuntimeIncarnationId& other) const no`

**Classification:** PUBLIC API · source access: `public`

Compares incarnation values for inequality.

- **Parameter `other`:** Incarnation to compare with this value.

```cpp
constexpr bool operator !=(const RuntimeIncarnationId& other) const noexcept
```

### `constexpr bool operator <(const RuntimeIncarnationId& other) const noe`

**Classification:** PUBLIC API · source access: `public`

Provides deterministic ordering by the durable incarnation scalar.

- **Parameter `other`:** Incarnation to compare with this value.

```cpp
constexpr bool operator <(const RuntimeIncarnationId& other) const noexcept
```

