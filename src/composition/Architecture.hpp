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


        /// Evaluates every cross-domain Requirement contained in one DependsOn declaration.
        ///
        /// Consolidated Requirement cardinality is enforced while temporary legacy Needs retain
        /// their historical at-least-one-provider semantics during downstream migration.
        ///
        /// @tparam TRequirements Cross-domain Requirements being evaluated.
        /// @tparam TCompositions Domain Compositions participating in the Architecture.
        template<class... TRequirements, class... TCompositions>
        struct DependenciesSatisfied<
            DependsOn<TRequirements...>,
            TCompositions...
        > : std::bool_constant<
            (
                RequirementCardinalitySatisfied<
                    TRequirements,
                    ArchitectureSatisfyingProviderCountV<
                        TRequirements,
                        TCompositions...
                    >
                >::value &&
                ...
            )
        > {};


        /// Determines whether every provider in one ProviderList has its cross-domain dependencies satisfied.
        template<class TProviderList, class... TCompositions>
        struct ProviderListDependenciesSatisfied;


        /// Evaluates each provider's cross-domain dependency declaration against the complete Architecture.
        template<class... TProviders, class... TCompositions>
        struct ProviderListDependenciesSatisfied<ProviderList<TProviders...>, TCompositions...> : std::bool_constant<
            (DependenciesSatisfied<typename TProviders::CompositionDependencies, TCompositions...>::value && ...)
        > {};


        /// Concatenates every provider list represented by one Architecture.
        ///
        /// @tparam TCompositions Domain Compositions whose providers are being flattened.
        template<class... TCompositions>
        struct ArchitectureProviderTypes;


        /// Empty Architecture provider population.
        template<>
        struct ArchitectureProviderTypes<> {

            /// Empty provider list.
            using Type = ProviderList<>;

        };


        /// Concatenates one Composition's providers with the remaining Architecture providers.
        ///
        /// @tparam TFirstComposition First Composition in declaration order.
        /// @tparam TRestCompositions Remaining Compositions.
        template<class TFirstComposition, class... TRestCompositions>
        struct ArchitectureProviderTypes<
            TFirstComposition,
            TRestCompositions...
        > {

            private:

                /// Flattened provider list from the remaining Compositions.
                using Remaining = typename ArchitectureProviderTypes<
                    TRestCompositions...
                >::Type;


            public:

                /// Complete provider list preserving Composition and provider declaration order.
                using Type = ProviderListConcat<
                    typename TFirstComposition::ProviderTypes,
                    Remaining
                >;

        };


        /// Evaluates one consolidated Contract clause across a complete Architecture provider population.
        ///
        /// @tparam TClause Contract clause being evaluated.
        /// @tparam TProviderList Complete Architecture provider population.
        template<class TClause, class TProviderList>
        struct ContractClauseSatisfiedInArchitecture : std::true_type {};


        /// Evaluates one direct consolidated Requirement across an Architecture.
        ///
        /// @tparam TProviderList Complete Architecture provider population.
        /// @tparam TCapability Requested Capability.
        /// @tparam TScope Requirement scope.
        /// @tparam TCardinality Accepted satisfying-provider count.
        /// @tparam TConstraints Requirement qualification constraints.
        template<
            class TProviderList,
            class TCapability,
            RequirementScope TScope,
            class TCardinality,
            class... TConstraints
        >
        struct ContractClauseSatisfiedInArchitecture<
            Requirement<
                TCapability,
                TScope,
                TCardinality,
                TConstraints...
            >,
            TProviderList
        > : RequirementCardinalitySatisfied<
            Requirement<
                TCapability,
                TScope,
                TCardinality,
                TConstraints...
            >,
            SatisfyingProviderCountInList<
                Requirement<
                    TCapability,
                    TScope,
                    TCardinality,
                    TConstraints...
                >,
                TProviderList
            >::value
        > {};


        /// Evaluates one SameProvider relationship across an Architecture.
        ///
        /// @tparam TProviderList Complete Architecture provider population.
        /// @tparam TRequirements Requirements requiring one joint provider.
        template<class TProviderList, class... TRequirements>
        struct ContractClauseSatisfiedInArchitecture<
            SameProvider<TRequirements...>,
            TProviderList
        > : std::bool_constant<
            JointlySatisfyingProviderCount<
                RequirementList<TRequirements...>,
                TProviderList
            >::value > 0U
        > {};


        /// Evaluates one DistinctProviders relationship across an Architecture.
        ///
        /// @tparam TProviderList Complete Architecture provider population.
        /// @tparam TRequirements Requirements requiring distinct providers.
        template<class TProviderList, class... TRequirements>
        struct ContractClauseSatisfiedInArchitecture<
            DistinctProviders<TRequirements...>,
            TProviderList
        > : DistinctAssignmentExists<
            RequirementList<TRequirements...>,
            TProviderList
        > {};


        /// Requires one unambiguous initialization predecessor across the Architecture.
        ///
        /// @tparam TProviderList Complete Architecture provider population.
        /// @tparam TRequirement Lifecycle predecessor Requirement.
        template<class TProviderList, class TRequirement>
        struct ContractClauseSatisfiedInArchitecture<
            InitializesAfter<TRequirement>,
            TProviderList
        > : std::bool_constant<
            SatisfyingProviderCountInList<
                TRequirement,
                TProviderList
            >::value == 1U
        > {};


        /// Requires one unambiguous shutdown successor across the Architecture.
        ///
        /// @tparam TProviderList Complete Architecture provider population.
        /// @tparam TRequirement Lifecycle successor Requirement.
        template<class TProviderList, class TRequirement>
        struct ContractClauseSatisfiedInArchitecture<
            ShutsDownBefore<TRequirement>,
            TProviderList
        > : std::bool_constant<
            SatisfyingProviderCountInList<
                TRequirement,
                TProviderList
            >::value == 1U
        > {};


        /// Evaluates one consolidated Contract across an Architecture.
        ///
        /// @tparam TContract Contract or void while one provider remains on the temporary migration path.
        /// @tparam TProviderList Complete Architecture provider population.
        template<class TContract, class TProviderList>
        struct ContractSatisfiedInArchitecture : std::true_type {};


        /// Evaluates every clause in one concrete Contract.
        ///
        /// @tparam TProviderList Complete Architecture provider population.
        /// @tparam TClauses Contract clauses being evaluated.
        template<class TProviderList, class... TClauses>
        struct ContractSatisfiedInArchitecture<
            Contract<TClauses...>,
            TProviderList
        > : std::bool_constant<
            (
                ContractClauseSatisfiedInArchitecture<
                    TClauses,
                    TProviderList
                >::value &&
                ...
            )
        > {};


        /// Evaluates every provider Contract represented by one ProviderList.
        ///
        /// @tparam TProviderList Providers whose Contracts are being validated.
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        template<class TProviderList, class TArchitectureProviders>
        struct ProviderListContractsSatisfied;


        /// Evaluates every concrete provider Contract.
        ///
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TProviders Providers whose Contracts are being validated.
        template<class TArchitectureProviders, class... TProviders>
        struct ProviderListContractsSatisfied<
            ProviderList<TProviders...>,
            TArchitectureProviders
        > : std::bool_constant<
            (
                ContractSatisfiedInArchitecture<
                    typename TProviders::CompositionContract,
                    TArchitectureProviders
                >::value &&
                ...
            )
        > {};


        /// Indicates whether one lifecycle clause creates an ordering edge between two providers.
        ///
        /// @tparam TClause Contract clause being inspected.
        /// @tparam TBefore Provider Type proposed before the other provider.
        /// @tparam TAfter Provider Type proposed after the other provider.
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TInitializationOrder Whether initialization ordering is being derived.
        template<
            class TClause,
            class TBefore,
            class TAfter,
            class TArchitectureProviders,
            bool TInitializationOrder
        >
        struct LifecycleClauseCreatesEdge : std::false_type {};


        /// Creates an initialization edge from the uniquely selected predecessor to the owning provider.
        ///
        /// @tparam TBefore Candidate predecessor provider Type.
        /// @tparam TAfter Provider Type owning the InitializesAfter clause.
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TRequirement Requirement selecting the predecessor.
        template<
            class TBefore,
            class TAfter,
            class TArchitectureProviders,
            class TRequirement
        >
        struct LifecycleClauseCreatesEdge<
            InitializesAfter<TRequirement>,
            TBefore,
            TAfter,
            TArchitectureProviders,
            true
        > : std::bool_constant<
            ProviderSatisfiesNeed<
                TBefore,
                TRequirement
            >::value
        > {};


        /// Creates a shutdown edge from the owning provider to the uniquely selected successor.
        ///
        /// @tparam TBefore Provider Type owning the ShutsDownBefore clause.
        /// @tparam TAfter Candidate shutdown successor provider Type.
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TRequirement Requirement selecting the successor.
        template<
            class TBefore,
            class TAfter,
            class TArchitectureProviders,
            class TRequirement
        >
        struct LifecycleClauseCreatesEdge<
            ShutsDownBefore<TRequirement>,
            TBefore,
            TAfter,
            TArchitectureProviders,
            false
        > : std::bool_constant<
            ProviderSatisfiesNeed<
                TAfter,
                TRequirement
            >::value
        > {};


        /// Indicates whether one Contract establishes an ordering edge between two providers.
        ///
        /// @tparam TContract Contract being inspected.
        /// @tparam TBefore Provider Type proposed before the other provider.
        /// @tparam TAfter Provider Type proposed after the other provider.
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TInitializationOrder Whether initialization ordering is being derived.
        template<
            class TContract,
            class TBefore,
            class TAfter,
            class TArchitectureProviders,
            bool TInitializationOrder
        >
        struct ContractCreatesLifecycleEdge : std::false_type {};


        /// Evaluates every lifecycle clause in one consolidated Contract.
        ///
        /// @tparam TBefore Provider Type proposed before the other provider.
        /// @tparam TAfter Provider Type proposed after the other provider.
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TInitializationOrder Whether initialization ordering is being derived.
        /// @tparam TClauses Contract clauses being inspected.
        template<
            class TBefore,
            class TAfter,
            class TArchitectureProviders,
            bool TInitializationOrder,
            class... TClauses
        >
        struct ContractCreatesLifecycleEdge<
            Contract<TClauses...>,
            TBefore,
            TAfter,
            TArchitectureProviders,
            TInitializationOrder
        > : std::bool_constant<
            (
                LifecycleClauseCreatesEdge<
                    TClauses,
                    TBefore,
                    TAfter,
                    TArchitectureProviders,
                    TInitializationOrder
                >::value ||
                ...
            )
        > {};


        /// Indicates whether one directed lifecycle edge exists between two providers.
        ///
        /// Initialization edges are declared by the after-provider's Contract.
        /// Shutdown edges are declared by the before-provider's Contract.
        ///
        /// @tparam TBefore Provider Type proposed before the other provider.
        /// @tparam TAfter Provider Type proposed after the other provider.
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TInitializationOrder Whether initialization ordering is being derived.
        template<
            class TBefore,
            class TAfter,
            class TArchitectureProviders,
            bool TInitializationOrder
        >
        inline constexpr bool LifecycleEdgeV = []() consteval {
            if constexpr (std::is_same_v<TBefore, TAfter>) {
                return false;
            } else if constexpr (TInitializationOrder) {
                return ContractCreatesLifecycleEdge<
                    typename TAfter::CompositionContract,
                    TBefore,
                    TAfter,
                    TArchitectureProviders,
                    true
                >::value;
            } else {
                return ContractCreatesLifecycleEdge<
                    typename TBefore::CompositionContract,
                    TBefore,
                    TAfter,
                    TArchitectureProviders,
                    false
                >::value;
            }
        }();


        /// Indicates whether one provider has any incoming lifecycle edge from the remaining provider set.
        ///
        /// @tparam TCandidate Provider Type being tested for readiness.
        /// @tparam TRemainingProviders Remaining providers in the topological sort.
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TInitializationOrder Whether initialization ordering is being derived.
        template<
            class TCandidate,
            class TRemainingProviders,
            class TArchitectureProviders,
            bool TInitializationOrder
        >
        struct HasIncomingLifecycleEdge;


        /// Evaluates incoming edges from one concrete remaining provider pack.
        ///
        /// @tparam TCandidate Provider Type being tested for readiness.
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TInitializationOrder Whether initialization ordering is being derived.
        /// @tparam TProviders Remaining provider Types.
        template<
            class TCandidate,
            class TArchitectureProviders,
            bool TInitializationOrder,
            class... TProviders
        >
        struct HasIncomingLifecycleEdge<
            TCandidate,
            ProviderList<TProviders...>,
            TArchitectureProviders,
            TInitializationOrder
        > : std::bool_constant<
            (
                LifecycleEdgeV<
                    TProviders,
                    TCandidate,
                    TArchitectureProviders,
                    TInitializationOrder
                > ||
                ...
            )
        > {};


        /// Finds the first declaration-order provider with no incoming lifecycle edge.
        ///
        /// @tparam TRemainingProviders Remaining providers in the topological sort.
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TInitializationOrder Whether initialization ordering is being derived.
        template<
            class TRemainingProviders,
            class TArchitectureProviders,
            bool TInitializationOrder
        >
        struct FindLifecycleReadyProvider;


        /// No provider can be selected from an empty set.
        ///
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TInitializationOrder Whether initialization ordering is being derived.
        template<
            class TArchitectureProviders,
            bool TInitializationOrder
        >
        struct FindLifecycleReadyProvider<
            ProviderList<>,
            TArchitectureProviders,
            TInitializationOrder
        > {

            /// Empty-selection sentinel.
            using Type = void;

        };


        /// Selects the first provider having no incoming edge, preserving declaration order for unrelated providers.
        ///
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TInitializationOrder Whether initialization ordering is being derived.
        /// @tparam TFirstProvider Current candidate provider Type.
        /// @tparam TRestProviders Remaining provider Types.
        template<
            class TArchitectureProviders,
            bool TInitializationOrder,
            class TFirstProvider,
            class... TRestProviders
        >
        struct FindLifecycleReadyProvider<
            ProviderList<
                TFirstProvider,
                TRestProviders...
            >,
            TArchitectureProviders,
            TInitializationOrder
        > {

            /// First ready provider Type, or the result of inspecting the remaining providers.
            using Type = std::conditional_t<
                !HasIncomingLifecycleEdge<
                    TFirstProvider,
                    ProviderList<
                        TFirstProvider,
                        TRestProviders...
                    >,
                    TArchitectureProviders,
                    TInitializationOrder
                >::value,
                TFirstProvider,
                typename FindLifecycleReadyProvider<
                    ProviderList<TRestProviders...>,
                    TArchitectureProviders,
                    TInitializationOrder
                >::Type
            >;

        };


        /// Builds one deterministic lifecycle provider ordering.
        ///
        /// @tparam TOrderedProviders Providers already placed in lifecycle order.
        /// @tparam TRemainingProviders Providers still requiring placement.
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TInitializationOrder Whether initialization ordering is being derived.
        template<
            class TOrderedProviders,
            class TRemainingProviders,
            class TArchitectureProviders,
            bool TInitializationOrder
        >
        struct BuildLifecycleOrder;


        /// Completes lifecycle ordering when no providers remain.
        ///
        /// @tparam TOrderedProviders Complete provider order.
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TInitializationOrder Whether initialization ordering is being derived.
        template<
            class TOrderedProviders,
            class TArchitectureProviders,
            bool TInitializationOrder
        >
        struct BuildLifecycleOrder<
            TOrderedProviders,
            ProviderList<>,
            TArchitectureProviders,
            TInitializationOrder
        > {

            /// Completed lifecycle order.
            using Type = TOrderedProviders;

        };


        /// Continues lifecycle ordering after selecting one ready provider.
        ///
        /// @tparam TOrderedProviders Providers already placed in lifecycle order.
        /// @tparam TRemainingProviders Providers still requiring placement.
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TInitializationOrder Whether initialization ordering is being derived.
        /// @tparam TReadyProvider Selected ready provider Type.
        template<
            class TOrderedProviders,
            class TRemainingProviders,
            class TArchitectureProviders,
            bool TInitializationOrder,
            class TReadyProvider
        >
        struct ContinueLifecycleOrder {

            /// Remaining provider set after consuming the ready provider.
            using NextRemaining = typename RemoveProviderFromList<
                TReadyProvider,
                TRemainingProviders
            >::Type;

            /// Ordered provider set after appending the ready provider.
            using NextOrdered = typename AppendProviderList<
                TOrderedProviders,
                TReadyProvider
            >::Type;

            /// Completed deterministic lifecycle order.
            using Type = typename BuildLifecycleOrder<
                NextOrdered,
                NextRemaining,
                TArchitectureProviders,
                TInitializationOrder
            >::Type;

        };


        /// Rejects a lifecycle dependency cycle when no remaining provider is ready.
        ///
        /// @tparam TOrderedProviders Providers placed before the cycle was detected.
        /// @tparam TRemainingProviders Providers participating in the unresolved cycle.
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TInitializationOrder Whether initialization ordering is being derived.
        template<
            class TOrderedProviders,
            class TRemainingProviders,
            class TArchitectureProviders,
            bool TInitializationOrder
        >
        struct ContinueLifecycleOrder<
            TOrderedProviders,
            TRemainingProviders,
            TArchitectureProviders,
            TInitializationOrder,
            void
        > {

            static_assert(
                TRemainingProviders::IsEmpty,
                "Composition lifecycle ordering contains a dependency cycle"
            );

            /// Unreachable fallback Type retained only to terminate template substitution after the diagnostic.
            using Type = TOrderedProviders;

        };


        /// Selects one ready provider and continues deterministic lifecycle ordering.
        ///
        /// @tparam TOrderedProviders Providers already placed in lifecycle order.
        /// @tparam TFirstProvider First remaining provider Type.
        /// @tparam TRestProviders Remaining provider Types.
        /// @tparam TArchitectureProviders Complete Architecture provider population.
        /// @tparam TInitializationOrder Whether initialization ordering is being derived.
        template<
            class TOrderedProviders,
            class TFirstProvider,
            class... TRestProviders,
            class TArchitectureProviders,
            bool TInitializationOrder
        >
        struct BuildLifecycleOrder<
            TOrderedProviders,
            ProviderList<
                TFirstProvider,
                TRestProviders...
            >,
            TArchitectureProviders,
            TInitializationOrder
        > {

            private:

                /// Complete remaining provider list.
                using RemainingProviders = ProviderList<
                    TFirstProvider,
                    TRestProviders...
                >;

                /// First provider ready under the requested lifecycle relation.
                using ReadyProvider = typename FindLifecycleReadyProvider<
                    RemainingProviders,
                    TArchitectureProviders,
                    TInitializationOrder
                >::Type;


            public:

                /// Completed deterministic lifecycle order.
                using Type = typename ContinueLifecycleOrder<
                    TOrderedProviders,
                    RemainingProviders,
                    TArchitectureProviders,
                    TInitializationOrder,
                    ReadyProvider
                >::Type;

        };


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

        /// Complete deterministic provider population across all participating Compositions.
        using ProviderTypes = typename Detail::ArchitectureProviderTypes<
            TCompositions...
        >::Type;

        /// Deterministic provider initialization order derived only from explicit InitializesAfter clauses.
        using InitializationOrder = typename Detail::BuildLifecycleOrder<
            ProviderList<>,
            ProviderTypes,
            ProviderTypes,
            true
        >::Type;

        /// Deterministic provider shutdown order derived only from explicit ShutsDownBefore clauses.
        using ShutdownOrder = typename Detail::BuildLifecycleOrder<
            ProviderList<>,
            ProviderTypes,
            ProviderTypes,
            false
        >::Type;

        /// Indicates whether every provider's cross-domain dependencies are satisfied by participating Compositions.
        static constexpr bool AllDependenciesSatisfied =
            (Detail::ProviderListDependenciesSatisfied<
                typename TCompositions::ProviderTypes,
                TCompositions...
            >::value && ...);

        /// Indicates whether every consolidated provider Contract is satisfied across the complete Architecture.
        static constexpr bool AllContractsSatisfied =
            (
                Detail::ProviderListContractsSatisfied<
                    typename TCompositions::ProviderTypes,
                    ProviderTypes
                >::value &&
                ...
            );

        static_assert(
            AllDependenciesSatisfied,
            "Architecture contains an unsatisfied cross-domain provider dependency"
        );

        static_assert(
            AllContractsSatisfied,
            "Architecture contains an unsatisfied consolidated provider Contract"
        );

        // Architecture metadata.

        /// Marker used to identify Architecture declarations during compile-time inspection.
        using ArchitectureDeclarationTag = void;

        /// Number of Domain Compositions participating in this Architecture.
        static constexpr std::size_t CompositionCount = sizeof...(TCompositions);

        /// Indicates that all participating Compositions and cross-domain dependencies are valid.
        static constexpr bool IsValid =
            AllDependenciesSatisfied &&
            AllContractsSatisfied;

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


        // Consolidated Requirement queries.

        /// Returns the number of providers matching one Requirement qualification.
        ///
        /// @tparam TRequirement Requirement being queried.
        template<class TRequirement>
        static constexpr std::size_t MatchCount =
            ProviderCountSatisfying<TRequirement>;

        /// Indicates whether at least one provider matches one Requirement qualification.
        ///
        /// @tparam TRequirement Requirement being queried.
        template<class TRequirement>
        static constexpr bool HasMatch =
            MatchCount<TRequirement> > 0U;

        /// Returns every provider matching one Requirement qualification.
        ///
        /// @tparam TRequirement Requirement being queried.
        template<class TRequirement>
        using Matches = ProvidersSatisfying<TRequirement>;

        /// Indicates whether the Architecture provider population obeys one Requirement's cardinality.
        ///
        /// @tparam TRequirement Requirement being validated.
        template<class TRequirement>
        static constexpr bool SatisfiesRequirement =
            Detail::RequirementCardinalitySatisfied<
                TRequirement,
                MatchCount<TRequirement>
            >::value;

        /// Resolves Requirement matches using one explicit selection policy.
        ///
        /// @tparam TRequirement Requirement whose matching providers are being selected.
        /// @tparam TSelectionPolicy Explicit provider selection policy.
        template<
            class TRequirement,
            class TSelectionPolicy = SelectUnique
        >
        using Select = typename Detail::SelectionResult<
            TRequirement,
            TSelectionPolicy,
            Matches<TRequirement>
        >::Type;


        // Consumer Contract validation.

        /// Indicates whether the complete Architecture satisfies one reusable consumer Contract.
        ///
        /// @tparam TContract Consolidated consumer Contract.
        template<class TContract>
        static constexpr bool SatisfiesContract =
            Detail::ContractTraits<TContract>::IsValid &&
            Detail::ContractSatisfiedInArchitecture<
                TContract,
                ProviderTypes
            >::value;

        /// Performs strict compile-time validation of one consumer Contract.
        ///
        /// @tparam TContract Consolidated consumer Contract.
        template<class TContract>
        struct ValidateContract {

            static_assert(
                Detail::ContractTraits<TContract>::IsValid,
                "Architecture::ValidateContract requires a consolidated Contract"
            );

            static_assert(
                SatisfiesContract<TContract>,
                "Architecture does not satisfy the requested consumer Contract"
            );

            /// Indicates successful consumer Contract validation.
            static constexpr bool IsValid = true;

        };

    };

} // ESPressio::System::CompositionFramework
