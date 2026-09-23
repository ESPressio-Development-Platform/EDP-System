# src/composition/Requirement.hpp

**Primary classification:** PUBLIC COMPOSITION API

**Source baseline:** `9964ab2820cc5d290de69674000e011faaa684d9`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/9964ab2820cc5d290de69674000e011faaa684d9/src/composition/Requirement.hpp)

## Direct includes

- `cstddef`
- `type_traits`
- `Attribute.hpp`
- `Property.hpp`

## Documented declarations

### `TConstraint`

**Classification:** PUBLIC COMPOSITION API

Default constraint metadata for types that are not property constraints.

```cpp
template<class TConstraint, class = void>
        struct ConstraintTraits
```

### `IsValid`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the inspected type is a valid property constraint.

```cpp
static constexpr bool IsValid = false;
```

### `TConstraint`

**Classification:** PUBLIC COMPOSITION API

Extracts metadata from a property constraint declaration.

```cpp
template<class TConstraint>
        struct ConstraintTraits<
            TConstraint,
```

### `IsValid`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the inspected constraint references a valid property.

```cpp
static constexpr bool IsValid = IsPropertyV<typename TConstraint::PropertyType>;
```

### `TConstraint`

**Classification:** PUBLIC COMPOSITION API

Evaluates one constraint against a PropertySet-only view.

```cpp
template<class TConstraint, class TPropertySet, bool TIsAttributeConstraint = IsAttributeConstraintV<TConstraint>>
        struct ConstraintSatisfiedByProperties : std::bool_constant<
            TConstraint::template IsSatisfied<TPropertySet>()
        > {};
```

### `TConstraint`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates that an Attribute constraint cannot be satisfied from a property-only set.

```cpp
template<class TConstraint, class TPropertySet>
        struct ConstraintSatisfiedByProperties<TConstraint, TPropertySet, true> : std::false_type {};
```

### `TConstraint`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one property constraint against one capability Offer.

```cpp
template<class TConstraint, class TOffer, bool TIsAttributeConstraint = IsAttributeConstraintV<TConstraint>>
        struct ConstraintSatisfiedByOffer : std::bool_constant<
            TConstraint::template IsSatisfied<typename TOffer::Properties>()
        > {};
```

### `TConstraint`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one open-ended Attribute constraint against one capability Offer.

```cpp
template<class TConstraint, class TOffer>
        struct ConstraintSatisfiedByOffer<TConstraint, TOffer, true> : std::bool_constant<
            TConstraint::template IsSatisfied<typename TOffer::Attributes>()
        > {};
```

### `TConstraint`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether a type represents a valid property constraint.

```cpp
template<class TConstraint>
    inline constexpr bool IsConstraintV = Detail::ConstraintTraits<TConstraint>::IsValid;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Determines whether a valid property constraint belongs to the requested capability.

```cpp
template<class TCapability, class TConstraint, bool TIsConstraint = IsConstraintV<TConstraint>>
        struct IsConstraintFor : std::false_type {};
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Compares the capability carried by a valid property constraint with the requested capability.

```cpp
template<class TCapability, class TConstraint>
        struct IsConstraintFor<TCapability, TConstraint, true> : std::bool_constant<
            std::is_same_v<typename TConstraint::CapabilityType, TCapability>
        > {};
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether a constraint applies to the specified capability.

```cpp
template<class TCapability, class TConstraint>
    inline constexpr bool IsConstraintForV = Detail::IsConstraintFor<TCapability, TConstraint>::value;
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires a property value to equal the specified compile-time value.

```cpp
template<class TProperty, auto TExpectedValue>
    struct Equals
```

### `ConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker used to identify property constraints during compile-time inspection.

```cpp
using ConstraintTag = void;
```

### `PropertyType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Property evaluated by this constraint.

```cpp
using PropertyType = TProperty;
```

### `CapabilityType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Capability to which the evaluated property belongs.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `ExpectedValue`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Value against which the supplied property is compared.

```cpp
static constexpr typename TProperty::ValueType ExpectedValue = static_cast<typename TProperty::ValueType>(TExpectedValue);
```

### `TPropertySet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Determines whether the supplied property set satisfies this equality constraint.

```cpp
template<class TPropertySet>
        static constexpr bool IsSatisfied()
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires a property value to be greater than or equal to the specified compile-time value.

```cpp
template<class TProperty, auto TExpectedValue>
    struct AtLeast
```

### `ConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker used to identify property constraints during compile-time inspection.

```cpp
using ConstraintTag = void;
```

### `PropertyType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Property evaluated by this constraint.

```cpp
using PropertyType = TProperty;
```

### `CapabilityType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Capability to which the evaluated property belongs.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `ExpectedValue`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Minimum acceptable value for the supplied property.

```cpp
static constexpr typename TProperty::ValueType ExpectedValue = static_cast<typename TProperty::ValueType>(TExpectedValue);
```

### `TPropertySet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Determines whether the supplied property set satisfies this minimum-value constraint.

```cpp
template<class TPropertySet>
        static constexpr bool IsSatisfied()
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires a property value to be less than or equal to the specified compile-time value.

```cpp
template<class TProperty, auto TExpectedValue>
    struct AtMost
```

### `ConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker used to identify property constraints during compile-time inspection.

```cpp
using ConstraintTag = void;
```

### `PropertyType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Property evaluated by this constraint.

```cpp
using PropertyType = TProperty;
```

### `CapabilityType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Capability to which the evaluated property belongs.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `ExpectedValue`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Maximum acceptable value for the supplied property.

```cpp
static constexpr typename TProperty::ValueType ExpectedValue = static_cast<typename TProperty::ValueType>(TExpectedValue);
```

### `TPropertySet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Determines whether the supplied property set satisfies this maximum-value constraint.

```cpp
template<class TPropertySet>
        static constexpr bool IsSatisfied()
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires a property value to be strictly greater than the specified compile-time value.

```cpp
template<class TProperty, auto TExpectedValue>
    struct GreaterThan
```

### `ConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker used to identify property constraints during compile-time inspection.

```cpp
using ConstraintTag = void;
```

### `PropertyType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Property evaluated by this constraint.

```cpp
using PropertyType = TProperty;
```

### `CapabilityType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Capability to which the evaluated property belongs.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `ExpectedValue`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Exclusive lower bound for the supplied property.

```cpp
static constexpr typename TProperty::ValueType ExpectedValue = static_cast<typename TProperty::ValueType>(TExpectedValue);
```

### `TPropertySet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Determines whether the supplied property set satisfies this lower-bound constraint.

```cpp
template<class TPropertySet>
        static constexpr bool IsSatisfied()
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires a property value to be strictly less than the specified compile-time value.

```cpp
template<class TProperty, auto TExpectedValue>
    struct LessThan
```

### `ConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker used to identify property constraints during compile-time inspection.

```cpp
using ConstraintTag = void;
```

### `PropertyType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Property evaluated by this constraint.

```cpp
using PropertyType = TProperty;
```

### `CapabilityType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Capability to which the evaluated property belongs.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `ExpectedValue`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Exclusive upper bound for the supplied property.

```cpp
static constexpr typename TProperty::ValueType ExpectedValue = static_cast<typename TProperty::ValueType>(TExpectedValue);
```

### `TPropertySet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Determines whether the supplied property set satisfies this upper-bound constraint.

```cpp
template<class TPropertySet>
        static constexpr bool IsSatisfied()
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires a property value not to equal the supplied compile-time value.

- **Template parameter `TProperty`:** Property evaluated by this constraint.
- **Template parameter `TUnexpectedValue`:** Value which must not be advertised.

```cpp
template<class TProperty, auto TUnexpectedValue>
    struct NotEquals
```

### `ConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying a Property constraint.

```cpp
using ConstraintTag = void;
```

### `PropertyType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Property evaluated by this constraint.

```cpp
using PropertyType = TProperty;
```

### `CapabilityType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Capability owning the evaluated Property.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `UnexpectedValue`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Value which must not be advertised.

```cpp
static constexpr typename TProperty::ValueType UnexpectedValue =
            static_cast<typename TProperty::ValueType>(TUnexpectedValue);
```

### `TPropertySet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the supplied PropertySet advertises a different value.

- **Template parameter `TPropertySet`:** PropertySet being evaluated.

```cpp
template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires the supplied Property to be present.

- **Template parameter `TProperty`:** Property whose presence is required.

```cpp
template<class TProperty>
    struct HasProperty
```

### `ConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying a Property constraint.

```cpp
using ConstraintTag = void;
```

### `PropertyType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Property evaluated by this constraint.

```cpp
using PropertyType = TProperty;
```

### `CapabilityType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Capability owning the evaluated Property.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `TPropertySet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the supplied PropertySet contains the requested Property.

- **Template parameter `TPropertySet`:** PropertySet being evaluated.

```cpp
template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires the supplied Property to be absent.

- **Template parameter `TProperty`:** Property whose absence is required.

```cpp
template<class TProperty>
    struct DoesNotHaveProperty
```

### `ConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying a Property constraint.

```cpp
using ConstraintTag = void;
```

### `PropertyType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Property evaluated by this constraint.

```cpp
using PropertyType = TProperty;
```

### `CapabilityType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Capability owning the evaluated Property.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `TPropertySet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the supplied PropertySet omits the requested Property.

- **Template parameter `TPropertySet`:** PropertySet being evaluated.

```cpp
template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires a Property value to fall inside an inclusive compile-time range.

- **Template parameter `TProperty`:** Property evaluated by this constraint.
- **Template parameter `TMinimum`:** Inclusive lower bound.
- **Template parameter `TMaximum`:** Inclusive upper bound.

```cpp
template<class TProperty, auto TMinimum, auto TMaximum>
    struct Between
```

### `ConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying a Property constraint.

```cpp
using ConstraintTag = void;
```

### `PropertyType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Property evaluated by this constraint.

```cpp
using PropertyType = TProperty;
```

### `CapabilityType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Capability owning the evaluated Property.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `Minimum`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Inclusive lower bound.

```cpp
static constexpr typename TProperty::ValueType Minimum =
            static_cast<typename TProperty::ValueType>(TMinimum);
```

### `Maximum`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Inclusive upper bound.

```cpp
static constexpr typename TProperty::ValueType Maximum =
            static_cast<typename TProperty::ValueType>(TMaximum);
```

### `TPropertySet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the advertised Property value is inside the inclusive range.

- **Template parameter `TPropertySet`:** PropertySet being evaluated.

```cpp
template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires a Property value to equal one of the supplied compile-time values.

- **Template parameter `TProperty`:** Property evaluated by this constraint.
- **Template parameter `TAllowedValues`:** One or more accepted values.

```cpp
template<class TProperty, auto... TAllowedValues>
    struct OneOf
```

### `ConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying a Property constraint.

```cpp
using ConstraintTag = void;
```

### `PropertyType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Property evaluated by this constraint.

```cpp
using PropertyType = TProperty;
```

### `CapabilityType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Capability owning the evaluated Property.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `TPropertySet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the advertised Property value equals at least one accepted value.

- **Template parameter `TPropertySet`:** PropertySet being evaluated.

```cpp
template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires a Property value to equal none of the supplied compile-time values.

- **Template parameter `TProperty`:** Property evaluated by this constraint.
- **Template parameter `TExcludedValues`:** One or more prohibited values.

```cpp
template<class TProperty, auto... TExcludedValues>
    struct NoneOf
```

### `ConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying a Property constraint.

```cpp
using ConstraintTag = void;
```

### `PropertyType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Property evaluated by this constraint.

```cpp
using PropertyType = TProperty;
```

### `CapabilityType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Capability owning the evaluated Property.

```cpp
using CapabilityType = typename TProperty::CapabilityType;
```

### `TPropertySet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the advertised Property value differs from every excluded value.

- **Template parameter `TPropertySet`:** PropertySet being evaluated.

```cpp
template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept
```

### `HasAttribute`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires one open-ended Attribute to be present regardless of its value.

- **Template parameter `TName`:** Attribute name whose presence is required.

```cpp
template<FixedString TName>
    struct HasAttribute
```

### `AttributeConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying an Attribute constraint.

```cpp
using AttributeConstraintTag = void;
```

### `TAttributeSet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the supplied AttributeSet contains the requested name.

- **Template parameter `TAttributeSet`:** AttributeSet being evaluated.

```cpp
template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept
```

### `DoesNotHaveAttribute`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires one open-ended Attribute to be absent.

- **Template parameter `TName`:** Attribute name whose absence is required.

```cpp
template<FixedString TName>
    struct DoesNotHaveAttribute
```

### `AttributeConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying an Attribute constraint.

```cpp
using AttributeConstraintTag = void;
```

### `TAttributeSet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the supplied AttributeSet omits the requested name.

- **Template parameter `TAttributeSet`:** AttributeSet being evaluated.

```cpp
template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept
```

### `AttributeValuesEqual`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Forward declaration for safe compile-time Attribute equality comparison.

```cpp
template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValuesEqual() noexcept;
```

### `AttributeValuesNotEqual`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Forward declaration for safe compile-time Attribute inequality comparison.

```cpp
template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValuesNotEqual() noexcept;
```

### `AttributeValueAtLeast`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Forward declaration for safe compile-time Attribute greater-than-or-equal comparison.

```cpp
template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValueAtLeast() noexcept;
```

### `AttributeValueAtMost`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Forward declaration for safe compile-time Attribute less-than-or-equal comparison.

```cpp
template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValueAtMost() noexcept;
```

### `AttributeValueGreaterThan`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Forward declaration for safe compile-time Attribute strict greater-than comparison.

```cpp
template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValueGreaterThan() noexcept;
```

### `AttributeValueLessThan`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Forward declaration for safe compile-time Attribute strict less-than comparison.

```cpp
template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValueLessThan() noexcept;
```

### `AttributeNotEquals`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires one open-ended Attribute not to equal the supplied compile-time value.

Missing Attributes do not satisfy this constraint.

- **Template parameter `TName`:** Attribute name being evaluated.
- **Template parameter `TUnexpectedValue`:** Value which must not be advertised.

```cpp
template<FixedString TName, auto TUnexpectedValue>
    struct AttributeNotEquals
```

### `AttributeConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying an Attribute constraint.

```cpp
using AttributeConstraintTag = void;
```

### `TAttributeSet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the Attribute is present and differs from the prohibited value.

- **Template parameter `TAttributeSet`:** AttributeSet being evaluated.

```cpp
template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept
```

### `AttributeOneOf`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires one open-ended Attribute to equal at least one supplied compile-time value.

- **Template parameter `TName`:** Attribute name being evaluated.
- **Template parameter `TAllowedValues`:** One or more accepted values.

```cpp
template<FixedString TName, auto... TAllowedValues>
    struct AttributeOneOf
```

### `AttributeConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying an Attribute constraint.

```cpp
using AttributeConstraintTag = void;
```

### `TAttributeSet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the Attribute equals at least one accepted value.

- **Template parameter `TAttributeSet`:** AttributeSet being evaluated.

```cpp
template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept
```

### `AttributeNoneOf`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires one open-ended Attribute to equal none of the supplied compile-time values.

- **Template parameter `TName`:** Attribute name being evaluated.
- **Template parameter `TExcludedValues`:** One or more prohibited values.

```cpp
template<FixedString TName, auto... TExcludedValues>
    struct AttributeNoneOf
```

### `AttributeConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying an Attribute constraint.

```cpp
using AttributeConstraintTag = void;
```

### `TAttributeSet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the Attribute differs from every prohibited value.

- **Template parameter `TAttributeSet`:** AttributeSet being evaluated.

```cpp
template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept
```

### `AttributeAtLeast`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires one ordered Attribute value to be greater than or equal to the supplied value.

Missing or non-orderable Attributes do not satisfy this constraint.

- **Template parameter `TName`:** Attribute name being evaluated.
- **Template parameter `TMinimum`:** Inclusive lower bound.

```cpp
template<FixedString TName, auto TMinimum>
    struct AttributeAtLeast
```

### `AttributeConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying an Attribute constraint.

```cpp
using AttributeConstraintTag = void;
```

### `TAttributeSet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the Attribute is present, orderable and no smaller than the supplied bound.

- **Template parameter `TAttributeSet`:** AttributeSet being evaluated.

```cpp
template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept
```

### `AttributeAtMost`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires one ordered Attribute value to be less than or equal to the supplied value.

- **Template parameter `TName`:** Attribute name being evaluated.
- **Template parameter `TMaximum`:** Inclusive upper bound.

```cpp
template<FixedString TName, auto TMaximum>
    struct AttributeAtMost
```

### `AttributeConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying an Attribute constraint.

```cpp
using AttributeConstraintTag = void;
```

### `TAttributeSet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the Attribute is present, orderable and no larger than the supplied bound.

- **Template parameter `TAttributeSet`:** AttributeSet being evaluated.

```cpp
template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept
```

### `AttributeGreaterThan`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires one ordered Attribute value to be strictly greater than the supplied value.

- **Template parameter `TName`:** Attribute name being evaluated.
- **Template parameter `TMinimum`:** Exclusive lower bound.

```cpp
template<FixedString TName, auto TMinimum>
    struct AttributeGreaterThan
```

### `AttributeConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying an Attribute constraint.

```cpp
using AttributeConstraintTag = void;
```

### `TAttributeSet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the Attribute is present, orderable and greater than the supplied bound.

- **Template parameter `TAttributeSet`:** AttributeSet being evaluated.

```cpp
template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept
```

### `AttributeLessThan`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires one ordered Attribute value to be strictly less than the supplied value.

- **Template parameter `TName`:** Attribute name being evaluated.
- **Template parameter `TMaximum`:** Exclusive upper bound.

```cpp
template<FixedString TName, auto TMaximum>
    struct AttributeLessThan
```

### `AttributeConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying an Attribute constraint.

```cpp
using AttributeConstraintTag = void;
```

### `TAttributeSet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the Attribute is present, orderable and less than the supplied bound.

- **Template parameter `TAttributeSet`:** AttributeSet being evaluated.

```cpp
template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept
```

### `AttributeBetween`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires one ordered Attribute value to fall within the supplied inclusive interval.

- **Template parameter `TName`:** Attribute name being evaluated.
- **Template parameter `TMinimum`:** Inclusive lower bound.
- **Template parameter `TMaximum`:** Inclusive upper bound.

```cpp
template<FixedString TName, auto TMinimum, auto TMaximum>
    struct AttributeBetween
```

### `AttributeConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying an Attribute constraint.

```cpp
using AttributeConstraintTag = void;
```

### `TAttributeSet`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the Attribute is present, orderable and inside the inclusive interval.

- **Template parameter `TAttributeSet`:** AttributeSet being evaluated.

```cpp
template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept
```

### `AttributeValuesEqual`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Safely compares two compile-time Attribute values for equality.

- **Template parameter `TLeftValue`:** Left compile-time value.
- **Template parameter `TRightValue`:** Right compile-time value.

```cpp
template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValuesEqual() noexcept
```

### `AttributeValuesNotEqual`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Safely compares two compile-time Attribute values for inequality.

- **Template parameter `TLeftValue`:** Left compile-time value.
- **Template parameter `TRightValue`:** Right compile-time value.

```cpp
template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValuesNotEqual() noexcept
```

### `AttributeValueAtLeast`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Safely compares two compile-time Attribute values using greater-than-or-equal ordering.

- **Template parameter `TLeftValue`:** Left compile-time value.
- **Template parameter `TRightValue`:** Right compile-time value.

```cpp
template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValueAtLeast() noexcept
```

### `AttributeValueAtMost`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Safely compares two compile-time Attribute values using less-than-or-equal ordering.

- **Template parameter `TLeftValue`:** Left compile-time value.
- **Template parameter `TRightValue`:** Right compile-time value.

```cpp
template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValueAtMost() noexcept
```

### `AttributeValueGreaterThan`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Safely compares two compile-time Attribute values using strict greater-than ordering.

- **Template parameter `TLeftValue`:** Left compile-time value.
- **Template parameter `TRightValue`:** Right compile-time value.

```cpp
template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValueGreaterThan() noexcept
```

### `AttributeValueLessThan`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Safely compares two compile-time Attribute values using strict less-than ordering.

- **Template parameter `TLeftValue`:** Left compile-time value.
- **Template parameter `TRightValue`:** Right compile-time value.

```cpp
template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValueLessThan() noexcept
```

### `TConstraint`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Default logical-constraint metadata for unrelated Types.

- **Template parameter `TConstraint`:** Type being inspected.
- **Template parameter `TEnable`:** SFINAE helper used when logical-constraint metadata is present.

```cpp
template<class TConstraint, class TEnable = void>
        struct LogicalConstraintTraits
```

### `IsValid`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the inspected Type is a logical constraint expression.

```cpp
static constexpr bool IsValid = false;
```

### `TConstraint`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Extracts logical-constraint metadata.

- **Template parameter `TConstraint`:** Logical constraint Type being inspected.

```cpp
template<class TConstraint>
        struct LogicalConstraintTraits<
            TConstraint,
```

### `IsValid`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the inspected Type is a logical constraint expression.

```cpp
static constexpr bool IsValid = true;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether a Requirement constraint can be applied to the supplied Capability.

- **Template parameter `TCapability`:** Capability being qualified.
- **Template parameter `TConstraint`:** Constraint expression being inspected.

```cpp
template<class TCapability, class TConstraint>
        consteval bool RequirementConstraintAppliesToCapability() noexcept
```

### `TConstraint`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one Requirement constraint against one complete capability Offer.

- **Template parameter `TConstraint`:** Constraint expression being evaluated.
- **Template parameter `TOffer`:** Capability Offer being inspected.

```cpp
template<class TConstraint, class TOffer>
        consteval bool RequirementConstraintSatisfiedByOffer() noexcept
```

### `AllOf`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires every nested constraint expression to be satisfied.

- **Template parameter `TConstraints`:** Nested Requirement constraints.

```cpp
template<class... TConstraints>
    struct AllOf
```

### `LogicalConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying a logical Requirement constraint.

```cpp
using LogicalConstraintTag = void;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether every nested constraint can be applied to the supplied Capability.

- **Template parameter `TCapability`:** Capability being qualified.

```cpp
template<class TCapability>
        static consteval bool AppliesTo() noexcept
```

### `TOffer`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether every nested constraint is satisfied by one Offer.

- **Template parameter `TOffer`:** Offer being evaluated.

```cpp
template<class TOffer>
        static consteval bool IsSatisfied() noexcept
```

### `AnyOf`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires at least one nested constraint expression to be satisfied.

- **Template parameter `TConstraints`:** Nested Requirement constraints.

```cpp
template<class... TConstraints>
    struct AnyOf
```

### `LogicalConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying a logical Requirement constraint.

```cpp
using LogicalConstraintTag = void;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether every nested expression is meaningful for the supplied Capability.

- **Template parameter `TCapability`:** Capability being qualified.

```cpp
template<class TCapability>
        static consteval bool AppliesTo() noexcept
```

### `TOffer`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether at least one nested constraint is satisfied by one Offer.

- **Template parameter `TOffer`:** Offer being evaluated.

```cpp
template<class TOffer>
        static consteval bool IsSatisfied() noexcept
```

### `TConstraint`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Negates one nested Requirement constraint.

- **Template parameter `TConstraint`:** Nested Requirement constraint.

```cpp
template<class TConstraint>
    struct Not
```

### `LogicalConstraintTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker identifying a logical Requirement constraint.

```cpp
using LogicalConstraintTag = void;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the nested constraint can be applied to the supplied Capability.

- **Template parameter `TCapability`:** Capability being qualified.

```cpp
template<class TCapability>
        static consteval bool AppliesTo() noexcept
```

### `TOffer`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the nested constraint is not satisfied by one Offer.

- **Template parameter `TOffer`:** Offer being evaluated.

```cpp
template<class TOffer>
        static consteval bool IsSatisfied() noexcept
```

