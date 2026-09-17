#pragma once

#include <type_traits>

namespace ESPressio::System::CompositionFramework {

    /// Marker base for a compile-time composition domain.
    struct Domain {};


    // Domain inspection.

    /// Indicates whether a type represents a concrete composition domain.
    template<class TDomain>
    inline constexpr bool IsDomainV = std::is_base_of_v<Domain, TDomain> && !std::is_same_v<Domain, TDomain>;


    /// Base declaration for a capability that may have no more than one provider in a composition.
    template<class TDomain>
    struct ExclusiveCapability {

        static_assert(
            IsDomainV<TDomain>,
            "ExclusiveCapability requires a concrete composition Domain"
        );

        // Composition metadata.

        /// Domain to which this capability belongs.
        using CompositionDomain = TDomain;

    };


    /// Base declaration for a capability that may have multiple providers in a composition.
    template<class TDomain>
    struct SharedCapability {

        static_assert(
            IsDomainV<TDomain>,
            "SharedCapability requires a concrete composition Domain"
        );

        // Composition metadata.

        /// Domain to which this capability belongs.
        using CompositionDomain = TDomain;

    };


    namespace Detail {

        /// Default capability metadata for types that are not composition capabilities.
        template<class TCapability, class = void>
        struct CapabilityTraits {

            // Capability metadata.

            /// Domain associated with the inspected type when one exists.
            using DomainType = void;

            /// Indicates whether the inspected type is a valid concrete capability.
            static constexpr bool IsValid = false;

            /// Indicates whether the inspected capability is exclusive.
            static constexpr bool IsExclusive = false;

            /// Indicates whether the inspected capability is shared.
            static constexpr bool IsShared = false;

        };


        /// Extracts capability metadata from types that declare a composition domain.
        template<class TCapability>
        struct CapabilityTraits<TCapability, std::void_t<typename TCapability::CompositionDomain>> {

            // Capability metadata.

            /// Domain associated with the inspected capability.
            using DomainType = typename TCapability::CompositionDomain;

            /// Indicates whether the inspected type derives from the exclusive capability declaration for its domain.
            static constexpr bool IsExclusive =
                IsDomainV<DomainType> &&
                std::is_base_of_v<ExclusiveCapability<DomainType>, TCapability> &&
                !std::is_same_v<ExclusiveCapability<DomainType>, TCapability>;

            /// Indicates whether the inspected type derives from the shared capability declaration for its domain.
            static constexpr bool IsShared =
                IsDomainV<DomainType> &&
                std::is_base_of_v<SharedCapability<DomainType>, TCapability> &&
                !std::is_same_v<SharedCapability<DomainType>, TCapability>;

            /// Indicates whether the inspected type is a valid concrete capability.
            static constexpr bool IsValid = IsExclusive || IsShared;

        };

    } // ESPressio::System::CompositionFramework::Detail


    // Capability inspection.

    /// Indicates whether a type represents a concrete composition capability.
    template<class TCapability>
    inline constexpr bool IsCapabilityV = Detail::CapabilityTraits<TCapability>::IsValid;

    /// Indicates whether a capability belongs to the specified composition domain.
    template<class TDomain, class TCapability>
    inline constexpr bool IsCapabilityForV =
        IsCapabilityV<TCapability> &&
        std::is_same_v<typename Detail::CapabilityTraits<TCapability>::DomainType, TDomain>;

    /// Indicates whether a capability is exclusive within the specified composition domain.
    template<class TDomain, class TCapability>
    inline constexpr bool IsExclusiveCapabilityForV =
        IsCapabilityForV<TDomain, TCapability> &&
        Detail::CapabilityTraits<TCapability>::IsExclusive;

    /// Indicates whether a capability is shared within the specified composition domain.
    template<class TDomain, class TCapability>
    inline constexpr bool IsSharedCapabilityForV =
        IsCapabilityForV<TDomain, TCapability> &&
        Detail::CapabilityTraits<TCapability>::IsShared;

} // ESPressio::System::CompositionFramework
