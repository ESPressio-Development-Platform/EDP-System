# src/composition/Attribute.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `9964ab2820cc5d290de69674000e011faaa684d9`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/9964ab2820cc5d290de69674000e011faaa684d9/src/composition/Attribute.hpp)

## Direct includes

- `cstddef`
- `type_traits`

## Documented declarations

### `FixedString`

**Classification:** PUBLIC API

Stores one structural compile-time string value suitable for C++20 non-type template parameters.

```cpp
template<std::size_t TSize>
    struct FixedString
```

### `char Characters[TSize];`

**Classification:** PUBLIC API · source access: `public`

Character data including the terminating null character supplied by a string literal.

```cpp
char Characters[TSize];
```

### `FixedString`

**Classification:** PUBLIC API · source access: `public`

Copies a string literal into this structural compile-time value.

```cpp
constexpr FixedString(const char (&value)[TSize]) noexcept : Characters{}
```

### `Size`

**Classification:** PUBLIC API · source access: `public`

Number of characters including the terminating null character.

```cpp
static constexpr std::size_t Size = TSize;
```

### `Length`

**Classification:** PUBLIC API · source access: `public`

Number of meaningful characters excluding the terminating null character.

```cpp
static constexpr std::size_t Length = TSize > 0U ? TSize - 1U : 0U;
```

### `FixedString`

**Classification:** PUBLIC API

Deduces the structural compile-time string size from a string literal.

```cpp
template<std::size_t TSize>
    FixedString(const char (&)[TSize]) -> FixedString<TSize>;
```

### `TLeftSize`

**Classification:** PUBLIC API

Compares two structural compile-time strings for exact textual equality.

```cpp
template<std::size_t TLeftSize, std::size_t TRightSize>
    constexpr bool operator==(
        const FixedString<TLeftSize>& left,
        const FixedString<TRightSize>& right
    ) noexcept
```

### `TLeftSize`

**Classification:** PUBLIC API

Compares two structural compile-time strings for textual inequality.

```cpp
template<std::size_t TLeftSize, std::size_t TRightSize>
    constexpr bool operator!=(
        const FixedString<TLeftSize>& left,
        const FixedString<TRightSize>& right
    ) noexcept
```

### `TextValue`

**Classification:** PUBLIC API

Wraps one compile-time string so it can be used as a generic non-type template value.

```cpp
template<FixedString TValue>
    struct TextValue {};
```

### `template<FixedString TValue> inline constexpr TextValue<TValue> Text{}`

**Classification:** PUBLIC API · source access: `public`

Creates a generic compile-time text value usable wherever an auto non-type template parameter is accepted.

```cpp
template<FixedString TValue>
    inline constexpr TextValue<TValue> Text{};
```

### `TLeftValue`

**Classification:** PUBLIC API · source access: `public`

Compares two compile-time text wrappers for exact textual equality.

```cpp
template<FixedString TLeftValue, FixedString TRightValue>
    constexpr bool operator==(
        TextValue<TLeftValue>,
        TextValue<TRightValue>
    ) noexcept
```

### `TLeftValue`

**Classification:** PUBLIC API · source access: `public`

Compares two compile-time text wrappers for textual inequality.

```cpp
template<FixedString TLeftValue, FixedString TRightValue>
    constexpr bool operator!=(
        TextValue<TLeftValue> left,
        TextValue<TRightValue> right
    ) noexcept
```

### `Attribute`

**Classification:** PUBLIC API · source access: `public`

Declares one open-ended compile-time descriptor attached to a capability Offer.

```cpp
template<FixedString TName, auto TValue>
    struct Attribute
```

### `AttributeTag`

**Classification:** PUBLIC API · source access: `public`

Marker used to identify Attribute declarations during compile-time inspection.

```cpp
using AttributeTag = void;
```

### `TextAttribute`

**Classification:** PUBLIC API · source access: `public`

Convenience alias for an Attribute whose value is compile-time text.

```cpp
template<FixedString TName, FixedString TValue>
    using TextAttribute = Attribute<TName, Text<TValue>>;
```

### `TAttribute`

**Classification:** PUBLIC API · source access: `public`

Default attribute metadata for types that are not Attribute declarations.

```cpp
template<class TAttribute, class = void>
        struct AttributeTraits
```

### `IsValid`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the inspected type is a valid Attribute declaration.

```cpp
static constexpr bool IsValid = false;
```

### `AttributeTraits`

**Classification:** PUBLIC API · source access: `public`

Extracts compile-time metadata from one concrete Attribute declaration.

```cpp
template<FixedString TName, auto TValue>
        struct AttributeTraits<
            Attribute<TName, TValue>,
```

### `IsValid`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the inspected type is a valid Attribute declaration.

```cpp
static constexpr bool IsValid = true;
```

### `Name`

**Classification:** PUBLIC API · source access: `public`

Open-ended compile-time Attribute name.

```cpp
static constexpr auto Name = TName;
```

### `Value`

**Classification:** PUBLIC API · source access: `public`

Compile-time Attribute value used only by Composition matching.

```cpp
static constexpr auto Value = TValue;
```

### `TLeftAttribute`

**Classification:** PUBLIC API · source access: `public`

Indicates whether two Attribute declarations use the same open-ended name.

```cpp
template<class TLeftAttribute, class TRightAttribute>
        inline constexpr bool SameAttributeNameV =
            AttributeTraits<TLeftAttribute>::Name == AttributeTraits<TRightAttribute>::Name;
```

### `UniqueAttributes`

**Classification:** PUBLIC API · source access: `public`

Indicates whether every Attribute name in one pack is unique.

```cpp
template<class... TAttributes>
        struct UniqueAttributes;
```

### `UniqueAttributes`

**Classification:** PUBLIC API · source access: `public`

Empty Attribute packs contain no duplicate names.

```cpp
template<>
        struct UniqueAttributes<> : std::true_type {};
```

### `TFirstAttribute`

**Classification:** PUBLIC API · source access: `public`

Checks the first Attribute name against the remaining declarations and continues recursively.

```cpp
template<class TFirstAttribute, class... TRestAttributes>
        struct UniqueAttributes<TFirstAttribute, TRestAttributes...> : std::bool_constant<
            ((!SameAttributeNameV<TFirstAttribute, TRestAttributes>) && ...) &&
            UniqueAttributes<TRestAttributes...>::value
        > {};
```

### `FindAttribute`

**Classification:** PUBLIC API · source access: `public`

Locates an Attribute by its open-ended name within a compile-time Attribute pack.

```cpp
template<FixedString TName, class... TAttributes>
        struct FindAttribute;
```

### `FindAttribute`

**Classification:** PUBLIC API · source access: `public`

Represents an unsuccessful Attribute lookup.

```cpp
template<FixedString TName>
        struct FindAttribute<TName>
```

### `Type`

**Classification:** PUBLIC API · source access: `public`

Type returned when no matching Attribute exists.

```cpp
using Type = void;
```

### `TFirstAttribute`

**Classification:** PUBLIC API · source access: `public`

Continues an Attribute lookup until a declaration with the requested name is found.

```cpp
template<FixedString TName, class TFirstAttribute, class... TRestAttributes>
        struct FindAttribute<TName, TFirstAttribute, TRestAttributes...>
```

### `Type`

**Classification:** PUBLIC API · source access: `public`

Matching Attribute declaration, or the result of searching the remaining declarations.

```cpp
using Type = std::conditional_t<
                AttributeTraits<TFirstAttribute>::Name == TName,
```

### `TAttribute`

**Classification:** PUBLIC API · source access: `public`

Indicates whether a type represents a valid open-ended compile-time Attribute.

```cpp
template<class TAttribute>
    inline constexpr bool IsAttributeV = Detail::AttributeTraits<TAttribute>::IsValid;
```

### `AttributeSet`

**Classification:** PUBLIC API · source access: `public`

Stores the compile-time Attributes advertised by one capability Offer.

```cpp
template<class... TAttributes>
    struct AttributeSet
```

### `Count`

**Classification:** PUBLIC API · source access: `public`

Number of Attributes contained in this set.

```cpp
static constexpr std::size_t Count = sizeof...(TAttributes);
```

### `Contains`

**Classification:** PUBLIC API · source access: `public`

Indicates whether this set contains an Attribute with the requested name.

```cpp
template<FixedString TName>
        static constexpr bool Contains = !std::is_void_v<typename Detail::FindAttribute<TName, TAttributes...>::Type>;
```

### `Resolve`

**Classification:** PUBLIC API · source access: `public`

Resolves one Attribute from this set by name.

```cpp
template<FixedString TName>
        struct Resolve
```

### `Type`

**Classification:** PUBLIC API · source access: `public`

Attribute declaration associated with the requested name.

```cpp
using Type = typename Detail::FindAttribute<TName, TAttributes...>::Type;
```

### `TName`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the requested Attribute exists and exactly matches the supplied compile-time value.

```cpp
template<FixedString TName, auto TExpectedValue>
        static constexpr bool Matches = []() constexpr
```

### `AttributeEquals`

**Classification:** PUBLIC API · source access: `public`

Requires an open-ended Attribute to equal the supplied compile-time value.

```cpp
template<FixedString TName, auto TExpectedValue>
    struct AttributeEquals
```

### `AttributeConstraintTag`

**Classification:** PUBLIC API · source access: `public`

Marker used to identify Attribute constraints during compile-time inspection.

```cpp
using AttributeConstraintTag = void;
```

### `Name`

**Classification:** PUBLIC API · source access: `public`

Required open-ended Attribute name.

```cpp
static constexpr auto Name = TName;
```

### `ExpectedValue`

**Classification:** PUBLIC API · source access: `public`

Required compile-time Attribute value.

```cpp
static constexpr auto ExpectedValue = TExpectedValue;
```

### `TAttributeSet`

**Classification:** PUBLIC API · source access: `public`

Determines whether the supplied Attribute set contains the required exact value.

```cpp
template<class TAttributeSet>
        static constexpr bool IsSatisfied()
```

### `TextAttributeEquals`

**Classification:** PUBLIC API · source access: `public`

Convenience alias requiring a compile-time text Attribute to equal the supplied text.

```cpp
template<FixedString TName, FixedString TExpectedValue>
    using TextAttributeEquals = AttributeEquals<TName, Text<TExpectedValue>>;
```

### `TConstraint`

**Classification:** PUBLIC API · source access: `public`

Default Attribute-constraint metadata for types that are not Attribute constraints.

```cpp
template<class TConstraint, class = void>
        struct AttributeConstraintTraits
```

### `IsValid`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the inspected type is a valid Attribute constraint.

```cpp
static constexpr bool IsValid = false;
```

### `TConstraint`

**Classification:** PUBLIC API · source access: `public`

Extracts metadata from an Attribute constraint declaration.

```cpp
template<class TConstraint>
        struct AttributeConstraintTraits<
            TConstraint,
```

### `IsValid`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the inspected type is a valid Attribute constraint.

```cpp
static constexpr bool IsValid = true;
```

### `TConstraint`

**Classification:** PUBLIC API · source access: `public`

Indicates whether a type represents a valid open-ended Attribute constraint.

```cpp
template<class TConstraint>
    inline constexpr bool IsAttributeConstraintV = Detail::AttributeConstraintTraits<TConstraint>::IsValid;
```

