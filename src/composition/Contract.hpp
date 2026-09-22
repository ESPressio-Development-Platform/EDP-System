#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

#include "Requirement.hpp"

namespace ESPressio::System::CompositionFramework {

    /// Requirement scope restricting where a provider may satisfy one compile-time Requirement.
    enum class RequirementScope : std::uint8_t {
        SameDomain = 0,
        ExternalDomain = 1,
        AnyDomain = 2
    };


    /// Compile-time provider-count interval attached to one Requirement.
    ///
    /// @tparam TMinimum Minimum number of satisfying providers.
    /// @tparam TMaximum Maximum number of satisfying providers.
    template<std::size_t TMinimum, std::size_t TMaximum>
    struct ProviderCardinality {

        static_assert(
            TMinimum <= TMaximum,
            "ProviderCardinality minimum must not exceed its maximum"
        );

        // Cardinality metadata.

        /// Minimum accepted satisfying-provider count.
        static constexpr std::size_t Minimum = TMinimum;

        /// Maximum accepted satisfying-provider count.
        static constexpr std::size_t Maximum = TMaximum;


        // Cardinality evaluation.

        /// Indicates whether a satisfying-provider count falls inside this cardinality interval.
        ///
        /// @param count Number of providers satisfying the associated Requirement.
        static constexpr bool IsSatisfied(
            std::size_t count
        ) noexcept {
            return count >= Minimum && count <= Maximum;
        }

    };


    /// Requires exactly the specified number of satisfying providers.
    ///
    /// @tparam TCount Required provider count.
    template<std::size_t TCount>
    using ExactlyProviders = ProviderCardinality<TCount, TCount>;

    /// Requires at least the specified number of satisfying providers.
    ///
    /// @tparam TCount Minimum accepted provider count.
    template<std::size_t TCount>
    using AtLeastProviders = ProviderCardinality<
        TCount,
        std::numeric_limits<std::size_t>::max()
    >;

    /// Requires no more than the specified number of satisfying providers.
    ///
    /// @tparam TCount Maximum accepted provider count.
    template<std::size_t TCount>
    using AtMostProviders = ProviderCardinality<0U, TCount>;

    /// Requires a satisfying-provider count inside the supplied inclusive interval.
    ///
    /// @tparam TMinimum Minimum accepted provider count.
    /// @tparam TMaximum Maximum accepted provider count.
    template<std::size_t TMinimum, std::size_t TMaximum>
    using ProviderCountBetween = ProviderCardinality<TMinimum, TMaximum>;

    /// Accepts any satisfying-provider count, including zero.
    using AnyProviderCount = ProviderCardinality<
        0U,
        std::numeric_limits<std::size_t>::max()
    >;


    /// Declares one capability requirement together with its scope, cardinality and qualification constraints.
    ///
    /// A Requirement describes qualification and architectural necessity. It does not choose a provider
    /// when several providers satisfy the qualification; selection remains an explicit query concern.
    ///
    /// @tparam TCapability Capability required by this declaration.
    /// @tparam TScope Scope in which a satisfying provider may be found.
    /// @tparam TCardinality Accepted count of providers satisfying the complete requirement.
    /// @tparam TConstraints Compile-time Property, FlagProperty or Attribute constraints.
    template<
        class TCapability,
        RequirementScope TScope = RequirementScope::AnyDomain,
        class TCardinality = AtLeastProviders<1U>,
        class... TConstraints
    >
    struct Requirement {

        static_assert(
            IsCapabilityV<TCapability>,
            "Requirement requires a concrete composition Capability"
        );

        static_assert(
            TCardinality::Minimum <= TCardinality::Maximum,
            "Requirement requires a valid ProviderCardinality"
        );

        static_assert(
            (Detail::RequirementConstraintAppliesToCapability<TCapability, TConstraints>() && ...),
            "Requirement contains a constraint that cannot be applied to the requested capability"
        );

        // Compatibility metadata used by the current matching core during migration.

        /// Marker allowing the existing qualified-matching machinery to inspect Requirement declarations.
        using NeedTag = void;


        // Requirement metadata.

        /// Marker identifying the consolidated Requirement declaration.
        using RequirementTag = void;

        /// Capability required by this declaration.
        using CapabilityType = TCapability;

        /// Domain inherited from the requested capability.
        using CompositionDomain = typename Detail::CapabilityTraits<TCapability>::DomainType;

        /// Cardinality policy attached to this Requirement.
        using Cardinality = TCardinality;

        /// Scope in which satisfying providers are permitted.
        static constexpr RequirementScope Scope = TScope;

        /// Number of qualification constraints attached to this Requirement.
        static constexpr std::size_t ConstraintCount = sizeof...(TConstraints);


        // Requirement evaluation.

        /// Indicates whether a satisfying-provider count obeys this Requirement's cardinality.
        ///
        /// @param count Number of providers satisfying the Requirement qualification.
        static constexpr bool AcceptsProviderCount(
            std::size_t count
        ) noexcept {
            return Cardinality::IsSatisfied(
                count
            );
        }

        /// Indicates whether a supplied PropertySet satisfies every property-only constraint.
        ///
        /// Attribute constraints cannot be satisfied from a PropertySet-only view.
        ///
        /// @tparam TPropertySet PropertySet being inspected.
        template<class TPropertySet>
        static constexpr bool PropertiesSatisfied =
            (Detail::ConstraintSatisfiedByProperties<TConstraints, TPropertySet>::value && ...);

        /// Indicates whether one complete Offer satisfies every qualification constraint.
        ///
        /// @tparam TOffer Offer being inspected.
        template<class TOffer>
        static constexpr bool OfferSatisfied = (
            Detail::RequirementConstraintSatisfiedByOffer<
                TConstraints,
                TOffer
            >() &&
            ...
        );

    };


    namespace Detail {

        /// Default consolidated Requirement metadata for unrelated Types.
        ///
        /// @tparam TRequirement Type being inspected.
        /// @tparam TEnable SFINAE helper used when Requirement metadata is present.
        template<class TRequirement, class TEnable = void>
        struct RequirementTraits {

            // Requirement metadata.

            /// Indicates whether the inspected Type is a consolidated Requirement.
            static constexpr bool IsValid = false;

        };


        /// Extracts consolidated Requirement metadata.
        ///
        /// @tparam TRequirement Requirement Type being inspected.
        template<class TRequirement>
        struct RequirementTraits<
            TRequirement,
            std::void_t<
                typename TRequirement::RequirementTag,
                typename TRequirement::CapabilityType,
                typename TRequirement::CompositionDomain,
                typename TRequirement::Cardinality
            >
        > {

            // Requirement metadata.

            /// Indicates whether the inspected Type references a concrete Capability.
            static constexpr bool IsValid = IsCapabilityV<typename TRequirement::CapabilityType>;

        };


        /// Stores a compile-time list of consolidated Requirement Types.
        ///
        /// @tparam TRequirements Requirement Types represented by this internal list.
        template<class... TRequirements>
        struct RequirementList {};


        /// Stores a compile-time list of Contract clause Types.
        ///
        /// @tparam TClauses Contract clauses represented by this internal list.
        template<class... TClauses>
        struct ContractClauseList {};


        /// Default SameProvider metadata for unrelated Types.
        ///
        /// @tparam TClause Type being inspected.
        /// @tparam TEnable SFINAE helper used when SameProvider metadata is present.
        template<class TClause, class TEnable = void>
        struct SameProviderTraits {

            // Clause metadata.

            /// Indicates whether the inspected Type is a SameProvider clause.
            static constexpr bool IsValid = false;

        };


        /// Default DistinctProviders metadata for unrelated Types.
        ///
        /// @tparam TClause Type being inspected.
        /// @tparam TEnable SFINAE helper used when DistinctProviders metadata is present.
        template<class TClause, class TEnable = void>
        struct DistinctProvidersTraits {

            // Clause metadata.

            /// Indicates whether the inspected Type is a DistinctProviders clause.
            static constexpr bool IsValid = false;

        };


        /// Default lifecycle-clause metadata for unrelated Types.
        ///
        /// @tparam TClause Type being inspected.
        /// @tparam TEnable SFINAE helper used when lifecycle metadata is present.
        template<class TClause, class TEnable = void>
        struct LifecycleClauseTraits {

            // Clause metadata.

            /// Indicates whether the inspected Type is a lifecycle-ordering clause.
            static constexpr bool IsValid = false;

        };

    } // ESPressio::System::CompositionFramework::Detail


    /// Requires one concrete provider Type to satisfy every contained Requirement.
    ///
    /// @tparam TRequirements Requirements which must be jointly satisfied by one provider Type.
    template<class... TRequirements>
    struct SameProvider {

        static_assert(
            sizeof...(TRequirements) > 1U,
            "SameProvider requires at least two Requirements"
        );

        static_assert(
            (Detail::RequirementTraits<TRequirements>::IsValid && ...),
            "SameProvider entries must be consolidated Requirement declarations"
        );

        // Clause metadata.

        /// Marker identifying a SameProvider clause.
        using SameProviderTag = void;

        /// Number of Requirements joined by this clause.
        static constexpr std::size_t Count = sizeof...(TRequirements);

        /// Requirements represented by this clause.
        using Requirements = Detail::RequirementList<TRequirements...>;

    };


    namespace Detail {

        /// Extracts SameProvider metadata.
        ///
        /// @tparam TClause SameProvider clause being inspected.
        template<class TClause>
        struct SameProviderTraits<
            TClause,
            std::void_t<
                typename TClause::SameProviderTag,
                typename TClause::Requirements
            >
        > {

            // Clause metadata.

            /// Indicates whether the inspected Type is a SameProvider clause.
            static constexpr bool IsValid = true;

        };

    } // ESPressio::System::CompositionFramework::Detail


    /// Requires the contained Requirements to be satisfiable by pairwise-distinct provider Types.
    ///
    /// @tparam TRequirements Requirements requiring distinct provider assignments.
    template<class... TRequirements>
    struct DistinctProviders {

        static_assert(
            sizeof...(TRequirements) > 1U,
            "DistinctProviders requires at least two Requirements"
        );

        static_assert(
            (Detail::RequirementTraits<TRequirements>::IsValid && ...),
            "DistinctProviders entries must be consolidated Requirement declarations"
        );

        // Clause metadata.

        /// Marker identifying a DistinctProviders clause.
        using DistinctProvidersTag = void;

        /// Number of Requirements joined by this clause.
        static constexpr std::size_t Count = sizeof...(TRequirements);

        /// Requirements represented by this clause.
        using Requirements = Detail::RequirementList<TRequirements...>;

    };


    namespace Detail {

        /// Extracts DistinctProviders metadata.
        ///
        /// @tparam TClause DistinctProviders clause being inspected.
        template<class TClause>
        struct DistinctProvidersTraits<
            TClause,
            std::void_t<
                typename TClause::DistinctProvidersTag,
                typename TClause::Requirements
            >
        > {

            // Clause metadata.

            /// Indicates whether the inspected Type is a DistinctProviders clause.
            static constexpr bool IsValid = true;

        };

    } // ESPressio::System::CompositionFramework::Detail


    /// Declares that the owning provider must initialize after the uniquely selected provider satisfying one Requirement.
    ///
    /// @tparam TRequirement Requirement identifying the lifecycle predecessor.
    template<class TRequirement>
    struct InitializesAfter {

        static_assert(
            Detail::RequirementTraits<TRequirement>::IsValid,
            "InitializesAfter requires a consolidated Requirement"
        );

        // Lifecycle metadata.

        /// Marker identifying a lifecycle-ordering clause.
        using LifecycleClauseTag = void;

        /// Requirement identifying the initialization predecessor.
        using RequirementType = TRequirement;

        /// Indicates that this clause participates in initialization ordering.
        static constexpr bool IsInitializationClause = true;

        /// Indicates that this clause does not participate in shutdown ordering.
        static constexpr bool IsShutdownClause = false;

    };


    /// Declares that the owning provider must shut down before the uniquely selected provider satisfying one Requirement.
    ///
    /// @tparam TRequirement Requirement identifying the shutdown successor.
    template<class TRequirement>
    struct ShutsDownBefore {

        static_assert(
            Detail::RequirementTraits<TRequirement>::IsValid,
            "ShutsDownBefore requires a consolidated Requirement"
        );

        // Lifecycle metadata.

        /// Marker identifying a lifecycle-ordering clause.
        using LifecycleClauseTag = void;

        /// Requirement identifying the shutdown successor.
        using RequirementType = TRequirement;

        /// Indicates that this clause does not participate in initialization ordering.
        static constexpr bool IsInitializationClause = false;

        /// Indicates that this clause participates in shutdown ordering.
        static constexpr bool IsShutdownClause = true;

    };


    namespace Detail {

        /// Extracts lifecycle-clause metadata.
        ///
        /// @tparam TClause Lifecycle clause being inspected.
        template<class TClause>
        struct LifecycleClauseTraits<
            TClause,
            std::void_t<
                typename TClause::LifecycleClauseTag,
                typename TClause::RequirementType
            >
        > {

            // Clause metadata.

            /// Indicates whether the inspected Type is a lifecycle-ordering clause.
            static constexpr bool IsValid = true;

        };


        /// Indicates whether one Type is a valid Contract clause.
        ///
        /// @tparam TClause Type being classified.
        template<class TClause>
        inline constexpr bool IsContractClauseV =
            RequirementTraits<TClause>::IsValid ||
            SameProviderTraits<TClause>::IsValid ||
            DistinctProvidersTraits<TClause>::IsValid ||
            LifecycleClauseTraits<TClause>::IsValid;


        /// Indicates whether one consolidated Requirement is legal inside a provider Contract.
        ///
        /// @tparam TDomain Domain owning the provider Contract.
        /// @tparam TRequirement Requirement being validated.
        template<class TDomain, class TRequirement>
        inline constexpr bool ProviderRequirementIsValidV =
            (
                TRequirement::Scope == RequirementScope::SameDomain &&
                std::is_same_v<typename TRequirement::CompositionDomain, TDomain>
            ) ||
            (
                TRequirement::Scope == RequirementScope::ExternalDomain &&
                !std::is_same_v<typename TRequirement::CompositionDomain, TDomain>
            );


        /// Validates one Contract clause against an owning provider Domain.
        ///
        /// Unrelated relationship clauses are accepted here and validated by their dedicated specializations.
        ///
        /// @tparam TDomain Domain owning the provider Contract.
        /// @tparam TClause Contract clause being inspected.
        template<class TDomain, class TClause>
        struct ProviderClauseIsValid : std::true_type {};


        /// Validates one direct Requirement against an owning provider Domain.
        ///
        /// @tparam TDomain Domain owning the provider Contract.
        /// @tparam TCapability Capability requested by the Requirement.
        /// @tparam TScope Scope attached to the Requirement.
        /// @tparam TCardinality Provider cardinality attached to the Requirement.
        /// @tparam TConstraints Qualification constraints attached to the Requirement.
        template<
            class TDomain,
            class TCapability,
            RequirementScope TScope,
            class TCardinality,
            class... TConstraints
        >
        struct ProviderClauseIsValid<
            TDomain,
            Requirement<
                TCapability,
                TScope,
                TCardinality,
                TConstraints...
            >
        > : std::bool_constant<
            ProviderRequirementIsValidV<
                TDomain,
                Requirement<
                    TCapability,
                    TScope,
                    TCardinality,
                    TConstraints...
                >
            >
        > {};


        /// Validates nested SameProvider requirements against an owning provider Domain.
        ///
        /// @tparam TDomain Domain owning the provider Contract.
        /// @tparam TRequirements Nested Requirements.
        template<class TDomain, class... TRequirements>
        struct ProviderClauseIsValid<
            TDomain,
            SameProvider<TRequirements...>
        > : std::bool_constant<
            (ProviderRequirementIsValidV<TDomain, TRequirements> && ...)
        > {};


        /// Validates nested DistinctProviders requirements against an owning provider Domain.
        ///
        /// @tparam TDomain Domain owning the provider Contract.
        /// @tparam TRequirements Nested Requirements.
        template<class TDomain, class... TRequirements>
        struct ProviderClauseIsValid<
            TDomain,
            DistinctProviders<TRequirements...>
        > : std::bool_constant<
            (ProviderRequirementIsValidV<TDomain, TRequirements> && ...)
        > {};


        /// Validates one lifecycle Requirement against an owning provider Domain.
        ///
        /// @tparam TDomain Domain owning the provider Contract.
        /// @tparam TClause Lifecycle clause being inspected.
        template<class TDomain, class TClause>
        struct ProviderLifecycleClauseIsValid : std::false_type {};


        /// Validates InitializesAfter against an owning provider Domain.
        ///
        /// @tparam TDomain Domain owning the provider Contract.
        /// @tparam TRequirement Lifecycle predecessor Requirement.
        template<class TDomain, class TRequirement>
        struct ProviderLifecycleClauseIsValid<
            TDomain,
            InitializesAfter<TRequirement>
        > : std::bool_constant<
            ProviderRequirementIsValidV<TDomain, TRequirement>
        > {};


        /// Validates ShutsDownBefore against an owning provider Domain.
        ///
        /// @tparam TDomain Domain owning the provider Contract.
        /// @tparam TRequirement Lifecycle successor Requirement.
        template<class TDomain, class TRequirement>
        struct ProviderLifecycleClauseIsValid<
            TDomain,
            ShutsDownBefore<TRequirement>
        > : std::bool_constant<
            ProviderRequirementIsValidV<TDomain, TRequirement>
        > {};


        /// Routes lifecycle Contract clauses through their dedicated validation.
        ///
        /// @tparam TDomain Domain owning the provider Contract.
        /// @tparam TClause Contract clause being inspected.
        template<class TDomain, class TClause>
        inline constexpr bool ProviderClauseIsValidResolvedV =
            LifecycleClauseTraits<TClause>::IsValid
                ? ProviderLifecycleClauseIsValid<TDomain, TClause>::value
                : ProviderClauseIsValid<TDomain, TClause>::value;


        /// Indicates whether one Contract clause is a direct Requirement with the supplied scope.
        ///
        /// @tparam TClause Contract clause being inspected.
        /// @tparam TScope Requirement scope being matched.
        template<class TClause, RequirementScope TScope>
        struct DirectRequirementMatchesScope : std::false_type {};


        /// Matches one direct Requirement against the supplied scope.
        ///
        /// @tparam TCapability Capability requested by the Requirement.
        /// @tparam TRequirementScope Scope attached to the Requirement.
        /// @tparam TCardinality Provider cardinality attached to the Requirement.
        /// @tparam TConstraints Qualification constraints attached to the Requirement.
        /// @tparam TScope Requirement scope being matched.
        template<
            RequirementScope TScope,
            class TCapability,
            RequirementScope TRequirementScope,
            class TCardinality,
            class... TConstraints
        >
        struct DirectRequirementMatchesScope<
            Requirement<
                TCapability,
                TRequirementScope,
                TCardinality,
                TConstraints...
            >,
            TScope
        > : std::bool_constant<
            TRequirementScope == TScope
        > {};


        /// Collects direct Requirements in one Contract matching the supplied scope.
        ///
        /// @tparam TScope Requirement scope to collect.
        /// @tparam TAccumulatedRequirements RequirementList accumulated so far.
        /// @tparam TClauses Remaining Contract clauses.
        template<
            RequirementScope TScope,
            class TAccumulatedRequirements,
            class... TClauses
        >
        struct CollectRequirementsByScope;


        /// Completes scoped Requirement collection.
        ///
        /// @tparam TScope Requirement scope being collected.
        /// @tparam TRequirements Accumulated Requirements.
        template<
            RequirementScope TScope,
            class... TRequirements
        >
        struct CollectRequirementsByScope<
            TScope,
            RequirementList<TRequirements...>
        > {

            // Collection result.

            /// Direct Requirements matching the requested scope.
            using Type = RequirementList<TRequirements...>;

        };


        /// Inspects one Contract clause and continues scoped Requirement collection.
        ///
        /// @tparam TScope Requirement scope being collected.
        /// @tparam TRequirements Accumulated Requirements.
        /// @tparam TFirstClause Current Contract clause.
        /// @tparam TRestClauses Remaining Contract clauses.
        template<
            RequirementScope TScope,
            class... TRequirements,
            class TFirstClause,
            class... TRestClauses
        >
        struct CollectRequirementsByScope<
            TScope,
            RequirementList<TRequirements...>,
            TFirstClause,
            TRestClauses...
        > {

            // Collection state.

            /// Next Requirement list after conditionally adding the current direct Requirement.
            using NextRequirements = std::conditional_t<
                DirectRequirementMatchesScope<
                    TFirstClause,
                    TScope
                >::value,
                RequirementList<TRequirements..., TFirstClause>,
                RequirementList<TRequirements...>
            >;

            /// Final scoped Requirement list.
            using Type = typename CollectRequirementsByScope<
                TScope,
                NextRequirements,
                TRestClauses...
            >::Type;

        };


        /// Default Contract metadata for unrelated Types.
        ///
        /// @tparam TContract Type being inspected.
        /// @tparam TEnable SFINAE helper used when Contract metadata is present.
        template<class TContract, class TEnable = void>
        struct ContractTraits {

            // Contract metadata.

            /// Indicates whether the inspected Type is a Contract.
            static constexpr bool IsValid = false;

        };

    } // ESPressio::System::CompositionFramework::Detail


    /// Groups reusable compile-time architectural requirements for a provider or standalone consumer.
    ///
    /// A Contract contains no runtime state. Provider Contracts may use SameDomain and ExternalDomain
    /// Requirements. Standalone consumer Contracts may additionally use AnyDomain Requirements.
    ///
    /// @tparam TClauses Requirement and relationship clauses forming this Contract.
    template<class... TClauses>
    struct Contract {

        static_assert(
            (Detail::IsContractClauseV<TClauses> && ...),
            "Contract entries must be Requirement, SameProvider, DistinctProviders or lifecycle clauses"
        );

        // Contract metadata.

        /// Marker identifying this consolidated Contract declaration.
        using ContractTag = void;

        /// Number of clauses represented by this Contract.
        static constexpr std::size_t Count = sizeof...(TClauses);

        /// Complete compile-time Contract clause list.
        using Clauses = Detail::ContractClauseList<TClauses...>;


        // Provider-contract validation.

        /// Indicates whether this Contract is legal for a provider owned by the supplied Domain.
        ///
        /// @tparam TDomain Provider Domain against which scope relationships are validated.
        template<class TDomain>
        static constexpr bool IsProviderContractFor =
            IsDomainV<TDomain> &&
            (Detail::ProviderClauseIsValidResolvedV<TDomain, TClauses> && ...);


        // Scoped direct-Requirement inspection.

        /// Returns direct Requirements in this Contract having the requested scope.
        ///
        /// Relationship/lifecycle clauses remain represented in Clauses and are validated separately.
        ///
        /// @tparam TScope Requirement scope to collect.
        template<RequirementScope TScope>
        using RequirementsForScope = typename Detail::CollectRequirementsByScope<
            TScope,
            Detail::RequirementList<>,
            TClauses...
        >::Type;

    };


    namespace Detail {

        /// Extracts Contract metadata.
        ///
        /// @tparam TContract Contract Type being inspected.
        template<class TContract>
        struct ContractTraits<
            TContract,
            std::void_t<
                typename TContract::ContractTag,
                typename TContract::Clauses
            >
        > {

            // Contract metadata.

            /// Indicates whether the inspected Type is a Contract.
            static constexpr bool IsValid = true;

        };

    } // ESPressio::System::CompositionFramework::Detail

} // ESPressio::System::CompositionFramework
