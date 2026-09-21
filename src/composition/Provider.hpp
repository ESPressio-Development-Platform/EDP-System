#pragma once

#include <cstddef>
#include <type_traits>

#include "Requirement.hpp"

namespace ESPressio::System::CompositionFramework {

    namespace Detail {

        /// Stores a temporary compile-time type list while Offer characteristics are partitioned.
        template<class... TTypes>
        struct TypeList {};


        /// Appends one type to a compile-time type list.
        template<class TList, class TType>
        struct AppendType;


        /// Appends one type to the end of a compile-time type list.
        template<class... TTypes, class TType>
        struct AppendType<TypeList<TTypes...>, TType> {

            // List result.

            /// Type list containing the previous entries followed by the appended type.
            using Type = TypeList<TTypes..., TType>;

        };


        /// Collects only PropertyValue declarations from an Offer characteristic pack.
        template<class TAccumulatedProperties, class... TCharacteristics>
        struct CollectPropertyValues;


        /// Completes PropertyValue collection when no Offer characteristics remain.
        template<class TAccumulatedProperties>
        struct CollectPropertyValues<TAccumulatedProperties> {

            // Collection result.

            /// Accumulated PropertyValue type list.
            using Type = TAccumulatedProperties;

        };


        /// Inspects one Offer characteristic and continues PropertyValue collection.
        template<class TAccumulatedProperties, class TFirstCharacteristic, class... TRestCharacteristics>
        struct CollectPropertyValues<TAccumulatedProperties, TFirstCharacteristic, TRestCharacteristics...> {

            // Collection state.

            /// Next PropertyValue list after conditionally appending the current characteristic.
            using NextProperties = std::conditional_t<
                PropertyValueTraits<TFirstCharacteristic>::IsValid,
                typename AppendType<TAccumulatedProperties, TFirstCharacteristic>::Type,
                TAccumulatedProperties
            >;

            /// Final collected PropertyValue list.
            using Type = typename CollectPropertyValues<NextProperties, TRestCharacteristics...>::Type;

        };


        /// Collects only Attribute declarations from an Offer characteristic pack.
        template<class TAccumulatedAttributes, class... TCharacteristics>
        struct CollectAttributes;


        /// Completes Attribute collection when no Offer characteristics remain.
        template<class TAccumulatedAttributes>
        struct CollectAttributes<TAccumulatedAttributes> {

            // Collection result.

            /// Accumulated Attribute type list.
            using Type = TAccumulatedAttributes;

        };


        /// Inspects one Offer characteristic and continues Attribute collection.
        template<class TAccumulatedAttributes, class TFirstCharacteristic, class... TRestCharacteristics>
        struct CollectAttributes<TAccumulatedAttributes, TFirstCharacteristic, TRestCharacteristics...> {

            // Collection state.

            /// Next Attribute list after conditionally appending the current characteristic.
            using NextAttributes = std::conditional_t<
                IsAttributeV<TFirstCharacteristic>,
                typename AppendType<TAccumulatedAttributes, TFirstCharacteristic>::Type,
                TAccumulatedAttributes
            >;

            /// Final collected Attribute list.
            using Type = typename CollectAttributes<NextAttributes, TRestCharacteristics...>::Type;

        };


        /// Converts one compile-time type list into a PropertySet for a specific capability.
        template<class TCapability, class TPropertyList>
        struct MakePropertySet;


        /// Constructs a PropertySet from the collected PropertyValue declarations.
        template<class TCapability, class... TPropertyValues>
        struct MakePropertySet<TCapability, TypeList<TPropertyValues...>> {

            // Conversion result.

            /// PropertySet containing the collected PropertyValue declarations.
            using Type = PropertySet<TCapability, TPropertyValues...>;

        };


        /// Converts one compile-time type list into an AttributeSet.
        template<class TAttributeList>
        struct MakeAttributeSet;


        /// Constructs an AttributeSet from the collected Attribute declarations.
        template<class... TAttributes>
        struct MakeAttributeSet<TypeList<TAttributes...>> {

            // Conversion result.

            /// AttributeSet containing the collected Attribute declarations.
            using Type = AttributeSet<TAttributes...>;

        };


        /// Determines whether one valid PropertyValue characteristic belongs to the supplied capability.
        template<class TCapability, class TCharacteristic, bool TIsPropertyValue = PropertyValueTraits<TCharacteristic>::IsValid>
        struct IsOfferPropertyFor : std::false_type {};


        /// Compares the capability carried by a valid PropertyValue with the supplied capability.
        template<class TCapability, class TCharacteristic>
        struct IsOfferPropertyFor<TCapability, TCharacteristic, true> : std::bool_constant<
            std::is_same_v<typename TCharacteristic::CapabilityType, TCapability>
        > {};


        /// Indicates whether one Offer characteristic is valid for the supplied capability.
        template<class TCapability, class TCharacteristic>
        inline constexpr bool IsOfferCharacteristicForV =
            IsAttributeV<TCharacteristic> ||
            IsOfferPropertyFor<TCapability, TCharacteristic>::value;

    } // ESPressio::System::CompositionFramework::Detail


    /// Declares one capability and the compile-time properties and Attributes advertised for that capability by a provider.
    template<class TCapability, class... TCharacteristics>
    struct Offer {

        static_assert(
            IsCapabilityV<TCapability>,
            "Offer requires a concrete composition capability"
        );

        static_assert(
            (Detail::IsOfferCharacteristicForV<TCapability, TCharacteristics> && ...),
            "Offer characteristics must be PropertyValue or Attribute declarations valid for the supplied capability"
        );

        // Offer metadata.

        /// Marker used to identify Offer declarations during compile-time inspection.
        using OfferTag = void;

        /// Capability supplied by this offer.
        using CapabilityType = TCapability;

        /// Domain inherited from the supplied capability.
        using CompositionDomain = typename Detail::CapabilityTraits<TCapability>::DomainType;

        /// Compile-time properties advertised for the supplied capability.
        using Properties = typename Detail::MakePropertySet<
            TCapability,
            typename Detail::CollectPropertyValues<Detail::TypeList<>, TCharacteristics...>::Type
        >::Type;

        /// Open-ended compile-time Attributes advertised for the supplied capability.
        using Attributes = typename Detail::MakeAttributeSet<
            typename Detail::CollectAttributes<Detail::TypeList<>, TCharacteristics...>::Type
        >::Type;

    };


    namespace Detail {

        /// Default offer metadata for types that are not Offer declarations.
        template<class TOffer, class = void>
        struct OfferTraits {

            // Offer metadata.

            /// Indicates whether the inspected type is a valid Offer declaration.
            static constexpr bool IsValid = false;

        };


        /// Extracts metadata from an Offer declaration.
        template<class TOffer>
        struct OfferTraits<
            TOffer,
            std::void_t<
                typename TOffer::OfferTag,
                typename TOffer::CapabilityType,
                typename TOffer::CompositionDomain,
                typename TOffer::Properties,
                typename TOffer::Attributes
            >
        > {

            // Offer metadata.

            /// Indicates whether the inspected offer references a valid capability.
            static constexpr bool IsValid = IsCapabilityV<typename TOffer::CapabilityType>;

        };


        /// Indicates whether every offered capability in an offer pack is unique.
        template<class... TOffers>
        struct UniqueOffers;


        /// Empty offer packs are unique.
        template<>
        struct UniqueOffers<> : std::true_type {};


        /// Checks the first offer against the remaining offers and continues recursively.
        template<class TFirstOffer, class... TRestOffers>
        struct UniqueOffers<TFirstOffer, TRestOffers...> : std::bool_constant<
            ((!std::is_same_v<typename TFirstOffer::CapabilityType, typename TRestOffers::CapabilityType>) && ...) &&
            UniqueOffers<TRestOffers...>::value
        > {};


        /// Locates an offer by its capability type within a compile-time offer pack.
        template<class TCapability, class... TOffers>
        struct FindOffer;


        /// Represents an unsuccessful offer lookup.
        template<class TCapability>
        struct FindOffer<TCapability> {

            // Lookup result.

            /// Type returned when no matching offer exists.
            using Type = void;

        };


        /// Continues an offer lookup until a matching capability is found.
        template<class TCapability, class TFirstOffer, class... TRestOffers>
        struct FindOffer<TCapability, TFirstOffer, TRestOffers...> {

            // Lookup result.

            /// Matching Offer declaration, or the result of searching the remaining declarations.
            using Type = std::conditional_t<
                std::is_same_v<TCapability, typename TFirstOffer::CapabilityType>,
                TFirstOffer,
                typename FindOffer<TCapability, TRestOffers...>::Type
            >;

        };

    } // ESPressio::System::CompositionFramework::Detail


    /// Groups the capabilities supplied by a provider.
    template<class... TOffers>
    struct Provides {

        static_assert(
            sizeof...(TOffers) > 0U,
            "Provides must contain at least one Offer"
        );

        static_assert(
            (Detail::OfferTraits<TOffers>::IsValid && ...),
            "Provides entries must be Offer declarations"
        );

        static_assert(
            Detail::UniqueOffers<TOffers...>::value,
            "Provides contains the same capability more than once"
        );

        // Offer-set metadata.

        /// Marker used to identify Provides declarations during compile-time inspection.
        using ProvidesTag = void;

        /// Number of capability offers contained in this declaration.
        static constexpr std::size_t Count = sizeof...(TOffers);

        // Domain inspection.

        /// Indicates whether every offered capability belongs to the specified composition domain.
        template<class TDomain>
        static constexpr bool IsForDomain = (std::is_same_v<typename TOffers::CompositionDomain, TDomain> && ...);

        // Capability queries.

        /// Indicates whether this declaration supplies the specified capability.
        template<class TCapability>
        static constexpr bool Contains = (std::is_same_v<TCapability, typename TOffers::CapabilityType> || ...);

        /// Resolves one capability offer from this declaration.
        template<class TCapability>
        struct ResolveOffer {

            static_assert(
                Contains<TCapability>,
                "Requested capability is not supplied by this Provides declaration"
            );

            // Resolution result.

            /// Offer declaration associated with the requested capability.
            using Type = typename Detail::FindOffer<TCapability, TOffers...>::Type;

        };


        /// Returns the complete Offer declaration associated with the specified capability.
        template<class TCapability>
        using OfferFor = typename ResolveOffer<TCapability>::Type;

        /// Returns the compile-time property set advertised for the specified capability.
        template<class TCapability>
        using PropertiesFor = typename OfferFor<TCapability>::Properties;

        /// Returns the open-ended compile-time Attribute set advertised for the specified capability.
        template<class TCapability>
        using AttributesFor = typename OfferFor<TCapability>::Attributes;

    };


    /// Declares the compile-time capabilities supplied and required by one concrete provider type.
    template<
        class TDomain,
        class TProvides,
        class TRequires = Requires<>,
        class TDependsOn = DependsOn<>
    >
    struct Provider {

        static_assert(
            IsDomainV<TDomain>,
            "Provider requires a concrete composition Domain"
        );

        static_assert(
            TProvides::template IsForDomain<TDomain>,
            "Provider supplies a capability that belongs to another composition domain"
        );

        static_assert(
            TRequires::template IsForDomain<TDomain>,
            "Provider requires a same-domain capability that belongs to another composition domain"
        );

        static_assert(
            Detail::DependsOnTraits<TDependsOn>::IsValid,
            "Provider cross-domain dependencies must use a DependsOn declaration"
        );

        static_assert(
            TDependsOn::template IsExternalTo<TDomain>,
            "Provider DependsOn entries must belong to another composition domain; use Requires for same-domain requirements"
        );

        // Provider metadata.

        /// Marker used to identify provider declarations during compile-time inspection.
        using ProviderDeclarationTag = void;

        /// Domain to which this provider belongs.
        using CompositionDomain = TDomain;

        /// Capabilities supplied by this provider.
        using CompositionCapabilities = TProvides;

        /// Same-domain capabilities required by this provider.
        using CompositionRequirements = TRequires;

        /// Cross-domain capabilities required by this provider.
        using CompositionDependencies = TDependsOn;

    };

} // ESPressio::System::CompositionFramework
