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


        /// Default requirement metadata for types that are not Need declarations.
        template<class TNeed, class = void>
        struct NeedTraits {

            // Requirement metadata.

            /// Indicates whether the inspected type is a valid capability requirement.
            static constexpr bool IsValid = false;

        };


        /// Extracts metadata from a Need declaration.
        template<class TNeed>
        struct NeedTraits<
            TNeed,
            std::void_t<
                typename TNeed::NeedTag,
                typename TNeed::CapabilityType,
                typename TNeed::CompositionDomain
            >
        > {

            // Requirement metadata.

            /// Indicates whether the inspected requirement references a valid capability.
            static constexpr bool IsValid = IsCapabilityV<typename TNeed::CapabilityType>;

        };


        /// Indicates whether every required capability in a requirement pack is unique.
        template<class... TNeeds>
        struct UniqueNeeds;


        /// Empty requirement packs are unique.
        template<>
        struct UniqueNeeds<> : std::true_type {};


        /// Checks the first requirement against the remaining requirements and continues recursively.
        template<class TFirstNeed, class... TRestNeeds>
        struct UniqueNeeds<TFirstNeed, TRestNeeds...> : std::bool_constant<
            ((!std::is_same_v<typename TFirstNeed::CapabilityType, typename TRestNeeds::CapabilityType>) && ...) &&
            UniqueNeeds<TRestNeeds...>::value
        > {};


        /// Evaluates one constraint against a property-only set for backward-compatible Need inspection.
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


        /// Default cross-domain dependency metadata for types that are not DependsOn declarations.
        template<class TDependsOn, class = void>
        struct DependsOnTraits {

            // Dependency metadata.

            /// Indicates whether the inspected type is a valid DependsOn declaration.
            static constexpr bool IsValid = false;

        };


        /// Extracts metadata from a DependsOn declaration.
        template<class TDependsOn>
        struct DependsOnTraits<
            TDependsOn,
            std::void_t<typename TDependsOn::DependsOnTag>
        > {

            // Dependency metadata.

            /// Indicates whether the inspected type is a valid DependsOn declaration.
            static constexpr bool IsValid = true;

        };

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

                if constexpr (requires { actualValue != TUnexpectedValue; }) {
                    return actualValue != TUnexpectedValue;
                }

                return false;
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
                    (
                        requires { actualValue == TAllowedValues; } &&
                        actualValue == TAllowedValues
                    ) ||
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
                    (
                        requires { actualValue != TExcludedValues; } &&
                        actualValue != TExcludedValues
                    ) &&
                    ...
                );
            }
        }

    };


    namespace Detail {

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


    /// Declares one capability required by a provider, optionally constrained by properties and open-ended Attributes.
    template<class TCapability, class... TConstraints>
    struct Need {

        static_assert(
            IsCapabilityV<TCapability>,
            "Need requires a concrete composition capability"
        );

        static_assert(
            (Detail::RequirementConstraintAppliesToCapability<TCapability, TConstraints>() && ...),
            "Need contains a constraint that cannot be applied to the requested capability"
        );

        // Requirement metadata.

        /// Marker used to identify Need declarations during compile-time inspection.
        using NeedTag = void;

        /// Capability required by this declaration.
        using CapabilityType = TCapability;

        /// Domain inherited from the required capability.
        using CompositionDomain = typename Detail::CapabilityTraits<TCapability>::DomainType;

        /// Number of property and Attribute constraints attached to this requirement.
        static constexpr std::size_t ConstraintCount = sizeof...(TConstraints);

        // Requirement evaluation.

        /// Indicates whether a supplied property set satisfies every property constraint attached to this requirement.
        template<class TPropertySet>
        static constexpr bool PropertiesSatisfied = (Detail::ConstraintSatisfiedByProperties<TConstraints, TPropertySet>::value && ...);

        /// Indicates whether one complete capability Offer satisfies every constraint attached to this requirement.
        template<class TOffer>
        static constexpr bool OfferSatisfied = (
            Detail::RequirementConstraintSatisfiedByOffer<
                TConstraints,
                TOffer
            >() &&
            ...
        );

    };


    /// Groups the same-domain capabilities required by one provider.
    template<class... TNeeds>
    struct Requires {

        static_assert(
            (Detail::NeedTraits<TNeeds>::IsValid && ...),
            "Requires entries must be Need declarations"
        );

        static_assert(
            Detail::UniqueNeeds<TNeeds...>::value,
            "Requires contains the same capability more than once"
        );

        // Requirement-set metadata.

        /// Marker used to identify Requires declarations during compile-time inspection.
        using RequiresTag = void;

        /// Number of capability requirements contained in this declaration.
        static constexpr std::size_t Count = sizeof...(TNeeds);

        // Domain inspection.

        /// Indicates whether every requirement belongs to the specified composition domain.
        template<class TDomain>
        static constexpr bool IsForDomain = (std::is_same_v<typename TNeeds::CompositionDomain, TDomain> && ...);

    };


    /// Groups cross-domain capability dependencies required by one provider.
    template<class... TNeeds>
    struct DependsOn {

        static_assert(
            (Detail::NeedTraits<TNeeds>::IsValid && ...),
            "DependsOn entries must be Need declarations"
        );

        // Dependency-set metadata.

        /// Marker used to identify DependsOn declarations during compile-time inspection.
        using DependsOnTag = void;

        /// Number of cross-domain capability dependencies contained in this declaration.
        static constexpr std::size_t Count = sizeof...(TNeeds);

        // Domain inspection.

        /// Indicates whether every dependency belongs to a domain other than the specified provider domain.
        template<class TDomain>
        static constexpr bool IsExternalTo = ((!std::is_same_v<typename TNeeds::CompositionDomain, TDomain>) && ...);

    };

} // ESPressio::System::CompositionFramework
