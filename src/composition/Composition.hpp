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
                typename TProvider::CompositionRequirements
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
            TNeed::template PropertiesSatisfied<
                typename TProvider::CompositionCapabilities::template PropertiesFor<typename TNeed::CapabilityType>
            >
        > {};


        /// Counts providers that satisfy one capability requirement.
        template<class TNeed, class... TProviders>
        inline constexpr std::size_t SatisfyingProviderCountV =
            (std::size_t{0U} + ... + (ProviderSatisfiesNeed<TProviders, TNeed>::value ? std::size_t{1U} : std::size_t{0U}));


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

        /// Indicates whether every provider requirement is satisfied by the complete provider set.
        static constexpr bool AllRequirementsSatisfied =
            (Detail::RequirementsSatisfied<typename TProviders::CompositionRequirements, TProviders...>::value && ...);

        static_assert(
            NoCapabilityConflicts,
            "Composition contains multiple providers for an ExclusiveCapability"
        );

        static_assert(
            AllRequirementsSatisfied,
            "Composition contains an unsatisfied provider requirement"
        );

        // Composition metadata.

        /// Domain represented by this composition.
        using CompositionDomain = TDomain;

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

    };

} // ESPressio::System::CompositionFramework
