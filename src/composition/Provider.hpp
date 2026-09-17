#pragma once

#include <cstddef>
#include <type_traits>

#include "Requirement.hpp"

namespace ESPressio::System::CompositionFramework {

    /// Declares one capability and the compile-time properties advertised for that capability by a provider.
    template<class TCapability, class... TPropertyValues>
    struct Offer {

        static_assert(
            IsCapabilityV<TCapability>,
            "Offer requires a concrete composition capability"
        );

        // Offer metadata.

        /// Marker used to identify Offer declarations during compile-time inspection.
        using OfferTag = void;

        /// Capability supplied by this offer.
        using CapabilityType = TCapability;

        /// Domain inherited from the supplied capability.
        using CompositionDomain = typename Detail::CapabilityTraits<TCapability>::DomainType;

        /// Compile-time properties advertised for the supplied capability.
        using Properties = PropertySet<TCapability, TPropertyValues...>;

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
                typename TOffer::Properties
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


        /// Returns the compile-time property set advertised for the specified capability.
        template<class TCapability>
        using PropertiesFor = typename ResolveOffer<TCapability>::Type::Properties;

    };


    /// Declares the compile-time capabilities supplied and required by one concrete provider type.
    template<class TDomain, class TProvides, class TRequires = Requires<>>
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
            "Provider requires a capability that belongs to another composition domain"
        );

        // Provider metadata.

        /// Marker used to identify provider declarations during compile-time inspection.
        using ProviderDeclarationTag = void;

        /// Domain to which this provider belongs.
        using CompositionDomain = TDomain;

        /// Capabilities supplied by this provider.
        using CompositionCapabilities = TProvides;

        /// Capabilities required by this provider.
        using CompositionRequirements = TRequires;

    };

} // ESPressio::System::CompositionFramework
