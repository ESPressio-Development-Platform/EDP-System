# src/composition/Provider.hpp

**Primary classification:** PUBLIC COMPOSITION API

**Source baseline:** `9964ab2820cc5d290de69674000e011faaa684d9`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/9964ab2820cc5d290de69674000e011faaa684d9/src/composition/Provider.hpp)

## Direct includes

- `cstddef`
- `type_traits`
- `Contract.hpp`

## Documented declarations

### `TypeList`

**Classification:** PUBLIC COMPOSITION API

Stores a temporary compile-time type list while Offer characteristics are partitioned.

```cpp
template<class... TTypes>
        struct TypeList {};
```

### `TList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Appends one type to a compile-time type list.

```cpp
template<class TList, class TType>
        struct AppendType;
```

### `TType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Appends one type to the end of a compile-time type list.

```cpp
template<class... TTypes, class TType>
        struct AppendType<TypeList<TTypes...>, TType>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Type list containing the previous entries followed by the appended type.

```cpp
using Type = TypeList<TTypes..., TType>;
```

### `TAccumulatedProperties`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Collects only PropertyValue declarations from an Offer characteristic pack.

```cpp
template<class TAccumulatedProperties, class... TCharacteristics>
        struct CollectPropertyValues;
```

### `TAccumulatedProperties`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Completes PropertyValue collection when no Offer characteristics remain.

```cpp
template<class TAccumulatedProperties>
        struct CollectPropertyValues<TAccumulatedProperties>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Accumulated PropertyValue type list.

```cpp
using Type = TAccumulatedProperties;
```

### `TAccumulatedProperties`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Inspects one Offer characteristic and continues PropertyValue collection.

```cpp
template<class TAccumulatedProperties, class TFirstCharacteristic, class... TRestCharacteristics>
        struct CollectPropertyValues<TAccumulatedProperties, TFirstCharacteristic, TRestCharacteristics...>
```

### `NextProperties`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Next PropertyValue list after conditionally appending the current characteristic.

```cpp
using NextProperties = std::conditional_t<
                PropertyValueTraits<TFirstCharacteristic>::IsValid,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Final collected PropertyValue list.

```cpp
using Type = typename CollectPropertyValues<NextProperties, TRestCharacteristics...>::Type;
```

### `TAccumulatedAttributes`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Collects only Attribute declarations from an Offer characteristic pack.

```cpp
template<class TAccumulatedAttributes, class... TCharacteristics>
        struct CollectAttributes;
```

### `TAccumulatedAttributes`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Completes Attribute collection when no Offer characteristics remain.

```cpp
template<class TAccumulatedAttributes>
        struct CollectAttributes<TAccumulatedAttributes>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Accumulated Attribute type list.

```cpp
using Type = TAccumulatedAttributes;
```

### `TAccumulatedAttributes`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Inspects one Offer characteristic and continues Attribute collection.

```cpp
template<class TAccumulatedAttributes, class TFirstCharacteristic, class... TRestCharacteristics>
        struct CollectAttributes<TAccumulatedAttributes, TFirstCharacteristic, TRestCharacteristics...>
```

### `NextAttributes`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Next Attribute list after conditionally appending the current characteristic.

```cpp
using NextAttributes = std::conditional_t<
                IsAttributeV<TFirstCharacteristic>,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Final collected Attribute list.

```cpp
using Type = typename CollectAttributes<NextAttributes, TRestCharacteristics...>::Type;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Converts one compile-time type list into a PropertySet for a specific capability.

```cpp
template<class TCapability, class TPropertyList>
        struct MakePropertySet;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Constructs a PropertySet from the collected PropertyValue declarations.

```cpp
template<class TCapability, class... TPropertyValues>
        struct MakePropertySet<TCapability, TypeList<TPropertyValues...>>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

PropertySet containing the collected PropertyValue declarations.

```cpp
using Type = PropertySet<TCapability, TPropertyValues...>;
```

### `TAttributeList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Converts one compile-time type list into an AttributeSet.

```cpp
template<class TAttributeList>
        struct MakeAttributeSet;
```

### `MakeAttributeSet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Constructs an AttributeSet from the collected Attribute declarations.

```cpp
template<class... TAttributes>
        struct MakeAttributeSet<TypeList<TAttributes...>>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

AttributeSet containing the collected Attribute declarations.

```cpp
using Type = AttributeSet<TAttributes...>;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Determines whether one valid PropertyValue characteristic belongs to the supplied capability.

```cpp
template<class TCapability, class TCharacteristic, bool TIsPropertyValue = PropertyValueTraits<TCharacteristic>::IsValid>
        struct IsOfferPropertyFor : std::false_type {};
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Compares the capability carried by a valid PropertyValue with the supplied capability.

```cpp
template<class TCapability, class TCharacteristic>
        struct IsOfferPropertyFor<TCapability, TCharacteristic, true> : std::bool_constant<
            std::is_same_v<typename TCharacteristic::CapabilityType, TCapability>
        > {};
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether one Offer characteristic is valid for the supplied capability.

```cpp
template<class TCapability, class TCharacteristic>
        inline constexpr bool IsOfferCharacteristicForV =
            IsAttributeV<TCharacteristic> ||
            IsOfferPropertyFor<TCapability, TCharacteristic>::value;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Declares one capability and the compile-time properties and Attributes advertised for that capability by a provider.

```cpp
template<class TCapability, class... TCharacteristics>
    struct Offer
```

### `OfferTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker used to identify Offer declarations during compile-time inspection.

```cpp
using OfferTag = void;
```

### `CapabilityType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Capability supplied by this offer.

```cpp
using CapabilityType = TCapability;
```

### `CompositionDomain`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Domain inherited from the supplied capability.

```cpp
using CompositionDomain = typename Detail::CapabilityTraits<TCapability>::DomainType;
```

### `Properties`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Compile-time properties advertised for the supplied capability.

```cpp
using Properties = typename Detail::MakePropertySet<
            TCapability,
```

### `Attributes`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Open-ended compile-time Attributes advertised for the supplied capability.

```cpp
using Attributes = typename Detail::MakeAttributeSet<
            typename Detail::CollectAttributes<Detail::TypeList<>, TCharacteristics...>::Type
        >::Type;
```

### `TOffer`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Default offer metadata for types that are not Offer declarations.

```cpp
template<class TOffer, class = void>
        struct OfferTraits
```

### `IsValid`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the inspected type is a valid Offer declaration.

```cpp
static constexpr bool IsValid = false;
```

### `TOffer`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Extracts metadata from an Offer declaration.

```cpp
template<class TOffer>
        struct OfferTraits<
            TOffer,
```

### `IsValid`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the inspected offer references a valid capability.

```cpp
static constexpr bool IsValid = IsCapabilityV<typename TOffer::CapabilityType>;
```

### `UniqueOffers`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether every offered capability in an offer pack is unique.

```cpp
template<class... TOffers>
        struct UniqueOffers;
```

### `UniqueOffers`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Empty offer packs are unique.

```cpp
template<>
        struct UniqueOffers<> : std::true_type {};
```

### `TFirstOffer`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Checks the first offer against the remaining offers and continues recursively.

```cpp
template<class TFirstOffer, class... TRestOffers>
        struct UniqueOffers<TFirstOffer, TRestOffers...> : std::bool_constant<
            ((!std::is_same_v<typename TFirstOffer::CapabilityType, typename TRestOffers::CapabilityType>) && ...) &&
            UniqueOffers<TRestOffers...>::value
        > {};
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Locates an offer by its capability type within a compile-time offer pack.

```cpp
template<class TCapability, class... TOffers>
        struct FindOffer;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Represents an unsuccessful offer lookup.

```cpp
template<class TCapability>
        struct FindOffer<TCapability>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Type returned when no matching offer exists.

```cpp
using Type = void;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Continues an offer lookup until a matching capability is found.

```cpp
template<class TCapability, class TFirstOffer, class... TRestOffers>
        struct FindOffer<TCapability, TFirstOffer, TRestOffers...>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Matching Offer declaration, or the result of searching the remaining declarations.

```cpp
using Type = std::conditional_t<
                std::is_same_v<TCapability, typename TFirstOffer::CapabilityType>,
```

### `Offers`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Groups the capability Offers supplied by one provider.

- **Template parameter `TOffers`:** Capability Offer declarations supplied by the provider.

```cpp
template<class... TOffers>
    struct Offers
```

### `OffersTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker used to identify Offers declarations during compile-time inspection.

```cpp
using OffersTag = void;
```

### `Count`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Number of capability Offers contained in this declaration.

```cpp
static constexpr std::size_t Count = sizeof...(TOffers);
```

### `TDomain`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether every offered capability belongs to the specified composition Domain.

```cpp
template<class TDomain>
        static constexpr bool IsForDomain =
            (std::is_same_v<typename TOffers::CompositionDomain, TDomain> && ...);
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether this declaration supplies the specified capability.

```cpp
template<class TCapability>
        static constexpr bool Contains =
            (std::is_same_v<TCapability, typename TOffers::CapabilityType> || ...);
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Resolves one capability Offer from this declaration.

```cpp
template<class TCapability>
        struct ResolveOffer
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Offer declaration associated with the requested capability.

```cpp
using Type = typename Detail::FindOffer<TCapability, TOffers...>::Type;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns the complete Offer declaration associated with the specified capability.

```cpp
template<class TCapability>
        using OfferFor = typename ResolveOffer<TCapability>::Type;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns the compile-time Property set advertised for the specified capability.

```cpp
template<class TCapability>
        using PropertiesFor = typename OfferFor<TCapability>::Properties;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns the open-ended Attribute set advertised for the specified capability.

```cpp
template<class TCapability>
        using AttributesFor = typename OfferFor<TCapability>::Attributes;
```

### `TDomain`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Declares the compile-time Offers and consumer Contract of one concrete provider Type.

- **Template parameter `TDomain`:** Domain owning this provider.
- **Template parameter `TOffers`:** Capabilities and characteristics supplied by this provider.
- **Template parameter `TContract`:** Consolidated consumer Contract of this provider.

```cpp
template<
        class TDomain,
```

### `ProviderDeclarationTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker used to identify provider declarations during compile-time inspection.

```cpp
using ProviderDeclarationTag = void;
```

### `CompositionDomain`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Domain to which this provider belongs.

```cpp
using CompositionDomain = TDomain;
```

### `CompositionOffers`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Canonical Offer collection supplied by this provider.

```cpp
using CompositionOffers = TOffers;
```

### `CompositionContract`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Consolidated consumer Contract supplied by this provider.

```cpp
using CompositionContract = TContract;
```

