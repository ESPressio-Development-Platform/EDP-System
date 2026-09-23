# src/identity/DeviceIdentifier.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `9964ab2820cc5d290de69674000e011faaa684d9`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/9964ab2820cc5d290de69674000e011faaa684d9/src/identity/DeviceIdentifier.hpp)

## Direct includes

- `array`
- `cstddef`
- `cstdint`

## Documented declarations

### `DeviceIdentifier`

**Classification:** PUBLIC API

Permanent transport-independent 128-bit identity of one device.

The value identifies the device itself. It is not a transport address,
runtime incarnation, authentication credential, product classification,
or subsystem-specific identifier. The all-zero representation is
reserved as Invalid/Unspecified.

```cpp
class DeviceIdentifier final
```

### `uint8_t`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Complete fixed-width identifier representation.

```cpp
std::array<std::uint8_t, 16U> _bytes{};
```

### `Size`

**Classification:** PUBLIC API · source access: `public`

Identifier width in bytes.

```cpp
static constexpr std::size_t Size = 16U;
```

### `Storage`

**Classification:** PUBLIC API · source access: `public`

Complete fixed-width identifier storage type.

```cpp
using Storage = std::array<std::uint8_t, Size>;
```

### `DeviceIdentifier`

**Classification:** PUBLIC API · source access: `public`

Creates the Invalid/Unspecified all-zero identifier.

```cpp
constexpr DeviceIdentifier() noexcept = default;
```

### `DeviceIdentifier`

**Classification:** PUBLIC API · source access: `public`

Creates an identifier from its complete 128-bit representation.

- **Parameter `bytes`:** Exact identifier bytes in canonical storage order.

```cpp
constexpr explicit DeviceIdentifier(const Storage& bytes) noexcept :
            _bytes(bytes) {}
```

### `Bytes`

**Classification:** PUBLIC API · source access: `public`

Returns the complete immutable identifier representation without copying it.

```cpp
constexpr const Storage& Bytes() const noexcept
```

### `IsZero`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the identifier is the reserved all-zero value.

```cpp
constexpr bool IsZero() const noexcept
```

### `IsValid`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the identifier contains a valid non-zero device identity.

```cpp
constexpr bool IsValid() const noexcept
```

### `bool`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the identifier contains a valid non-zero device identity.

```cpp
constexpr explicit operator bool() const noexcept
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Compares identifiers by exact byte equality.

- **Parameter `other`:** Identifier to compare with this value.

```cpp
constexpr bool operator ==(const DeviceIdentifier& other) const noexcept
```

### `constexpr bool operator !=(const DeviceIdentifier& other) const noexce`

**Classification:** PUBLIC API · source access: `public`

Compares identifiers for inequality.

- **Parameter `other`:** Identifier to compare with this value.

```cpp
constexpr bool operator !=(const DeviceIdentifier& other) const noexcept
```

### `constexpr bool operator <(const DeviceIdentifier& other) const noexcep`

**Classification:** PUBLIC API · source access: `public`

Provides deterministic lexicographic ordering by canonical identifier bytes.

- **Parameter `other`:** Identifier to compare with this value.

```cpp
constexpr bool operator <(const DeviceIdentifier& other) const noexcept
```

