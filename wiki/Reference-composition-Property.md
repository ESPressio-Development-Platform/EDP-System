# src/composition/Property.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `9964ab2820cc5d290de69674000e011faaa684d9`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/9964ab2820cc5d290de69674000e011faaa684d9/src/composition/Property.hpp)

## Direct includes

- `cstddef`
- `type_traits`
- `Capability.hpp`

## Documented declarations

### `TCapability`

**Classification:** PUBLIC API

Base declaration for a compile-time property associated with a capability.

```cpp
template<class TCapability, class TValue>
    struct Property
```

### `CapabilityType`

**Classification:** PUBLIC API · source access: `public`

Capability to which this property belongs.

```cpp
using CapabilityType = TCapability;
```

### `CompositionDomain`

**Classification:** PUBLIC API · source access: `public`

Domain inherited from the property's capability.

```cpp
using CompositionDomain = typename Detail::CapabilityTraits<TCapability>::DomainType;
```

### `ValueType`

**Classification:** PUBLIC API · source access: `public`

Value type used by this property.

```cpp
using ValueType = TValue;
```

### `TProperty`

**Classification:** PUBLIC API

Default property metadata for types that are not composition properties.

```cpp
template<class TProperty, class = void>
        struct PropertyTraits
```

### `CapabilityType`

**Classification:** PUBLIC API · source access: `public`

Capability associated with the inspected type when one exists.

```cpp
using CapabilityType = void;
```

### `DomainType`

**Classification:** PUBLIC API · source access: `public`

Domain associated with the inspected type when one exists.

```cpp
using DomainType = void;
```

### `ValueType`

**Classification:** PUBLIC API · source access: `public`

Value type associated with the inspected type when one exists.

```cpp
using ValueType = void;
```

### `IsValid`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the inspected type is a valid concrete property.

```cpp
static constexpr bool IsValid = false;
```

### `TProperty`

**Classification:** PUBLIC API

Extracts property metadata from types that declare the required property aliases.

```cpp
template<class TProperty>
        struct PropertyTraits<
            TProperty,
```

### `CapabilityType`

**Classification:** PUBLIC API · source access: `public`

Capability associated with the inspected property.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `DomainType`

**Classification:** PUBLIC API · source access: `public`

Domain associated with the inspected property.

```cpp
using DomainType = typename TProperty::CompositionDomain;
```

### `ValueType`

**Classification:** PUBLIC API · source access: `public`

Value type associated with the inspected property.

```cpp
using ValueType = typename TProperty::ValueType;
```

### `IsValid`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the inspected type is a concrete property derived from the matching Property declaration.

```cpp
static constexpr bool IsValid =
                IsCapabilityV<CapabilityType> &&
                std::is_same_v<typename CapabilityTraits<CapabilityType>::DomainType, DomainType> &&
                std::is_base_of_v<Property<CapabilityType, ValueType>, TProperty> &&
                !std::is_same_v<Property<CapabilityType, ValueType>, TProperty>;
```

### `TPropertyValue`

**Classification:** PUBLIC API

Default property-value metadata for types that are not PropertyValue declarations.

```cpp
template<class TPropertyValue, class = void>
        struct PropertyValueTraits
```

### `IsValid`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the inspected type is a valid PropertyValue declaration.

```cpp
static constexpr bool IsValid = false;
```

### `TPropertyValue`

**Classification:** PUBLIC API

Extracts metadata from a PropertyValue declaration.

```cpp
template<class TPropertyValue>
        struct PropertyValueTraits<
            TPropertyValue,
```

### `IsValid`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the inspected type contains a valid property type.

```cpp
static constexpr bool IsValid = PropertyTraits<typename TPropertyValue::PropertyType>::IsValid;
```

### `TProperty`

**Classification:** PUBLIC API

Locates a property value by its property type within a compile-time property-value pack.

```cpp
template<class TProperty, class... TPropertyValues>
        struct FindPropertyValue;
```

### `TProperty`

**Classification:** PUBLIC API

Represents an unsuccessful property-value lookup.

```cpp
template<class TProperty>
        struct FindPropertyValue<TProperty>
```

### `Type`

**Classification:** PUBLIC API · source access: `public`

Type returned when no matching property value exists.

```cpp
using Type = void;
```

### `TProperty`

**Classification:** PUBLIC API

Continues a property-value lookup until a matching property type is found.

```cpp
template<class TProperty, class TFirstPropertyValue, class... TRestPropertyValues>
        struct FindPropertyValue<TProperty, TFirstPropertyValue, TRestPropertyValues...>
```

### `Type`

**Classification:** PUBLIC API · source access: `public`

Matching PropertyValue declaration, or the result of searching the remaining declarations.

```cpp
using Type = std::conditional_t<
                std::is_same_v<TProperty, typename TFirstPropertyValue::PropertyType>,
```

### `UniquePropertyValues`

**Classification:** PUBLIC API

Indicates whether every property type in a PropertyValue pack is unique.

```cpp
template<class... TPropertyValues>
        struct UniquePropertyValues;
```

### `UniquePropertyValues`

**Classification:** PUBLIC API

Empty property-value packs are unique.

```cpp
template<>
        struct UniquePropertyValues<> : std::true_type {};
```

### `TFirstPropertyValue`

**Classification:** PUBLIC API · source access: `public`

Checks the first property value against the remaining property values and continues recursively.

```cpp
template<class TFirstPropertyValue, class... TRestPropertyValues>
        struct UniquePropertyValues<TFirstPropertyValue, TRestPropertyValues...> : std::bool_constant<
            ((!std::is_same_v<typename TFirstPropertyValue::PropertyType, typename TRestPropertyValues::PropertyType>) && ...) &&
            UniquePropertyValues<TRestPropertyValues...>::value
        > {};
```

### `TProperty`

**Classification:** PUBLIC API · source access: `public`

Indicates whether a type represents a concrete composition property.

```cpp
template<class TProperty>
    inline constexpr bool IsPropertyV = Detail::PropertyTraits<TProperty>::IsValid;
```

### `TCapability`

**Classification:** PUBLIC API · source access: `public`

Indicates whether a property belongs to the specified capability.

```cpp
template<class TCapability, class TProperty>
    inline constexpr bool IsPropertyForV =
        IsPropertyV<TProperty> &&
        std::is_same_v<typename Detail::PropertyTraits<TProperty>::CapabilityType, TCapability>;
```

### `TProperty`

**Classification:** PUBLIC API · source access: `public`

Associates a compile-time value with a concrete capability property.

```cpp
template<class TProperty, auto TPropertyValue>
    struct PropertyValue
```

### `PropertyValueTag`

**Classification:** PUBLIC API · source access: `public`

Marker used to identify PropertyValue declarations during compile-time inspection.

```cpp
using PropertyValueTag = void;
```

### `PropertyType`

**Classification:** PUBLIC API · source access: `public`

Property represented by this value.

```cpp
using PropertyType = TProperty;
```

### `CapabilityType`

**Classification:** PUBLIC API · source access: `public`

Capability to which the represented property belongs.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `CompositionDomain`

**Classification:** PUBLIC API · source access: `public`

Domain to which the represented property belongs.

```cpp
using CompositionDomain = typename TProperty::CompositionDomain;
```

### `ValueType`

**Classification:** PUBLIC API · source access: `public`

Value type declared by the represented property.

```cpp
using ValueType = typename TProperty::ValueType;
```

### `Value`

**Classification:** PUBLIC API · source access: `public`

Compile-time value advertised for the represented property.

```cpp
static constexpr ValueType Value = static_cast<ValueType>(TPropertyValue);
```

### `TCapability`

**Classification:** PUBLIC API · source access: `public`

Stores the compile-time property values advertised for one capability offer.

```cpp
template<class TCapability, class... TPropertyValues>
    struct PropertySet
```

### `CapabilityType`

**Classification:** PUBLIC API · source access: `public`

Capability described by this set of property values.

```cpp
using CapabilityType = TCapability;
```

### `CompositionDomain`

**Classification:** PUBLIC API · source access: `public`

Domain inherited from the described capability.

```cpp
using CompositionDomain = typename Detail::CapabilityTraits<TCapability>::DomainType;
```

### `Count`

**Classification:** PUBLIC API · source access: `public`

Number of property values contained in this set.

```cpp
static constexpr std::size_t Count = sizeof...(TPropertyValues);
```

### `TProperty`

**Classification:** PUBLIC API · source access: `public`

Indicates whether this set contains a value for the specified property.

```cpp
template<class TProperty>
        static constexpr bool Contains = !std::is_void_v<typename Detail::FindPropertyValue<TProperty, TPropertyValues...>::Type>;
```

### `TProperty`

**Classification:** PUBLIC API · source access: `public`

Resolves one property value from this set.

```cpp
template<class TProperty>
        struct Resolve
```

### `Entry`

**Classification:** PUBLIC API · source access: `public`

PropertyValue declaration associated with the requested property.

```cpp
using Entry = typename Detail::FindPropertyValue<TProperty, TPropertyValues...>::Type;
```

### `Value`

**Classification:** PUBLIC API · source access: `public`

Compile-time value associated with the requested property.

```cpp
static constexpr typename TProperty::ValueType Value = Entry::Value;
```

### `TProperty`

**Classification:** PUBLIC API · source access: `public`

Returns the compile-time value associated with the specified property.

```cpp
template<class TProperty>
        static constexpr typename TProperty::ValueType Value = Resolve<TProperty>::Value;
```

