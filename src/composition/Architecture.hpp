#pragma once

#include <cstddef>
#include <type_traits>

#include "Composition.hpp"

namespace ESPressio::System::CompositionFramework {

    namespace Detail {

        /// Default metadata for types that are not Composition declarations.
        template<class TComposition, class = void>
        struct CompositionTraits {

            // Composition metadata.

            /// Indicates whether the inspected type is a valid Composition declaration.
            static constexpr bool IsValid = false;

            /// Domain associated with the inspected type when one exists.
            using DomainType = void;

        };


        /// Extracts metadata from one Composition declaration.
        template<class TComposition>
        struct CompositionTraits<
            TComposition,
            std::void_t<
                typename TComposition::CompositionDeclarationTag,
                typename TComposition::CompositionDomain,
                typename TComposition::ProviderTypes
            >
        > {

            // Composition metadata.

            /// Indicates whether the inspected type represents a Composition over a concrete Domain.
            static constexpr bool IsValid = IsDomainV<typename TComposition::CompositionDomain>;

            /// Domain represented by the inspected Composition.
            using DomainType = typename TComposition::CompositionDomain;

        };


        /// Indicates whether every Composition domain in one pack is unique.
        template<class... TCompositions>
        struct UniqueCompositionDomains;


        /// Empty Composition packs contain no duplicate domains.
        template<>
        struct UniqueCompositionDomains<> : std::true_type {};


        /// Checks the first Composition domain against the remaining declarations and continues recursively.
        template<class TFirstComposition, class... TRestCompositions>
        struct UniqueCompositionDomains<TFirstComposition, TRestCompositions...> : std::bool_constant<
            ((!std::is_same_v<
                typename TFirstComposition::CompositionDomain,
                typename TRestCompositions::CompositionDomain
            >) && ...) &&
            UniqueCompositionDomains<TRestCompositions...>::value
        > {};


        /// Counts providers satisfying one Need within a Composition only when the Need belongs to that Composition domain.
        template<
            class TNeed,
            class TComposition,
            bool TDomainMatches = std::is_same_v<typename TNeed::CompositionDomain, typename TComposition::CompositionDomain>
        >
        struct SatisfyingProviderCountInComposition : std::integral_constant<std::size_t, 0U> {};


        /// Returns the satisfying-provider count when the Need belongs to the inspected Composition domain.
        template<class TNeed, class TComposition>
        struct SatisfyingProviderCountInComposition<TNeed, TComposition, true> : std::integral_constant<
            std::size_t,
            TComposition::template ProviderCountSatisfying<TNeed>
        > {};


        /// Counts providers satisfying one Need across every Composition participating in an Architecture.
        template<class TNeed, class... TCompositions>
        inline constexpr std::size_t ArchitectureSatisfyingProviderCountV =
            (std::size_t{0U} + ... + SatisfyingProviderCountInComposition<TNeed, TCompositions>::value);


        /// Determines whether every Need in one DependsOn declaration is satisfied across an Architecture.
        template<class TDependsOn, class... TCompositions>
        struct DependenciesSatisfied;


        /// Evaluates every cross-domain Need contained in one DependsOn declaration.
        template<class... TNeeds, class... TCompositions>
        struct DependenciesSatisfied<DependsOn<TNeeds...>, TCompositions...> : std::bool_constant<
            ((ArchitectureSatisfyingProviderCountV<TNeeds, TCompositions...> > 0U) && ...)
        > {};


        /// Determines whether every provider in one ProviderList has its cross-domain dependencies satisfied.
        template<class TProviderList, class... TCompositions>
        struct ProviderListDependenciesSatisfied;


        /// Evaluates each provider's cross-domain dependency declaration against the complete Architecture.
        template<class... TProviders, class... TCompositions>
        struct ProviderListDependenciesSatisfied<ProviderList<TProviders...>, TCompositions...> : std::bool_constant<
            (DependenciesSatisfied<typename TProviders::CompositionDependencies, TCompositions...>::value && ...)
        > {};


        /// Counts Compositions representing one requested Domain.
        template<class TDomain, class... TCompositions>
        inline constexpr std::size_t CompositionCountForDomainV =
            (std::size_t{0U} + ... + (
                std::is_same_v<TDomain, typename TCompositions::CompositionDomain>
                    ? std::size_t{1U}
                    : std::size_t{0U}
            ));


        /// Locates the Composition representing one requested Domain.
        template<class TDomain, class... TCompositions>
        struct FindComposition;


        /// Represents an unsuccessful Composition lookup.
        template<class TDomain>
        struct FindComposition<TDomain> {

            // Lookup result.

            /// Type returned when no Composition represents the requested Domain.
            using Type = void;

        };


        /// Continues a Composition lookup until the requested Domain is found.
        template<class TDomain, class TFirstComposition, class... TRestCompositions>
        struct FindComposition<TDomain, TFirstComposition, TRestCompositions...> {

            // Lookup result.

            /// Composition representing the requested Domain, or the result of searching remaining Compositions.
            using Type = std::conditional_t<
                std::is_same_v<TDomain, typename TFirstComposition::CompositionDomain>,
                TFirstComposition,
                typename FindComposition<TDomain, TRestCompositions...>::Type
            >;

        };

    } // ESPressio::System::CompositionFramework::Detail


    /// Validates cross-domain dependencies across a set of already-valid Domain Compositions.
    template<class... TCompositions>
    struct Architecture {

        static_assert(
            sizeof...(TCompositions) > 0U,
            "Architecture must contain at least one Composition"
        );

        static_assert(
            (Detail::CompositionTraits<TCompositions>::IsValid && ...),
            "Architecture entries must be valid Composition declarations"
        );

        static_assert(
            Detail::UniqueCompositionDomains<TCompositions...>::value,
            "Architecture contains more than one Composition for the same Domain"
        );

        // Architecture validation.

        /// Indicates whether every provider's cross-domain dependencies are satisfied by participating Compositions.
        static constexpr bool AllDependenciesSatisfied =
            (Detail::ProviderListDependenciesSatisfied<
                typename TCompositions::ProviderTypes,
                TCompositions...
            >::value && ...);

        static_assert(
            AllDependenciesSatisfied,
            "Architecture contains an unsatisfied cross-domain provider dependency"
        );

        // Architecture metadata.

        /// Marker used to identify Architecture declarations during compile-time inspection.
        using ArchitectureDeclarationTag = void;

        /// Number of Domain Compositions participating in this Architecture.
        static constexpr std::size_t CompositionCount = sizeof...(TCompositions);

        /// Indicates that all participating Compositions and cross-domain dependencies are valid.
        static constexpr bool IsValid = AllDependenciesSatisfied;

        // Domain queries.

        /// Resolves the unique Composition representing one requested Domain.
        template<class TDomain>
        struct ResolveComposition {

            static_assert(
                IsDomainV<TDomain>,
                "Architecture Composition query requires a concrete Domain"
            );

            static_assert(
                Detail::CompositionCountForDomainV<TDomain, TCompositions...> == 1U,
                "Architecture requires exactly one Composition for the requested Domain"
            );

            // Resolution result.

            /// Composition representing the requested Domain.
            using Type = typename Detail::FindComposition<TDomain, TCompositions...>::Type;

        };


        /// Returns the unique Composition representing one requested Domain.
        template<class TDomain>
        using CompositionFor = typename ResolveComposition<TDomain>::Type;

        // Cross-domain qualified provider queries.

        /// Returns the number of providers satisfying one Need across the complete Architecture.
        template<class TNeed>
        static constexpr std::size_t ProviderCountSatisfying =
            Detail::ArchitectureSatisfyingProviderCountV<TNeed, TCompositions...>;

        /// Indicates whether at least one provider satisfies one Need across the complete Architecture.
        template<class TNeed>
        static constexpr bool HasProviderSatisfying = ProviderCountSatisfying<TNeed> > 0U;

        /// Returns every provider satisfying one Need from the Composition owning that Need's Domain.
        template<class TNeed>
        using ProvidersSatisfying = typename CompositionFor<typename TNeed::CompositionDomain>::template ProvidersSatisfying<TNeed>;

        /// Returns the unique provider satisfying one Need from the Composition owning that Need's Domain.
        template<class TNeed>
        using ProviderSatisfying = typename CompositionFor<typename TNeed::CompositionDomain>::template ProviderSatisfying<TNeed>;

    };

} // ESPressio::System::CompositionFramework
