# src/composition/FlagProperty.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `9964ab2820cc5d290de69674000e011faaa684d9`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/9964ab2820cc5d290de69674000e011faaa684d9/src/composition/FlagProperty.hpp)

## Direct includes

- `cstdint`
- `type_traits`
- `../flags/FlagSet.hpp`
- `Property.hpp`

## Documented declarations

### `TCapability`

**Classification:** PUBLIC API

Base declaration for a compile-time capability Property whose value is a finite set of typed flags.

Flag values use the same zero-based bit-position contract as ESPressio::System::FlagSet.
The Property's compile-time value remains the selected unsigned storage type so it integrates
with the existing PropertyValue and PropertySet machinery without changing their contracts.

- **Template parameter `TCapability`:** Capability whose compile-time characteristics include this flag set.
- **Template parameter `TFlag`:** Enum type defining the finite flag domain and zero-based bit positions.
- **Template parameter `TStorage`:** Unsigned integral type used as the complete compile-time flag representation.

```cpp
template<class TCapability, class TFlag, class TStorage = std::uint32_t>
    struct FlagProperty : Property<TCapability, TStorage>
```

### `FlagType`

**Classification:** PUBLIC API · source access: `public`

Enum type defining the Property's finite flag domain.

```cpp
using FlagType = TFlag;
```

### `StorageType`

**Classification:** PUBLIC API · source access: `public`

Unsigned integral type used to store the complete Property value.

```cpp
using StorageType = TStorage;
```

### `FlagSetType`

**Classification:** PUBLIC API · source access: `public`

Strongly typed flag-set type used to construct and inspect the Property value.

```cpp
using FlagSetType = ESPressio::System::FlagSet<TFlag, TStorage>;
```

### `TProperty`

**Classification:** PUBLIC API

Default flag-property metadata for types that are not concrete FlagProperty declarations.

- **Template parameter `TProperty`:** Type being inspected for flag-property metadata.
- **Template parameter `TEnable`:** SFINAE helper used when the inspected type exposes flag-property aliases.

```cpp
template<class TProperty, class TEnable = void>
        struct FlagPropertyTraits
```

### `CapabilityType`

**Classification:** PUBLIC API · source access: `public`

Capability associated with the inspected type when one exists.

```cpp
using CapabilityType = void;
```

### `FlagType`

**Classification:** PUBLIC API · source access: `public`

Flag enum associated with the inspected type when one exists.

```cpp
using FlagType = void;
```

### `StorageType`

**Classification:** PUBLIC API · source access: `public`

Storage type associated with the inspected type when one exists.

```cpp
using StorageType = void;
```

### `FlagSetType`

**Classification:** PUBLIC API · source access: `public`

Strongly typed flag-set type associated with the inspected type when one exists.

```cpp
using FlagSetType = void;
```

### `IsValid`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the inspected type is a valid concrete FlagProperty.

```cpp
static constexpr bool IsValid = false;
```

### `TProperty`

**Classification:** PUBLIC API

Extracts flag-property metadata from a concrete type derived from FlagProperty.

- **Template parameter `TProperty`:** Type being inspected for flag-property metadata.

```cpp
template<class TProperty>
        struct FlagPropertyTraits<
            TProperty,
```

### `CapabilityType`

**Classification:** PUBLIC API · source access: `public`

Capability associated with the inspected flag property.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `FlagType`

**Classification:** PUBLIC API · source access: `public`

Flag enum associated with the inspected flag property.

```cpp
using FlagType = typename TProperty::FlagType;
```

### `StorageType`

**Classification:** PUBLIC API · source access: `public`

Storage type associated with the inspected flag property.

```cpp
using StorageType = typename TProperty::StorageType;
```

### `FlagSetType`

**Classification:** PUBLIC API · source access: `public`

Strongly typed flag-set type associated with the inspected flag property.

```cpp
using FlagSetType = typename TProperty::FlagSetType;
```

### `IsValid`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the inspected type is a concrete FlagProperty derived from the matching declaration.

```cpp
static constexpr bool IsValid =
                IsPropertyV<TProperty> &&
                std::is_base_of_v<
                    FlagProperty<
                        CapabilityType,
```

### `TProperty`

**Classification:** PUBLIC API

Indicates whether every supplied compile-time flag belongs to the flag enum declared by a Property.

- **Template parameter `TProperty`:** Concrete FlagProperty whose flag domain is being validated.
- **Template parameter `TFlags`:** Compile-time flag values being validated.

```cpp
template<class TProperty, auto... TFlags>
        consteval bool FlagArgumentsUsePropertyDomain() noexcept
```

### `TProperty`

**Classification:** PUBLIC API

Indicates whether every supplied compile-time flag fits the storage selected by a Property.

- **Template parameter `TProperty`:** Concrete FlagProperty whose storage capacity is being validated.
- **Template parameter `TFlags`:** Compile-time flag values being validated.

```cpp
template<class TProperty, auto... TFlags>
        consteval bool FlagArgumentsAreRepresentable() noexcept
```

### `TProperty`

**Classification:** PUBLIC API

Builds the raw compile-time storage value represented by a typed flag pack.

- **Template parameter `TProperty`:** Concrete FlagProperty whose storage value is being produced.
- **Template parameter `TFlags`:** Compile-time flags to set in the produced value.

```cpp
template<class TProperty, auto... TFlags>
        consteval auto FlagPropertyStorageValue() noexcept
```

### `TProperty`

**Classification:** PUBLIC API

Indicates whether a type represents a concrete flag-valued Composition Property.

- **Template parameter `TProperty`:** Type being inspected.

```cpp
template<class TProperty>
    inline constexpr bool IsFlagPropertyV = Detail::FlagPropertyTraits<TProperty>::IsValid;
```

### `TProperty`

**Classification:** PUBLIC API

Associates zero or more typed compile-time flags with a concrete FlagProperty.

This declaration is compatible with the existing PropertyValue/PropertySet machinery while
preventing callers from hand-maintaining raw masks for normal flag-property advertisement.

- **Template parameter `TProperty`:** Concrete FlagProperty receiving the advertised flags.
- **Template parameter `TFlags`:** Typed enum values to advertise as set; an empty pack advertises no flags.

```cpp
template<class TProperty, auto... TFlags>
    struct FlagPropertyValue : PropertyValue<
        TProperty,
```

### `TProperty`

**Classification:** PUBLIC API

Requires every supplied flag to be present in one advertised FlagProperty value.

- **Template parameter `TProperty`:** Concrete FlagProperty evaluated by this constraint.
- **Template parameter `TFlags`:** One or more typed flags which must all be set.

```cpp
template<class TProperty, auto... TFlags>
    struct HasAllFlags
```

### `ConstraintTag`

**Classification:** PUBLIC API · source access: `public`

Marker used to identify this declaration as a Property constraint.

```cpp
using ConstraintTag = void;
```

### `PropertyType`

**Classification:** PUBLIC API · source access: `public`

FlagProperty evaluated by this constraint.

```cpp
using PropertyType = TProperty;
```

### `CapabilityType`

**Classification:** PUBLIC API · source access: `public`

Capability to which the evaluated FlagProperty belongs.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `TPropertySet`

**Classification:** PUBLIC API · source access: `public`

Determines whether the supplied PropertySet contains every required flag.

- **Template parameter `TPropertySet`:** PropertySet advertised by a candidate capability Offer.

```cpp
template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept
```

### `TProperty`

**Classification:** PUBLIC API

Requires at least one supplied flag to be present in one advertised FlagProperty value.

- **Template parameter `TProperty`:** Concrete FlagProperty evaluated by this constraint.
- **Template parameter `TFlags`:** One or more typed flags of which at least one must be set.

```cpp
template<class TProperty, auto... TFlags>
    struct HasAnyFlags
```

### `ConstraintTag`

**Classification:** PUBLIC API · source access: `public`

Marker used to identify this declaration as a Property constraint.

```cpp
using ConstraintTag = void;
```

### `PropertyType`

**Classification:** PUBLIC API · source access: `public`

FlagProperty evaluated by this constraint.

```cpp
using PropertyType = TProperty;
```

### `CapabilityType`

**Classification:** PUBLIC API · source access: `public`

Capability to which the evaluated FlagProperty belongs.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `TPropertySet`

**Classification:** PUBLIC API · source access: `public`

Determines whether the supplied PropertySet contains at least one requested flag.

- **Template parameter `TPropertySet`:** PropertySet advertised by a candidate capability Offer.

```cpp
template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept
```

### `TProperty`

**Classification:** PUBLIC API

Requires every supplied flag to be absent from one advertised FlagProperty value.

- **Template parameter `TProperty`:** Concrete FlagProperty evaluated by this constraint.
- **Template parameter `TFlags`:** One or more typed flags which must all be clear.

```cpp
template<class TProperty, auto... TFlags>
    struct HasNoFlags
```

### `ConstraintTag`

**Classification:** PUBLIC API · source access: `public`

Marker used to identify this declaration as a Property constraint.

```cpp
using ConstraintTag = void;
```

### `PropertyType`

**Classification:** PUBLIC API · source access: `public`

FlagProperty evaluated by this constraint.

```cpp
using PropertyType = TProperty;
```

### `CapabilityType`

**Classification:** PUBLIC API · source access: `public`

Capability to which the evaluated FlagProperty belongs.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `TPropertySet`

**Classification:** PUBLIC API · source access: `public`

Determines whether the supplied PropertySet contains none of the prohibited flags.

- **Template parameter `TPropertySet`:** PropertySet advertised by a candidate capability Offer.

```cpp
template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept
```

