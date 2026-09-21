#pragma once

#include <cstddef>
#include <type_traits>

#include "Provider.hpp"

namespace ESPressio::System::CompositionFramework {

    /// Stores a compile-time list of provider types returned by a Composition query.
    template<class... TProviders>
    struct ProviderList {

        // Provider-list metadata.

        /// Number of provider types contained in this list.
        static constexpr std::size_t Count = sizeof...(TProviders);

        // Provider queries.

        /// Indicates whether the specified provider type is contained in this list.
        template<class TProvider>
        static constexpr bool Contains = (std::is_same_v<TProvider, TProviders> || ...);

    };


    namespace Detail {

        /// Indicates whether a type pack contains the specified type.
        template<class TNeedle, class... THaystack>
        inline constexpr bool ContainsTypeV = (std::is_same_v<TNeedle, THaystack> || ...);


        /// Default provider metadata for types that are not provider declarations.
        template<class TDomain, class TProvider, class = void>
        struct IsProviderFor : std::false_type {};


        /// Validates a concrete provider type against a composition domain.
        template<class TDomain, class TProvider>
        struct IsProviderFor<
            TDomain,
            TProvider,
            std::void_t<
                typename TProvider::ProviderDeclarationTag,
                typename TProvider::CompositionDomain,
                typename TProvider::CompositionCapabilities,
                typename TProvider::CompositionRequirements,
                typename TProvider::CompositionDependencies
            >
        > : std::bool_constant<
            std::is_same_v<typename TProvider::CompositionDomain, TDomain> &&
            TProvider::CompositionCapabilities::template IsForDomain<TDomain> &&
            TProvider::CompositionRequirements::template IsForDomain<TDomain>
        > {};


        /// Indicates whether one provider supplies the requested capability.
        template<class TProvider, class TCapability>
        inline constexpr bool ProviderProvidesV = TProvider::CompositionCapabilities::template Contains<TCapability>;


        /// Counts providers that supply the requested capability.
        template<class TCapability, class... TProviders>
        inline constexpr std::size_t ProviderCountV =
            (std::size_t{0U} + ... + (ProviderProvidesV<TProviders, TCapability> ? std::size_t{1U} : std::size_t{0U}));


        /// Selects the first provider that supplies the requested capability.
        template<class TCapability, class... TProviders>
        struct FirstProvider;


        /// Represents an unsuccessful provider lookup.
        template<class TCapability>
        struct FirstProvider<TCapability> {

            // Lookup result.

            /// Type returned when no matching provider exists.
            using Type = void;

        };


        /// Continues a provider lookup until a provider supplying the requested capability is found.
        template<class TCapability, class TFirstProvider, class... TRestProviders>
        struct FirstProvider<TCapability, TFirstProvider, TRestProviders...> {

            // Lookup result.

            /// First provider supplying the requested capability, or the result of searching the remaining providers.
            using Type = std::conditional_t<
                ProviderProvidesV<TFirstProvider, TCapability>,
                TFirstProvider,
                typename FirstProvider<TCapability, TRestProviders...>::Type
            >;

        };


        /// Filters a provider pack to providers that supply the requested capability.
        template<class TCapability, class TAccumulatedProviders, class... TProviders>
        struct FilterProviders;


        /// Completes provider filtering when no providers remain to inspect.
        template<class TCapability, class... TAccumulatedProviders>
        struct FilterProviders<TCapability, ProviderList<TAccumulatedProviders...>> {

            // Filtering result.

            /// Provider list accumulated during filtering.
            using Type = ProviderList<TAccumulatedProviders...>;

        };


        /// Adds matching providers to the accumulated provider list and continues filtering.
        template<class TCapability, class... TAccumulatedProviders, class TFirstProvider, class... TRestProviders>
        struct FilterProviders<TCapability, ProviderList<TAccumulatedProviders...>, TFirstProvider, TRestProviders...> {

            // Filtering state.

            /// Provider list to use for the next filtering step.
            using NextProviders = std::conditional_t<
                ProviderProvidesV<TFirstProvider, TCapability>,
                ProviderList<TAccumulatedProviders..., TFirstProvider>,
                ProviderList<TAccumulatedProviders...>
            >;

            /// Final provider list returned after the remaining providers are inspected.
            using Type = typename FilterProviders<TCapability, NextProviders, TRestProviders...>::Type;

        };


        /// Determines whether one provider satisfies one capability requirement.
        template<class TProvider, class TNeed, bool TProvidesCapability = ProviderProvidesV<TProvider, typename TNeed::CapabilityType>>
        struct ProviderSatisfiesNeed : std::false_type {};


        /// Evaluates a capability requirement against a provider that supplies the required capability.
        template<class TProvider, class TNeed>
        struct ProviderSatisfiesNeed<TProvider, TNeed, true> : std::bool_constant<
            TNeed::template OfferSatisfied<
                typename TProvider::CompositionCapabilities::template OfferFor<typename TNeed::CapabilityType>
            >
        > {};


        /// Counts providers that satisfy one capability requirement.
        template<class TNeed, class... TProviders>
        inline constexpr std::size_t SatisfyingProviderCountV =
            (std::size_t{0U} + ... + (ProviderSatisfiesNeed<TProviders, TNeed>::value ? std::size_t{1U} : std::size_t{0U}));


        /// Selects the first provider satisfying one complete capability requirement.
        template<class TNeed, class... TProviders>
        struct FirstSatisfyingProvider;


        /// Represents an unsuccessful satisfying-provider lookup.
        template<class TNeed>
        struct FirstSatisfyingProvider<TNeed> {

            // Lookup result.

            /// Type returned when no provider satisfies the requested Need.
            using Type = void;

        };


        /// Continues a satisfying-provider lookup until one provider satisfies the requested Need.
        template<class TNeed, class TFirstProvider, class... TRestProviders>
        struct FirstSatisfyingProvider<TNeed, TFirstProvider, TRestProviders...> {

            // Lookup result.

            /// First provider satisfying the Need, or the result of searching the remaining providers.
            using Type = std::conditional_t<
                ProviderSatisfiesNeed<TFirstProvider, TNeed>::value,
                TFirstProvider,
                typename FirstSatisfyingProvider<TNeed, TRestProviders...>::Type
            >;

        };


        /// Filters a provider pack to providers satisfying one complete capability requirement.
        template<class TNeed, class TAccumulatedProviders, class... TProviders>
        struct FilterSatisfyingProviders;


        /// Completes satisfying-provider filtering when no providers remain to inspect.
        template<class TNeed, class... TAccumulatedProviders>
        struct FilterSatisfyingProviders<TNeed, ProviderList<TAccumulatedProviders...>> {

            // Filtering result.

            /// Provider list accumulated during filtering.
            using Type = ProviderList<TAccumulatedProviders...>;

        };


        /// Adds providers satisfying the Need to the accumulated list and continues filtering.
        template<class TNeed, class... TAccumulatedProviders, class TFirstProvider, class... TRestProviders>
        struct FilterSatisfyingProviders<TNeed, ProviderList<TAccumulatedProviders...>, TFirstProvider, TRestProviders...> {

            // Filtering state.

            /// Provider list to use for the next filtering step.
            using NextProviders = std::conditional_t<
                ProviderSatisfiesNeed<TFirstProvider, TNeed>::value,
                ProviderList<TAccumulatedProviders..., TFirstProvider>,
                ProviderList<TAccumulatedProviders...>
            >;

            /// Final provider list returned after the remaining providers are inspected.
            using Type = typename FilterSatisfyingProviders<TNeed, NextProviders, TRestProviders...>::Type;

        };


        /// Determines whether all requirements in a Requires declaration are satisfied by a provider pack.
        template<class TRequires, class... TProviders>
        struct RequirementsSatisfied;


        /// Evaluates every Need declaration contained in a Requires declaration.
        template<class... TNeeds, class... TProviders>
        struct RequirementsSatisfied<Requires<TNeeds...>, TProviders...> : std::bool_constant<
            ((SatisfyingProviderCountV<TNeeds, TProviders...> > 0U) && ...)
        > {};


        /// Determines whether all offers in a Provides declaration obey exclusive-capability rules.
        template<class TProvides, class TDomain, class... TProviders>
        struct ProvidesIsConflictFree;


        /// Checks every offered capability against the complete provider pack.
        template<class... TOffers, class TDomain, class... TProviders>
        struct ProvidesIsConflictFree<Provides<TOffers...>, TDomain, TProviders...> : std::bool_constant<
            ((
                !IsExclusiveCapabilityForV<TDomain, typename TOffers::CapabilityType> ||
                ProviderCountV<typename TOffers::CapabilityType, TProviders...> <= 1U
            ) && ...)
        > {};

    } // ESPressio::System::CompositionFramework::Detail


    /// Validates and exposes the compile-time architecture formed by a set of providers within one domain.
    template<class TDomain, class... TProviders>
    struct Composition {

        static_assert(
            IsDomainV<TDomain>,
            "Composition requires a concrete composition Domain"
        );

        static_assert(
            (Detail::IsProviderFor<TDomain, TProviders>::value && ...),
            "Composition contains a provider that is invalid or belongs to another domain"
        );

        // Internal composition validation.

        /// Indicates whether every exclusive capability has no more than one provider.
        static constexpr bool NoCapabilityConflicts =
            (Detail::ProvidesIsConflictFree<typename TProviders::CompositionCapabilities, TDomain, TProviders...>::value && ...);

        /// Indicates whether every same-domain provider requirement is satisfied by the complete provider set.
        static constexpr bool AllRequirementsSatisfied =
            (Detail::RequirementsSatisfied<typename TProviders::CompositionRequirements, TProviders...>::value && ...);

        static_assert(
            NoCapabilityConflicts,
            "Composition contains multiple providers for an ExclusiveCapability"
        );

        static_assert(
            AllRequirementsSatisfied,
            "Composition contains an unsatisfied same-domain provider requirement"
        );

        // Composition metadata.

        /// Marker used to identify Composition declarations during architecture-level inspection.
        using CompositionDeclarationTag = void;

        /// Domain represented by this composition.
        using CompositionDomain = TDomain;

        /// Complete compile-time list of providers contained by this Composition.
        using ProviderTypes = ProviderList<TProviders...>;

        /// Number of providers contained in this composition.
        static constexpr std::size_t ProviderCount = sizeof...(TProviders);

        /// Indicates that compile-time composition validation completed successfully.
        static constexpr bool IsValid = NoCapabilityConflicts && AllRequirementsSatisfied;

        // Capability queries.

        /// Returns the number of providers supplying the specified capability.
        template<class TCapability>
        static constexpr std::size_t ProviderCountFor = Detail::ProviderCountV<TCapability, TProviders...>;

        /// Indicates whether at least one provider supplies the specified capability.
        template<class TCapability>
        static constexpr bool Provides = ProviderCountFor<TCapability> > 0U;

        /// Returns every provider that supplies the specified capability.
        template<class TCapability>
        using ProvidersFor = typename Detail::FilterProviders<TCapability, ProviderList<>, TProviders...>::Type;

        /// Resolves the single provider supplying a capability.
        template<class TCapability>
        struct ResolveProvider {

            static_assert(
                IsCapabilityForV<TDomain, TCapability>,
                "Provider query capability does not belong to this Composition domain"
            );

            static_assert(
                ProviderCountFor<TCapability> == 1U,
                "ProviderFor requires exactly one provider for the requested capability"
            );

            // Resolution result.

            /// Provider type supplying the requested capability.
            using Type = typename Detail::FirstProvider<TCapability, TProviders...>::Type;

        };


        /// Returns the single provider supplying the specified capability.
        template<class TCapability>
        using ProviderFor = typename ResolveProvider<TCapability>::Type;

        // Qualified requirement queries.

        /// Validates one Need before qualified provider resolution is attempted.
        template<class TNeed>
        struct ValidateNeed {

            static_assert(
                Detail::NeedTraits<TNeed>::IsValid,
                "Qualified provider queries require a Need declaration"
            );

            static_assert(
                std::is_same_v<typename TNeed::CompositionDomain, TDomain>,
                "Qualified provider Need does not belong to this Composition domain"
            );

            // Validation result.

            /// Indicates that the Need is valid for this Composition domain.
            static constexpr bool IsValid = true;

        };


        /// Returns the number of providers satisfying one complete Need declaration.
        template<class TNeed>
        static constexpr std::size_t ProviderCountSatisfying =
            ValidateNeed<TNeed>::IsValid
                ? Detail::SatisfyingProviderCountV<TNeed, TProviders...>
                : 0U;

        /// Indicates whether at least one provider satisfies one complete Need declaration.
        template<class TNeed>
        static constexpr bool HasProviderSatisfying = ProviderCountSatisfying<TNeed> > 0U;

        /// Returns every provider satisfying one complete Need declaration.
        template<class TNeed>
        using ProvidersSatisfying = typename Detail::FilterSatisfyingProviders<
            TNeed,
            ProviderList<>,
            TProviders...
        >::Type;

        /// Resolves the single provider satisfying one complete Need declaration.
        template<class TNeed>
        struct ResolveProviderSatisfying {

            static_assert(
                ValidateNeed<TNeed>::IsValid,
                "ProviderSatisfying requires a Need belonging to this Composition domain"
            );

            static_assert(
                ProviderCountSatisfying<TNeed> == 1U,
                "ProviderSatisfying requires exactly one provider satisfying the requested Need"
            );

            // Resolution result.

            /// Provider type uniquely satisfying the requested Need.
            using Type = typename Detail::FirstSatisfyingProvider<TNeed, TProviders...>::Type;

        };


        /// Returns the single provider satisfying one complete Need declaration.
        template<class TNeed>
        using ProviderSatisfying = typename ResolveProviderSatisfying<TNeed>::Type;

        // Provider property queries.

        /// Resolves the property set advertised by one provider for one capability.
        template<class TProvider, class TCapability>
        struct ResolveProviderProperties {

            static_assert(
                Detail::ContainsTypeV<TProvider, TProviders...>,
                "Property query provider is not contained in this Composition"
            );

            static_assert(
                IsCapabilityForV<TDomain, TCapability>,
                "Property query capability does not belong to this Composition domain"
            );

            static_assert(
                TProvider::CompositionCapabilities::template Contains<TCapability>,
                "Property query provider does not supply the requested capability"
            );

            // Resolution result.

            /// Property set advertised by the provider for the requested capability.
            using Type = typename TProvider::CompositionCapabilities::template PropertiesFor<TCapability>;

        };


        /// Returns the property set advertised by a specific provider for a capability.
        template<class TProvider, class TCapability>
        using PropertiesForProvider = typename ResolveProviderProperties<TProvider, TCapability>::Type;

        /// Returns the property set advertised by the single provider for a capability.
        template<class TCapability>
        using PropertiesFor = PropertiesForProvider<ProviderFor<TCapability>, TCapability>;

        /// Indicates whether the single provider for a capability advertises the specified property.
        template<class TCapability, class TProperty>
        static constexpr bool HasProperty = PropertiesFor<TCapability>::template Contains<TProperty>;

        /// Returns the value advertised for a property by the single provider for a capability.
        template<class TCapability, class TProperty>
        static constexpr typename TProperty::ValueType PropertyValue = PropertiesFor<TCapability>::template Value<TProperty>;

        // Provider Attribute queries.

        /// Resolves the Attribute set advertised by one provider for one capability.
        template<class TProvider, class TCapability>
        struct ResolveProviderAttributes {

            static_assert(
                Detail::ContainsTypeV<TProvider, TProviders...>,
                "Attribute query provider is not contained in this Composition"
            );

            static_assert(
                IsCapabilityForV<TDomain, TCapability>,
                "Attribute query capability does not belong to this Composition domain"
            );

            static_assert(
                TProvider::CompositionCapabilities::template Contains<TCapability>,
                "Attribute query provider does not supply the requested capability"
            );

            // Resolution result.

            /// Attribute set advertised by the provider for the requested capability.
            using Type = typename TProvider::CompositionCapabilities::template AttributesFor<TCapability>;

        };


        /// Returns the Attribute set advertised by a specific provider for a capability.
        template<class TProvider, class TCapability>
        using AttributesForProvider = typename ResolveProviderAttributes<TProvider, TCapability>::Type;

        /// Returns the Attribute set advertised by the single provider for a capability.
        template<class TCapability>
        using AttributesFor = AttributesForProvider<ProviderFor<TCapability>, TCapability>;

    };

} // ESPressio::System::CompositionFramework
