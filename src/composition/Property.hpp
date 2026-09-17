#pragma once

#include <cstddef>
#include <type_traits>

#include "Capability.hpp"

namespace ESPressio::System::CompositionFramework {

    /// Base declaration for a compile-time property associated with a capability.
    template<class TCapability, class TValue>
    struct Property {

        static_assert(
            IsCapabilityV<TCapability>,
            "Property requires a concrete composition capability"
        );

        // Property metadata.

        /// Capability to which this property belongs.
        using CapabilityType = TCapability;

        /// Domain inherited from the property's capability.
        using CompositionDomain = typename Detail::CapabilityTraits<TCapability>::DomainType;

        /// Value type used by this property.
        using ValueType = TValue;

    };


    namespace Detail {

        /// Default property metadata for types that are not composition properties.
        template<class TProperty, class = void>
        struct PropertyTraits {

            // Property metadata.

            /// Capability associated with the inspected type when one exists.
            using CapabilityType = void;

            /// Domain associated with the inspected type when one exists.
            using DomainType = void;

            /// Value type associated with the inspected type when one exists.
            using ValueType = void;

            /// Indicates whether the inspected type is a valid concrete property.
            static constexpr bool IsValid = false;

        };


        /// Extracts property metadata from types that declare the required property aliases.
        template<class TProperty>
        struct PropertyTraits<
            TProperty,
            std::void_t<
                typename TProperty::CapabilityType,
                typename TProperty::CompositionDomain,
                typename TProperty::ValueType
            >
        > {

            // Property metadata.

            /// Capability associated with the inspected property.
            using CapabilityType = typename TProperty::CapabilityType;

            /// Domain associated with the inspected property.
            using DomainType = typename TProperty::CompositionDomain;

            /// Value type associated with the inspected property.
            using ValueType = typename TProperty::ValueType;

            /// Indicates whether the inspected type is a concrete property derived from the matching Property declaration.
            static constexpr bool IsValid =
                IsCapabilityV<CapabilityType> &&
                std::is_same_v<typename CapabilityTraits<CapabilityType>::DomainType, DomainType> &&
                std::is_base_of_v<Property<CapabilityType, ValueType>, TProperty> &&
                !std::is_same_v<Property<CapabilityType, ValueType>, TProperty>;

        };


        /// Default property-value metadata for types that are not PropertyValue declarations.
        template<class TPropertyValue, class = void>
        struct PropertyValueTraits {

            // Property-value metadata.

            /// Indicates whether the inspected type is a valid PropertyValue declaration.
            static constexpr bool IsValid = false;

        };


        /// Extracts metadata from a PropertyValue declaration.
        template<class TPropertyValue>
        struct PropertyValueTraits<
            TPropertyValue,
            std::void_t<
                typename TPropertyValue::PropertyValueTag,
                typename TPropertyValue::PropertyType,
                typename TPropertyValue::CapabilityType
            >
        > {

            // Property-value metadata.

            /// Indicates whether the inspected type contains a valid property type.
            static constexpr bool IsValid = PropertyTraits<typename TPropertyValue::PropertyType>::IsValid;

        };


        /// Locates a property value by its property type within a compile-time property-value pack.
        template<class TProperty, class... TPropertyValues>
        struct FindPropertyValue;


        /// Represents an unsuccessful property-value lookup.
        template<class TProperty>
        struct FindPropertyValue<TProperty> {

            // Lookup result.

            /// Type returned when no matching property value exists.
            using Type = void;

        };


        /// Continues a property-value lookup until a matching property type is found.
        template<class TProperty, class TFirstPropertyValue, class... TRestPropertyValues>
        struct FindPropertyValue<TProperty, TFirstPropertyValue, TRestPropertyValues...> {

            // Lookup result.

            /// Matching PropertyValue declaration, or the result of searching the remaining declarations.
            using Type = std::conditional_t<
                std::is_same_v<TProperty, typename TFirstPropertyValue::PropertyType>,
                TFirstPropertyValue,
                typename FindPropertyValue<TProperty, TRestPropertyValues...>::Type
            >;

        };


        /// Indicates whether every property type in a PropertyValue pack is unique.
        template<class... TPropertyValues>
        struct UniquePropertyValues;


        /// Empty property-value packs are unique.
        template<>
        struct UniquePropertyValues<> : std::true_type {};


        /// Checks the first property value against the remaining property values and continues recursively.
        template<class TFirstPropertyValue, class... TRestPropertyValues>
        struct UniquePropertyValues<TFirstPropertyValue, TRestPropertyValues...> : std::bool_constant<
            ((!std::is_same_v<typename TFirstPropertyValue::PropertyType, typename TRestPropertyValues::PropertyType>) && ...) &&
            UniquePropertyValues<TRestPropertyValues...>::value
        > {};

    } // ESPressio::System::CompositionFramework::Detail


    // Property inspection.

    /// Indicates whether a type represents a concrete composition property.
    template<class TProperty>
    inline constexpr bool IsPropertyV = Detail::PropertyTraits<TProperty>::IsValid;

    /// Indicates whether a property belongs to the specified capability.
    template<class TCapability, class TProperty>
    inline constexpr bool IsPropertyForV =
        IsPropertyV<TProperty> &&
        std::is_same_v<typename Detail::PropertyTraits<TProperty>::CapabilityType, TCapability>;


    /// Associates a compile-time value with a concrete capability property.
    template<class TProperty, auto TPropertyValue>
    struct PropertyValue {

        static_assert(
            IsPropertyV<TProperty>,
            "PropertyValue requires a concrete composition Property"
        );

        static_assert(
            std::is_convertible_v<decltype(TPropertyValue), typename TProperty::ValueType>,
            "PropertyValue cannot be converted to the property's declared value type"
        );

        // Property-value metadata.

        /// Marker used to identify PropertyValue declarations during compile-time inspection.
        using PropertyValueTag = void;

        /// Property represented by this value.
        using PropertyType = TProperty;

        /// Capability to which the represented property belongs.
        using CapabilityType = typename TProperty::CapabilityType;

        /// Domain to which the represented property belongs.
        using CompositionDomain = typename TProperty::CompositionDomain;

        /// Value type declared by the represented property.
        using ValueType = typename TProperty::ValueType;

        /// Compile-time value advertised for the represented property.
        static constexpr ValueType Value = static_cast<ValueType>(TPropertyValue);

    };


    /// Stores the compile-time property values advertised for one capability offer.
    template<class TCapability, class... TPropertyValues>
    struct PropertySet {

        static_assert(
            IsCapabilityV<TCapability>,
            "PropertySet requires a concrete composition capability"
        );

        static_assert(
            (Detail::PropertyValueTraits<TPropertyValues>::IsValid && ...),
            "PropertySet entries must be PropertyValue declarations"
        );

        static_assert(
            (std::is_same_v<typename TPropertyValues::CapabilityType, TCapability> && ...),
            "PropertySet contains a property that belongs to another capability"
        );

        static_assert(
            Detail::UniquePropertyValues<TPropertyValues...>::value,
            "PropertySet contains the same property more than once"
        );

        // Property-set metadata.

        /// Capability described by this set of property values.
        using CapabilityType = TCapability;

        /// Domain inherited from the described capability.
        using CompositionDomain = typename Detail::CapabilityTraits<TCapability>::DomainType;

        /// Number of property values contained in this set.
        static constexpr std::size_t Count = sizeof...(TPropertyValues);

        // Property queries.

        /// Indicates whether this set contains a value for the specified property.
        template<class TProperty>
        static constexpr bool Contains = !std::is_void_v<typename Detail::FindPropertyValue<TProperty, TPropertyValues...>::Type>;

        /// Resolves one property value from this set.
        template<class TProperty>
        struct Resolve {

            static_assert(
                IsPropertyForV<TCapability, TProperty>,
                "Property query does not belong to this PropertySet capability"
            );

            static_assert(
                Contains<TProperty>,
                "Requested property is not present in this PropertySet"
            );

            // Resolution result.

            /// PropertyValue declaration associated with the requested property.
            using Entry = typename Detail::FindPropertyValue<TProperty, TPropertyValues...>::Type;

            /// Compile-time value associated with the requested property.
            static constexpr typename TProperty::ValueType Value = Entry::Value;

        };


        /// Returns the compile-time value associated with the specified property.
        template<class TProperty>
        static constexpr typename TProperty::ValueType Value = Resolve<TProperty>::Value;

    };

} // ESPressio::System::CompositionFramework
