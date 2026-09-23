# src/composition/Capability.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `9964ab2820cc5d290de69674000e011faaa684d9`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/9964ab2820cc5d290de69674000e011faaa684d9/src/composition/Capability.hpp)

## Direct includes

- `type_traits`

## Documented declarations

### `Domain`

**Classification:** PUBLIC API

Marker base for a compile-time composition domain.

```cpp
struct Domain {};
```

### `TDomain`

**Classification:** PUBLIC API · source access: `public`

Indicates whether a type represents a concrete composition domain.

```cpp
template<class TDomain>
    inline constexpr bool IsDomainV = std::is_base_of_v<Domain, TDomain> && !std::is_same_v<Domain, TDomain>;
```

### `TDomain`

**Classification:** PUBLIC API · source access: `public`

Base declaration for a capability that may have no more than one provider in a composition.

```cpp
template<class TDomain>
    struct ExclusiveCapability
```

### `CompositionDomain`

**Classification:** PUBLIC API · source access: `public`

Domain to which this capability belongs.

```cpp
using CompositionDomain = TDomain;
```

### `TDomain`

**Classification:** PUBLIC API · source access: `public`

Base declaration for a capability that may have multiple providers in a composition.

```cpp
template<class TDomain>
    struct SharedCapability
```

### `CompositionDomain`

**Classification:** PUBLIC API · source access: `public`

Domain to which this capability belongs.

```cpp
using CompositionDomain = TDomain;
```

### `TCapability`

**Classification:** PUBLIC API · source access: `public`

Default capability metadata for types that are not composition capabilities.

```cpp
template<class TCapability, class = void>
        struct CapabilityTraits
```

### `DomainType`

**Classification:** PUBLIC API · source access: `public`

Domain associated with the inspected type when one exists.

```cpp
using DomainType = void;
```

### `IsValid`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the inspected type is a valid concrete capability.

```cpp
static constexpr bool IsValid = false;
```

### `IsExclusive`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the inspected capability is exclusive.

```cpp
static constexpr bool IsExclusive = false;
```

### `IsShared`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the inspected capability is shared.

```cpp
static constexpr bool IsShared = false;
```

### `TCapability`

**Classification:** PUBLIC API · source access: `public`

Extracts capability metadata from types that declare a composition domain.

```cpp
template<class TCapability>
        struct CapabilityTraits<TCapability, std::void_t<typename TCapability::CompositionDomain>>
```

### `DomainType`

**Classification:** PUBLIC API · source access: `public`

Domain associated with the inspected capability.

```cpp
using DomainType = typename TCapability::CompositionDomain;
```

### `IsExclusive`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the inspected type derives from the exclusive capability declaration for its domain.

```cpp
static constexpr bool IsExclusive =
                IsDomainV<DomainType> &&
                std::is_base_of_v<ExclusiveCapability<DomainType>, TCapability> &&
                !std::is_same_v<ExclusiveCapability<DomainType>, TCapability>;
```

### `IsShared`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the inspected type derives from the shared capability declaration for its domain.

```cpp
static constexpr bool IsShared =
                IsDomainV<DomainType> &&
                std::is_base_of_v<SharedCapability<DomainType>, TCapability> &&
                !std::is_same_v<SharedCapability<DomainType>, TCapability>;
```

### `IsValid`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the inspected type is a valid concrete capability.

```cpp
static constexpr bool IsValid = IsExclusive || IsShared;
```

### `TCapability`

**Classification:** PUBLIC API · source access: `public`

Indicates whether a type represents a concrete composition capability.

```cpp
template<class TCapability>
    inline constexpr bool IsCapabilityV = Detail::CapabilityTraits<TCapability>::IsValid;
```

### `TDomain`

**Classification:** PUBLIC API · source access: `public`

Indicates whether a capability belongs to the specified composition domain.

```cpp
template<class TDomain, class TCapability>
    inline constexpr bool IsCapabilityForV =
        IsCapabilityV<TCapability> &&
        std::is_same_v<typename Detail::CapabilityTraits<TCapability>::DomainType, TDomain>;
```

### `TDomain`

**Classification:** PUBLIC API · source access: `public`

Indicates whether a capability is exclusive within the specified composition domain.

```cpp
template<class TDomain, class TCapability>
    inline constexpr bool IsExclusiveCapabilityForV =
        IsCapabilityForV<TDomain, TCapability> &&
        Detail::CapabilityTraits<TCapability>::IsExclusive;
```

### `TDomain`

**Classification:** PUBLIC API · source access: `public`

Indicates whether a capability is shared within the specified composition domain.

```cpp
template<class TDomain, class TCapability>
    inline constexpr bool IsSharedCapabilityForV =
        IsCapabilityForV<TDomain, TCapability> &&
        Detail::CapabilityTraits<TCapability>::IsShared;
```

