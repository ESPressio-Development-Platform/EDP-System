#pragma once

#include <cstddef>
#include <type_traits>

#include "Attribute.hpp"
#include "Property.hpp"

namespace ESPressio::System::CompositionFramework {

    namespace Detail {

        /// Default constraint metadata for types that are not property constraints.
        template<class TConstraint, class = void>
        struct ConstraintTraits {

            // Constraint metadata.

            /// Indicates whether the inspected type is a valid property constraint.
            static constexpr bool IsValid = false;

        };


        /// Extracts metadata from a property constraint declaration.
        template<class TConstraint>
        struct ConstraintTraits<
            TConstraint,
            std::void_t<
                typename TConstraint::ConstraintTag,
                typename TConstraint::PropertyType,
                typename TConstraint::CapabilityType
            >
        > {

            // Constraint metadata.

            /// Indicates whether the inspected constraint references a valid property.
            static constexpr bool IsValid = IsPropertyV<typename TConstraint::PropertyType>;

        };


        /// Evaluates one constraint against a PropertySet-only view.
        template<class TConstraint, class TPropertySet, bool TIsAttributeConstraint = IsAttributeConstraintV<TConstraint>>
        struct ConstraintSatisfiedByProperties : std::bool_constant<
            TConstraint::template IsSatisfied<TPropertySet>()
        > {};


        /// Indicates that an Attribute constraint cannot be satisfied from a property-only set.
        template<class TConstraint, class TPropertySet>
        struct ConstraintSatisfiedByProperties<TConstraint, TPropertySet, true> : std::false_type {};


        /// Evaluates one property constraint against one capability Offer.
        template<class TConstraint, class TOffer, bool TIsAttributeConstraint = IsAttributeConstraintV<TConstraint>>
        struct ConstraintSatisfiedByOffer : std::bool_constant<
            TConstraint::template IsSatisfied<typename TOffer::Properties>()
        > {};


        /// Evaluates one open-ended Attribute constraint against one capability Offer.
        template<class TConstraint, class TOffer>
        struct ConstraintSatisfiedByOffer<TConstraint, TOffer, true> : std::bool_constant<
            TConstraint::template IsSatisfied<typename TOffer::Attributes>()
        > {};


    } // ESPressio::System::CompositionFramework::Detail


    // Constraint inspection.

    /// Indicates whether a type represents a valid property constraint.
    template<class TConstraint>
    inline constexpr bool IsConstraintV = Detail::ConstraintTraits<TConstraint>::IsValid;


    namespace Detail {

        /// Determines whether a valid property constraint belongs to the requested capability.
        template<class TCapability, class TConstraint, bool TIsConstraint = IsConstraintV<TConstraint>>
        struct IsConstraintFor : std::false_type {};


        /// Compares the capability carried by a valid property constraint with the requested capability.
        template<class TCapability, class TConstraint>
        struct IsConstraintFor<TCapability, TConstraint, true> : std::bool_constant<
            std::is_same_v<typename TConstraint::CapabilityType, TCapability>
        > {};

    } // ESPressio::System::CompositionFramework::Detail


    /// Indicates whether a constraint applies to the specified capability.
    template<class TCapability, class TConstraint>
    inline constexpr bool IsConstraintForV = Detail::IsConstraintFor<TCapability, TConstraint>::value;


    /// Requires a property value to equal the specified compile-time value.
    template<class TProperty, auto TExpectedValue>
    struct Equals {

        static_assert(
            IsPropertyV<TProperty>,
            "Equals requires a concrete composition Property"
        );

        static_assert(
            std::is_convertible_v<decltype(TExpectedValue), typename TProperty::ValueType>,
            "Equals comparison value cannot be converted to the property's declared value type"
        );

        // Constraint metadata.

        /// Marker used to identify property constraints during compile-time inspection.
        using ConstraintTag = void;

        /// Property evaluated by this constraint.
        using PropertyType = TProperty;

        /// Capability to which the evaluated property belongs.
        using CapabilityType = typename TProperty::CapabilityType;

        /// Value against which the supplied property is compared.
        static constexpr typename TProperty::ValueType ExpectedValue = static_cast<typename TProperty::ValueType>(TExpectedValue);

        // Constraint evaluation.

        /// Determines whether the supplied property set satisfies this equality constraint.
        template<class TPropertySet>
        static constexpr bool IsSatisfied() {
            if constexpr (!TPropertySet::template Contains<TProperty>) {
                return false;
            } else {
                return TPropertySet::template Value<TProperty> == ExpectedValue;
            }
        }

    };


    /// Requires a property value to be greater than or equal to the specified compile-time value.
    template<class TProperty, auto TExpectedValue>
    struct AtLeast {

        static_assert(
            IsPropertyV<TProperty>,
            "AtLeast requires a concrete composition Property"
        );

        static_assert(
            std::is_convertible_v<decltype(TExpectedValue), typename TProperty::ValueType>,
            "AtLeast comparison value cannot be converted to the property's declared value type"
        );

        // Constraint metadata.

        /// Marker used to identify property constraints during compile-time inspection.
        using ConstraintTag = void;

        /// Property evaluated by this constraint.
        using PropertyType = TProperty;

        /// Capability to which the evaluated property belongs.
        using CapabilityType = typename TProperty::CapabilityType;

        /// Minimum acceptable value for the supplied property.
        static constexpr typename TProperty::ValueType ExpectedValue = static_cast<typename TProperty::ValueType>(TExpectedValue);

        // Constraint evaluation.

        /// Determines whether the supplied property set satisfies this minimum-value constraint.
        template<class TPropertySet>
        static constexpr bool IsSatisfied() {
            if constexpr (!TPropertySet::template Contains<TProperty>) {
                return false;
            } else {
                return TPropertySet::template Value<TProperty> >= ExpectedValue;
            }
        }

    };


    /// Requires a property value to be less than or equal to the specified compile-time value.
    template<class TProperty, auto TExpectedValue>
    struct AtMost {

        static_assert(
            IsPropertyV<TProperty>,
            "AtMost requires a concrete composition Property"
        );

        static_assert(
            std::is_convertible_v<decltype(TExpectedValue), typename TProperty::ValueType>,
            "AtMost comparison value cannot be converted to the property's declared value type"
        );

        // Constraint metadata.

        /// Marker used to identify property constraints during compile-time inspection.
        using ConstraintTag = void;

        /// Property evaluated by this constraint.
        using PropertyType = TProperty;

        /// Capability to which the evaluated property belongs.
        using CapabilityType = typename TProperty::CapabilityType;

        /// Maximum acceptable value for the supplied property.
        static constexpr typename TProperty::ValueType ExpectedValue = static_cast<typename TProperty::ValueType>(TExpectedValue);

        // Constraint evaluation.

        /// Determines whether the supplied property set satisfies this maximum-value constraint.
        template<class TPropertySet>
        static constexpr bool IsSatisfied() {
            if constexpr (!TPropertySet::template Contains<TProperty>) {
                return false;
            } else {
                return TPropertySet::template Value<TProperty> <= ExpectedValue;
            }
        }

    };


    /// Requires a property value to be strictly greater than the specified compile-time value.
    template<class TProperty, auto TExpectedValue>
    struct GreaterThan {

        static_assert(
            IsPropertyV<TProperty>,
            "GreaterThan requires a concrete composition Property"
        );

        static_assert(
            std::is_convertible_v<decltype(TExpectedValue), typename TProperty::ValueType>,
            "GreaterThan comparison value cannot be converted to the property's declared value type"
        );

        // Constraint metadata.

        /// Marker used to identify property constraints during compile-time inspection.
        using ConstraintTag = void;

        /// Property evaluated by this constraint.
        using PropertyType = TProperty;

        /// Capability to which the evaluated property belongs.
        using CapabilityType = typename TProperty::CapabilityType;

        /// Exclusive lower bound for the supplied property.
        static constexpr typename TProperty::ValueType ExpectedValue = static_cast<typename TProperty::ValueType>(TExpectedValue);

        // Constraint evaluation.

        /// Determines whether the supplied property set satisfies this lower-bound constraint.
        template<class TPropertySet>
        static constexpr bool IsSatisfied() {
            if constexpr (!TPropertySet::template Contains<TProperty>) {
                return false;
            } else {
                return TPropertySet::template Value<TProperty> > ExpectedValue;
            }
        }

    };


    /// Requires a property value to be strictly less than the specified compile-time value.
    template<class TProperty, auto TExpectedValue>
    struct LessThan {

        static_assert(
            IsPropertyV<TProperty>,
            "LessThan requires a concrete composition Property"
        );

        static_assert(
            std::is_convertible_v<decltype(TExpectedValue), typename TProperty::ValueType>,
            "LessThan comparison value cannot be converted to the property's declared value type"
        );

        // Constraint metadata.

        /// Marker used to identify property constraints during compile-time inspection.
        using ConstraintTag = void;

        /// Property evaluated by this constraint.
        using PropertyType = TProperty;

        /// Capability to which the evaluated property belongs.
        using CapabilityType = typename TProperty::CapabilityType;

        /// Exclusive upper bound for the supplied property.
        static constexpr typename TProperty::ValueType ExpectedValue = static_cast<typename TProperty::ValueType>(TExpectedValue);

        // Constraint evaluation.

        /// Determines whether the supplied property set satisfies this upper-bound constraint.
        template<class TPropertySet>
        static constexpr bool IsSatisfied() {
            if constexpr (!TPropertySet::template Contains<TProperty>) {
                return false;
            } else {
                return TPropertySet::template Value<TProperty> < ExpectedValue;
            }
        }

    };


    /// Requires a property value not to equal the supplied compile-time value.
    ///
    /// @tparam TProperty Property evaluated by this constraint.
    /// @tparam TUnexpectedValue Value which must not be advertised.
    template<class TProperty, auto TUnexpectedValue>
    struct NotEquals {

        static_assert(
            IsPropertyV<TProperty>,
            "NotEquals requires a concrete composition Property"
        );

        static_assert(
            std::is_convertible_v<decltype(TUnexpectedValue), typename TProperty::ValueType>,
            "NotEquals comparison value cannot be converted to the property's declared value type"
        );

        // Constraint metadata.

        /// Marker identifying a Property constraint.
        using ConstraintTag = void;

        /// Property evaluated by this constraint.
        using PropertyType = TProperty;

        /// Capability owning the evaluated Property.
        using CapabilityType = typename TProperty::CapabilityType;

        /// Value which must not be advertised.
        static constexpr typename TProperty::ValueType UnexpectedValue =
            static_cast<typename TProperty::ValueType>(TUnexpectedValue);


        // Constraint evaluation.

        /// Indicates whether the supplied PropertySet advertises a different value.
        ///
        /// @tparam TPropertySet PropertySet being evaluated.
        template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept {
            if constexpr (!TPropertySet::template Contains<TProperty>) {
                return false;
            } else {
                return TPropertySet::template Value<TProperty> != UnexpectedValue;
            }
        }

    };


    /// Requires the supplied Property to be present.
    ///
    /// @tparam TProperty Property whose presence is required.
    template<class TProperty>
    struct HasProperty {

        static_assert(
            IsPropertyV<TProperty>,
            "HasProperty requires a concrete composition Property"
        );

        // Constraint metadata.

        /// Marker identifying a Property constraint.
        using ConstraintTag = void;

        /// Property evaluated by this constraint.
        using PropertyType = TProperty;

        /// Capability owning the evaluated Property.
        using CapabilityType = typename TProperty::CapabilityType;


        // Constraint evaluation.

        /// Indicates whether the supplied PropertySet contains the requested Property.
        ///
        /// @tparam TPropertySet PropertySet being evaluated.
        template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept {
            return TPropertySet::template Contains<TProperty>;
        }

    };


    /// Requires the supplied Property to be absent.
    ///
    /// @tparam TProperty Property whose absence is required.
    template<class TProperty>
    struct DoesNotHaveProperty {

        static_assert(
            IsPropertyV<TProperty>,
            "DoesNotHaveProperty requires a concrete composition Property"
        );

        // Constraint metadata.

        /// Marker identifying a Property constraint.
        using ConstraintTag = void;

        /// Property evaluated by this constraint.
        using PropertyType = TProperty;

        /// Capability owning the evaluated Property.
        using CapabilityType = typename TProperty::CapabilityType;


        // Constraint evaluation.

        /// Indicates whether the supplied PropertySet omits the requested Property.
        ///
        /// @tparam TPropertySet PropertySet being evaluated.
        template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept {
            return !TPropertySet::template Contains<TProperty>;
        }

    };


    /// Requires a Property value to fall inside an inclusive compile-time range.
    ///
    /// @tparam TProperty Property evaluated by this constraint.
    /// @tparam TMinimum Inclusive lower bound.
    /// @tparam TMaximum Inclusive upper bound.
    template<class TProperty, auto TMinimum, auto TMaximum>
    struct Between {

        static_assert(
            IsPropertyV<TProperty>,
            "Between requires a concrete composition Property"
        );

        static_assert(
            std::is_convertible_v<decltype(TMinimum), typename TProperty::ValueType> &&
            std::is_convertible_v<decltype(TMaximum), typename TProperty::ValueType>,
            "Between bounds must be convertible to the property's declared value type"
        );

        // Constraint metadata.

        /// Marker identifying a Property constraint.
        using ConstraintTag = void;

        /// Property evaluated by this constraint.
        using PropertyType = TProperty;

        /// Capability owning the evaluated Property.
        using CapabilityType = typename TProperty::CapabilityType;

        /// Inclusive lower bound.
        static constexpr typename TProperty::ValueType Minimum =
            static_cast<typename TProperty::ValueType>(TMinimum);

        /// Inclusive upper bound.
        static constexpr typename TProperty::ValueType Maximum =
            static_cast<typename TProperty::ValueType>(TMaximum);

        static_assert(
            Minimum <= Maximum,
            "Between minimum must not exceed maximum"
        );


        // Constraint evaluation.

        /// Indicates whether the advertised Property value is inside the inclusive range.
        ///
        /// @tparam TPropertySet PropertySet being evaluated.
        template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept {
            if constexpr (!TPropertySet::template Contains<TProperty>) {
                return false;
            } else {
                constexpr auto value = TPropertySet::template Value<TProperty>;

                return value >= Minimum && value <= Maximum;
            }
        }

    };


    /// Requires a Property value to equal one of the supplied compile-time values.
    ///
    /// @tparam TProperty Property evaluated by this constraint.
    /// @tparam TAllowedValues One or more accepted values.
    template<class TProperty, auto... TAllowedValues>
    struct OneOf {

        static_assert(
            IsPropertyV<TProperty>,
            "OneOf requires a concrete composition Property"
        );

        static_assert(
            sizeof...(TAllowedValues) > 0U,
            "OneOf requires at least one accepted value"
        );

        static_assert(
            (std::is_convertible_v<decltype(TAllowedValues), typename TProperty::ValueType> && ...),
            "OneOf values must be convertible to the property's declared value type"
        );

        // Constraint metadata.

        /// Marker identifying a Property constraint.
        using ConstraintTag = void;

        /// Property evaluated by this constraint.
        using PropertyType = TProperty;

        /// Capability owning the evaluated Property.
        using CapabilityType = typename TProperty::CapabilityType;


        // Constraint evaluation.

        /// Indicates whether the advertised Property value equals at least one accepted value.
        ///
        /// @tparam TPropertySet PropertySet being evaluated.
        template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept {
            if constexpr (!TPropertySet::template Contains<TProperty>) {
                return false;
            } else {
                constexpr auto value = TPropertySet::template Value<TProperty>;

                return (
                    (
                        value ==
                        static_cast<typename TProperty::ValueType>(TAllowedValues)
                    ) ||
                    ...
                );
            }
        }

    };


    /// Requires a Property value to equal none of the supplied compile-time values.
    ///
    /// @tparam TProperty Property evaluated by this constraint.
    /// @tparam TExcludedValues One or more prohibited values.
    template<class TProperty, auto... TExcludedValues>
    struct NoneOf {

        static_assert(
            IsPropertyV<TProperty>,
            "NoneOf requires a concrete composition Property"
        );

        static_assert(
            sizeof...(TExcludedValues) > 0U,
            "NoneOf requires at least one excluded value"
        );

        static_assert(
            (std::is_convertible_v<decltype(TExcludedValues), typename TProperty::ValueType> && ...),
            "NoneOf values must be convertible to the property's declared value type"
        );

        // Constraint metadata.

        /// Marker identifying a Property constraint.
        using ConstraintTag = void;

        /// Property evaluated by this constraint.
        using PropertyType = TProperty;

        /// Capability owning the evaluated Property.
        using CapabilityType = typename TProperty::CapabilityType;


        // Constraint evaluation.

        /// Indicates whether the advertised Property value differs from every excluded value.
        ///
        /// @tparam TPropertySet PropertySet being evaluated.
        template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept {
            if constexpr (!TPropertySet::template Contains<TProperty>) {
                return false;
            } else {
                constexpr auto value = TPropertySet::template Value<TProperty>;

                return (
                    (
                        value !=
                        static_cast<typename TProperty::ValueType>(TExcludedValues)
                    ) &&
                    ...
                );
            }
        }

    };


    /// Requires one open-ended Attribute to be present regardless of its value.
    ///
    /// @tparam TName Attribute name whose presence is required.
    template<FixedString TName>
    struct HasAttribute {

        // Attribute-constraint metadata.

        /// Marker identifying an Attribute constraint.
        using AttributeConstraintTag = void;


        // Constraint evaluation.

        /// Indicates whether the supplied AttributeSet contains the requested name.
        ///
        /// @tparam TAttributeSet AttributeSet being evaluated.
        template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept {
            return TAttributeSet::template Contains<TName>;
        }

    };


    /// Requires one open-ended Attribute to be absent.
    ///
    /// @tparam TName Attribute name whose absence is required.
    template<FixedString TName>
    struct DoesNotHaveAttribute {

        // Attribute-constraint metadata.

        /// Marker identifying an Attribute constraint.
        using AttributeConstraintTag = void;


        // Constraint evaluation.

        /// Indicates whether the supplied AttributeSet omits the requested name.
        ///
        /// @tparam TAttributeSet AttributeSet being evaluated.
        template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept {
            return !TAttributeSet::template Contains<TName>;
        }

    };


    namespace Detail {

        /// Forward declaration for safe compile-time Attribute equality comparison.
        template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValuesEqual() noexcept;

        /// Forward declaration for safe compile-time Attribute inequality comparison.
        template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValuesNotEqual() noexcept;

        /// Forward declaration for safe compile-time Attribute greater-than-or-equal comparison.
        template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValueAtLeast() noexcept;

        /// Forward declaration for safe compile-time Attribute less-than-or-equal comparison.
        template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValueAtMost() noexcept;

        /// Forward declaration for safe compile-time Attribute strict greater-than comparison.
        template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValueGreaterThan() noexcept;

        /// Forward declaration for safe compile-time Attribute strict less-than comparison.
        template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValueLessThan() noexcept;

    } // ESPressio::System::CompositionFramework::Detail


    /// Requires one open-ended Attribute not to equal the supplied compile-time value.
    ///
    /// Missing Attributes do not satisfy this constraint.
    ///
    /// @tparam TName Attribute name being evaluated.
    /// @tparam TUnexpectedValue Value which must not be advertised.
    template<FixedString TName, auto TUnexpectedValue>
    struct AttributeNotEquals {

        // Attribute-constraint metadata.

        /// Marker identifying an Attribute constraint.
        using AttributeConstraintTag = void;


        // Constraint evaluation.

        /// Indicates whether the Attribute is present and differs from the prohibited value.
        ///
        /// @tparam TAttributeSet AttributeSet being evaluated.
        template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept {
            if constexpr (!TAttributeSet::template Contains<TName>) {
                return false;
            } else {
                using AttributeType = typename TAttributeSet::template Resolve<TName>::Type;
                constexpr auto actualValue = Detail::AttributeTraits<AttributeType>::Value;

                return Detail::AttributeValuesNotEqual<
                    actualValue,
                    TUnexpectedValue
                >();
            }
        }

    };


    /// Requires one open-ended Attribute to equal at least one supplied compile-time value.
    ///
    /// @tparam TName Attribute name being evaluated.
    /// @tparam TAllowedValues One or more accepted values.
    template<FixedString TName, auto... TAllowedValues>
    struct AttributeOneOf {

        static_assert(
            sizeof...(TAllowedValues) > 0U,
            "AttributeOneOf requires at least one accepted value"
        );

        // Attribute-constraint metadata.

        /// Marker identifying an Attribute constraint.
        using AttributeConstraintTag = void;


        // Constraint evaluation.

        /// Indicates whether the Attribute equals at least one accepted value.
        ///
        /// @tparam TAttributeSet AttributeSet being evaluated.
        template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept {
            if constexpr (!TAttributeSet::template Contains<TName>) {
                return false;
            } else {
                using AttributeType = typename TAttributeSet::template Resolve<TName>::Type;
                constexpr auto actualValue = Detail::AttributeTraits<AttributeType>::Value;

                return (
                    Detail::AttributeValuesEqual<
                        actualValue,
                        TAllowedValues
                    >() ||
                    ...
                );
            }
        }

    };


    /// Requires one open-ended Attribute to equal none of the supplied compile-time values.
    ///
    /// @tparam TName Attribute name being evaluated.
    /// @tparam TExcludedValues One or more prohibited values.
    template<FixedString TName, auto... TExcludedValues>
    struct AttributeNoneOf {

        static_assert(
            sizeof...(TExcludedValues) > 0U,
            "AttributeNoneOf requires at least one excluded value"
        );

        // Attribute-constraint metadata.

        /// Marker identifying an Attribute constraint.
        using AttributeConstraintTag = void;


        // Constraint evaluation.

        /// Indicates whether the Attribute differs from every prohibited value.
        ///
        /// @tparam TAttributeSet AttributeSet being evaluated.
        template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept {
            if constexpr (!TAttributeSet::template Contains<TName>) {
                return false;
            } else {
                using AttributeType = typename TAttributeSet::template Resolve<TName>::Type;
                constexpr auto actualValue = Detail::AttributeTraits<AttributeType>::Value;

                return (
                    Detail::AttributeValuesNotEqual<
                        actualValue,
                        TExcludedValues
                    >() &&
                    ...
                );
            }
        }

    };


    /// Requires one ordered Attribute value to be greater than or equal to the supplied value.
    ///
    /// Missing or non-orderable Attributes do not satisfy this constraint.
    ///
    /// @tparam TName Attribute name being evaluated.
    /// @tparam TMinimum Inclusive lower bound.
    template<FixedString TName, auto TMinimum>
    struct AttributeAtLeast {

        // Attribute-constraint metadata.

        /// Marker identifying an Attribute constraint.
        using AttributeConstraintTag = void;


        // Constraint evaluation.

        /// Indicates whether the Attribute is present, orderable and no smaller than the supplied bound.
        ///
        /// @tparam TAttributeSet AttributeSet being evaluated.
        template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept {
            if constexpr (!TAttributeSet::template Contains<TName>) {
                return false;
            } else {
                using AttributeType = typename TAttributeSet::template Resolve<TName>::Type;
                constexpr auto actualValue = Detail::AttributeTraits<AttributeType>::Value;

                return Detail::AttributeValueAtLeast<
                    actualValue,
                    TMinimum
                >();
            }
        }

    };


    /// Requires one ordered Attribute value to be less than or equal to the supplied value.
    ///
    /// @tparam TName Attribute name being evaluated.
    /// @tparam TMaximum Inclusive upper bound.
    template<FixedString TName, auto TMaximum>
    struct AttributeAtMost {

        // Attribute-constraint metadata.

        /// Marker identifying an Attribute constraint.
        using AttributeConstraintTag = void;


        // Constraint evaluation.

        /// Indicates whether the Attribute is present, orderable and no larger than the supplied bound.
        ///
        /// @tparam TAttributeSet AttributeSet being evaluated.
        template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept {
            if constexpr (!TAttributeSet::template Contains<TName>) {
                return false;
            } else {
                using AttributeType = typename TAttributeSet::template Resolve<TName>::Type;
                constexpr auto actualValue = Detail::AttributeTraits<AttributeType>::Value;

                return Detail::AttributeValueAtMost<
                    actualValue,
                    TMaximum
                >();
            }
        }

    };


    /// Requires one ordered Attribute value to be strictly greater than the supplied value.
    ///
    /// @tparam TName Attribute name being evaluated.
    /// @tparam TMinimum Exclusive lower bound.
    template<FixedString TName, auto TMinimum>
    struct AttributeGreaterThan {

        // Attribute-constraint metadata.

        /// Marker identifying an Attribute constraint.
        using AttributeConstraintTag = void;


        // Constraint evaluation.

        /// Indicates whether the Attribute is present, orderable and greater than the supplied bound.
        ///
        /// @tparam TAttributeSet AttributeSet being evaluated.
        template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept {
            if constexpr (!TAttributeSet::template Contains<TName>) {
                return false;
            } else {
                using AttributeType = typename TAttributeSet::template Resolve<TName>::Type;
                constexpr auto actualValue = Detail::AttributeTraits<AttributeType>::Value;

                return Detail::AttributeValueGreaterThan<
                    actualValue,
                    TMinimum
                >();
            }
        }

    };


    /// Requires one ordered Attribute value to be strictly less than the supplied value.
    ///
    /// @tparam TName Attribute name being evaluated.
    /// @tparam TMaximum Exclusive upper bound.
    template<FixedString TName, auto TMaximum>
    struct AttributeLessThan {

        // Attribute-constraint metadata.

        /// Marker identifying an Attribute constraint.
        using AttributeConstraintTag = void;


        // Constraint evaluation.

        /// Indicates whether the Attribute is present, orderable and less than the supplied bound.
        ///
        /// @tparam TAttributeSet AttributeSet being evaluated.
        template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept {
            if constexpr (!TAttributeSet::template Contains<TName>) {
                return false;
            } else {
                using AttributeType = typename TAttributeSet::template Resolve<TName>::Type;
                constexpr auto actualValue = Detail::AttributeTraits<AttributeType>::Value;

                return Detail::AttributeValueLessThan<
                    actualValue,
                    TMaximum
                >();
            }
        }

    };


    /// Requires one ordered Attribute value to fall within the supplied inclusive interval.
    ///
    /// @tparam TName Attribute name being evaluated.
    /// @tparam TMinimum Inclusive lower bound.
    /// @tparam TMaximum Inclusive upper bound.
    template<FixedString TName, auto TMinimum, auto TMaximum>
    struct AttributeBetween {

        // Attribute-constraint metadata.

        /// Marker identifying an Attribute constraint.
        using AttributeConstraintTag = void;


        // Constraint evaluation.

        /// Indicates whether the Attribute is present, orderable and inside the inclusive interval.
        ///
        /// @tparam TAttributeSet AttributeSet being evaluated.
        template<class TAttributeSet>
        static constexpr bool IsSatisfied() noexcept {
            if constexpr (!TAttributeSet::template Contains<TName>) {
                return false;
            } else {
                using AttributeType = typename TAttributeSet::template Resolve<TName>::Type;
                constexpr auto actualValue = Detail::AttributeTraits<AttributeType>::Value;

                return Detail::AttributeValueAtLeast<
                    actualValue,
                    TMinimum
                >() &&
                Detail::AttributeValueAtMost<
                    actualValue,
                    TMaximum
                >();
            }
        }

    };


    namespace Detail {

        /// Safely compares two compile-time Attribute values for equality.
        ///
        /// @tparam TLeftValue Left compile-time value.
        /// @tparam TRightValue Right compile-time value.
        template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValuesEqual() noexcept {
            if constexpr (requires { TLeftValue == TRightValue; }) {
                return TLeftValue == TRightValue;
            } else {
                return false;
            }
        }


        /// Safely compares two compile-time Attribute values for inequality.
        ///
        /// @tparam TLeftValue Left compile-time value.
        /// @tparam TRightValue Right compile-time value.
        template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValuesNotEqual() noexcept {
            if constexpr (requires { TLeftValue != TRightValue; }) {
                return TLeftValue != TRightValue;
            } else {
                return false;
            }
        }


        /// Safely compares two compile-time Attribute values using greater-than-or-equal ordering.
        ///
        /// @tparam TLeftValue Left compile-time value.
        /// @tparam TRightValue Right compile-time value.
        template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValueAtLeast() noexcept {
            if constexpr (requires { TLeftValue >= TRightValue; }) {
                return TLeftValue >= TRightValue;
            } else {
                return false;
            }
        }


        /// Safely compares two compile-time Attribute values using less-than-or-equal ordering.
        ///
        /// @tparam TLeftValue Left compile-time value.
        /// @tparam TRightValue Right compile-time value.
        template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValueAtMost() noexcept {
            if constexpr (requires { TLeftValue <= TRightValue; }) {
                return TLeftValue <= TRightValue;
            } else {
                return false;
            }
        }


        /// Safely compares two compile-time Attribute values using strict greater-than ordering.
        ///
        /// @tparam TLeftValue Left compile-time value.
        /// @tparam TRightValue Right compile-time value.
        template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValueGreaterThan() noexcept {
            if constexpr (requires { TLeftValue > TRightValue; }) {
                return TLeftValue > TRightValue;
            } else {
                return false;
            }
        }


        /// Safely compares two compile-time Attribute values using strict less-than ordering.
        ///
        /// @tparam TLeftValue Left compile-time value.
        /// @tparam TRightValue Right compile-time value.
        template<auto TLeftValue, auto TRightValue>
        consteval bool AttributeValueLessThan() noexcept {
            if constexpr (requires { TLeftValue < TRightValue; }) {
                return TLeftValue < TRightValue;
            } else {
                return false;
            }
        }


        /// Default logical-constraint metadata for unrelated Types.
        ///
        /// @tparam TConstraint Type being inspected.
        /// @tparam TEnable SFINAE helper used when logical-constraint metadata is present.
        template<class TConstraint, class TEnable = void>
        struct LogicalConstraintTraits {

            // Constraint metadata.

            /// Indicates whether the inspected Type is a logical constraint expression.
            static constexpr bool IsValid = false;

        };


        /// Extracts logical-constraint metadata.
        ///
        /// @tparam TConstraint Logical constraint Type being inspected.
        template<class TConstraint>
        struct LogicalConstraintTraits<
            TConstraint,
            std::void_t<typename TConstraint::LogicalConstraintTag>
        > {

            // Constraint metadata.

            /// Indicates whether the inspected Type is a logical constraint expression.
            static constexpr bool IsValid = true;

        };


        /// Indicates whether a Requirement constraint can be applied to the supplied Capability.
        ///
        /// @tparam TCapability Capability being qualified.
        /// @tparam TConstraint Constraint expression being inspected.
        template<class TCapability, class TConstraint>
        consteval bool RequirementConstraintAppliesToCapability() noexcept {
            if constexpr (IsConstraintV<TConstraint>) {
                return IsConstraintForV<TCapability, TConstraint>;
            } else if constexpr (IsAttributeConstraintV<TConstraint>) {
                return true;
            } else if constexpr (LogicalConstraintTraits<TConstraint>::IsValid) {
                return TConstraint::template AppliesTo<TCapability>();
            } else {
                return false;
            }
        }


        /// Evaluates one Requirement constraint against one complete capability Offer.
        ///
        /// @tparam TConstraint Constraint expression being evaluated.
        /// @tparam TOffer Capability Offer being inspected.
        template<class TConstraint, class TOffer>
        consteval bool RequirementConstraintSatisfiedByOffer() noexcept {
            if constexpr (IsConstraintV<TConstraint>) {
                return TConstraint::template IsSatisfied<typename TOffer::Properties>();
            } else if constexpr (IsAttributeConstraintV<TConstraint>) {
                return TConstraint::template IsSatisfied<typename TOffer::Attributes>();
            } else if constexpr (LogicalConstraintTraits<TConstraint>::IsValid) {
                return TConstraint::template IsSatisfied<TOffer>();
            } else {
                return false;
            }
        }

    } // ESPressio::System::CompositionFramework::Detail


    /// Requires every nested constraint expression to be satisfied.
    ///
    /// @tparam TConstraints Nested Requirement constraints.
    template<class... TConstraints>
    struct AllOf {

        static_assert(
            sizeof...(TConstraints) > 0U,
            "AllOf requires at least one nested constraint"
        );

        // Logical-constraint metadata.

        /// Marker identifying a logical Requirement constraint.
        using LogicalConstraintTag = void;


        // Capability validation.

        /// Indicates whether every nested constraint can be applied to the supplied Capability.
        ///
        /// @tparam TCapability Capability being qualified.
        template<class TCapability>
        static consteval bool AppliesTo() noexcept {
            return (
                Detail::RequirementConstraintAppliesToCapability<
                    TCapability,
                    TConstraints
                >() &&
                ...
            );
        }


        // Constraint evaluation.

        /// Indicates whether every nested constraint is satisfied by one Offer.
        ///
        /// @tparam TOffer Offer being evaluated.
        template<class TOffer>
        static consteval bool IsSatisfied() noexcept {
            return (
                Detail::RequirementConstraintSatisfiedByOffer<
                    TConstraints,
                    TOffer
                >() &&
                ...
            );
        }

    };


    /// Requires at least one nested constraint expression to be satisfied.
    ///
    /// @tparam TConstraints Nested Requirement constraints.
    template<class... TConstraints>
    struct AnyOf {

        static_assert(
            sizeof...(TConstraints) > 0U,
            "AnyOf requires at least one nested constraint"
        );

        // Logical-constraint metadata.

        /// Marker identifying a logical Requirement constraint.
        using LogicalConstraintTag = void;


        // Capability validation.

        /// Indicates whether every nested expression is meaningful for the supplied Capability.
        ///
        /// @tparam TCapability Capability being qualified.
        template<class TCapability>
        static consteval bool AppliesTo() noexcept {
            return (
                Detail::RequirementConstraintAppliesToCapability<
                    TCapability,
                    TConstraints
                >() &&
                ...
            );
        }


        // Constraint evaluation.

        /// Indicates whether at least one nested constraint is satisfied by one Offer.
        ///
        /// @tparam TOffer Offer being evaluated.
        template<class TOffer>
        static consteval bool IsSatisfied() noexcept {
            return (
                Detail::RequirementConstraintSatisfiedByOffer<
                    TConstraints,
                    TOffer
                >() ||
                ...
            );
        }

    };


    /// Negates one nested Requirement constraint.
    ///
    /// @tparam TConstraint Nested Requirement constraint.
    template<class TConstraint>
    struct Not {

        // Logical-constraint metadata.

        /// Marker identifying a logical Requirement constraint.
        using LogicalConstraintTag = void;


        // Capability validation.

        /// Indicates whether the nested constraint can be applied to the supplied Capability.
        ///
        /// @tparam TCapability Capability being qualified.
        template<class TCapability>
        static consteval bool AppliesTo() noexcept {
            return Detail::RequirementConstraintAppliesToCapability<
                TCapability,
                TConstraint
            >();
        }


        // Constraint evaluation.

        /// Indicates whether the nested constraint is not satisfied by one Offer.
        ///
        /// @tparam TOffer Offer being evaluated.
        template<class TOffer>
        static consteval bool IsSatisfied() noexcept {
            return !Detail::RequirementConstraintSatisfiedByOffer<
                TConstraint,
                TOffer
            >();
        }

    };


} // ESPressio::System::CompositionFramework
