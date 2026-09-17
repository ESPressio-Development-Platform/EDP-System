#pragma once

#include <cstddef>
#include <type_traits>

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

    } // ESPressio::System::CompositionFramework::Detail


    // Constraint inspection.

    /// Indicates whether a type represents a valid property constraint.
    template<class TConstraint>
    inline constexpr bool IsConstraintV = Detail::ConstraintTraits<TConstraint>::IsValid;

    /// Indicates whether a constraint applies to the specified capability.
    template<class TCapability, class TConstraint>
    inline constexpr bool IsConstraintForV =
        IsConstraintV<TConstraint> &&
        std::is_same_v<typename TConstraint::CapabilityType, TCapability>;


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
            }

            return TPropertySet::template Value<TProperty> == ExpectedValue;
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
            }

            return TPropertySet::template Value<TProperty> >= ExpectedValue;
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
            }

            return TPropertySet::template Value<TProperty> <= ExpectedValue;
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
            }

            return TPropertySet::template Value<TProperty> > ExpectedValue;
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
            }

            return TPropertySet::template Value<TProperty> < ExpectedValue;
        }

    };


    /// Declares one capability required by a provider, optionally constrained by capability properties.
    template<class TCapability, class... TConstraints>
    struct Need {

        static_assert(
            IsCapabilityV<TCapability>,
            "Need requires a concrete composition capability"
        );

        static_assert(
            (IsConstraintForV<TCapability, TConstraints> && ...),
            "Need contains a property constraint that belongs to another capability"
        );

        // Requirement metadata.

        /// Marker used to identify Need declarations during compile-time inspection.
        using NeedTag = void;

        /// Capability required by this declaration.
        using CapabilityType = TCapability;

        /// Domain inherited from the required capability.
        using CompositionDomain = typename Detail::CapabilityTraits<TCapability>::DomainType;

        /// Number of property constraints attached to this requirement.
        static constexpr std::size_t ConstraintCount = sizeof...(TConstraints);

        // Requirement evaluation.

        /// Indicates whether a supplied property set satisfies every constraint attached to this requirement.
        template<class TPropertySet>
        static constexpr bool PropertiesSatisfied = (TConstraints::template IsSatisfied<TPropertySet>() && ...);

    };


    /// Groups the capabilities required by a provider.
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

} // ESPressio::System::CompositionFramework
