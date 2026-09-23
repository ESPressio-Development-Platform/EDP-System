# src/composition/Architecture.hpp

**Primary classification:** PUBLIC COMPOSITION API

**Source baseline:** `9964ab2820cc5d290de69674000e011faaa684d9`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/9964ab2820cc5d290de69674000e011faaa684d9/src/composition/Architecture.hpp)

## Direct includes

- `cstddef`
- `type_traits`
- `Composition.hpp`

## Documented declarations

### `TComposition`

**Classification:** PUBLIC COMPOSITION API

Default metadata for types that are not Composition declarations.

```cpp
template<class TComposition, class = void>
        struct CompositionTraits
```

### `IsValid`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the inspected type is a valid Composition declaration.

```cpp
static constexpr bool IsValid = false;
```

### `DomainType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Domain associated with the inspected type when one exists.

```cpp
using DomainType = void;
```

### `TComposition`

**Classification:** PUBLIC COMPOSITION API

Extracts metadata from one Composition declaration.

```cpp
template<class TComposition>
        struct CompositionTraits<
            TComposition,
```

### `IsValid`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the inspected type represents a Composition over a concrete Domain.

```cpp
static constexpr bool IsValid = IsDomainV<typename TComposition::CompositionDomain>;
```

### `DomainType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Domain represented by the inspected Composition.

```cpp
using DomainType = typename TComposition::CompositionDomain;
```

### `UniqueCompositionDomains`

**Classification:** PUBLIC COMPOSITION API

Indicates whether every Composition domain in one pack is unique.

```cpp
template<class... TCompositions>
        struct UniqueCompositionDomains;
```

### `UniqueCompositionDomains`

**Classification:** PUBLIC COMPOSITION API

Empty Composition packs contain no duplicate domains.

```cpp
template<>
        struct UniqueCompositionDomains<> : std::true_type {};
```

### `TFirstComposition`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Checks the first Composition domain against the remaining declarations and continues recursively.

```cpp
template<class TFirstComposition, class... TRestCompositions>
        struct UniqueCompositionDomains<TFirstComposition, TRestCompositions...> : std::bool_constant<
            ((!std::is_same_v<
                typename TFirstComposition::CompositionDomain,
                typename TRestCompositions::CompositionDomain
            >) && ...) &&
            UniqueCompositionDomains<TRestCompositions...>::value
        > {};
```

### `ArchitectureProviderTypes`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Concatenates every provider list represented by one Architecture.

- **Template parameter `TCompositions`:** Domain Compositions whose providers are being flattened.

```cpp
template<class... TCompositions>
        struct ArchitectureProviderTypes;
```

### `ArchitectureProviderTypes`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Empty Architecture provider population.

```cpp
template<>
        struct ArchitectureProviderTypes<>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Empty provider list.

```cpp
using Type = ProviderList<>;
```

### `TFirstComposition`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Concatenates one Composition's providers with the remaining Architecture providers.

- **Template parameter `TFirstComposition`:** First Composition in declaration order.
- **Template parameter `TRestCompositions`:** Remaining Compositions.

```cpp
template<class TFirstComposition, class... TRestCompositions>
        struct ArchitectureProviderTypes<
            TFirstComposition,
```

### `Remaining`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Flattened provider list from the remaining Compositions.

```cpp
using Remaining = typename ArchitectureProviderTypes<
                    TRestCompositions...
                >::Type;
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Complete provider list preserving Composition and provider declaration order.

```cpp
using Type = ProviderListConcat<
                    typename TFirstComposition::ProviderTypes,
```

### `TClause`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one consolidated Contract clause across a complete Architecture provider population.

- **Template parameter `TClause`:** Contract clause being evaluated.
- **Template parameter `TProviderList`:** Complete Architecture provider population.

```cpp
template<class TClause, class TProviderList>
        struct ContractClauseSatisfiedInArchitecture : std::true_type {};
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one direct consolidated Requirement across an Architecture.

- **Template parameter `TProviderList`:** Complete Architecture provider population.
- **Template parameter `TCapability`:** Requested Capability.
- **Template parameter `TScope`:** Requirement scope.
- **Template parameter `TCardinality`:** Accepted satisfying-provider count.
- **Template parameter `TConstraints`:** Requirement qualification constraints.

```cpp
template<
            class TProviderList,
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one SameProvider relationship across an Architecture.

- **Template parameter `TProviderList`:** Complete Architecture provider population.
- **Template parameter `TRequirements`:** Requirements requiring one joint provider.

```cpp
template<class TProviderList, class... TRequirements>
        struct ContractClauseSatisfiedInArchitecture<
            SameProvider<TRequirements...>,
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one DistinctProviders relationship across an Architecture.

- **Template parameter `TProviderList`:** Complete Architecture provider population.
- **Template parameter `TRequirements`:** Requirements requiring distinct providers.

```cpp
template<class TProviderList, class... TRequirements>
        struct ContractClauseSatisfiedInArchitecture<
            DistinctProviders<TRequirements...>,
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires one unambiguous initialization predecessor across the Architecture.

- **Template parameter `TProviderList`:** Complete Architecture provider population.
- **Template parameter `TRequirement`:** Lifecycle predecessor Requirement.

```cpp
template<class TProviderList, class TRequirement>
        struct ContractClauseSatisfiedInArchitecture<
            InitializesAfter<TRequirement>,
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Requires one unambiguous shutdown successor across the Architecture.

- **Template parameter `TProviderList`:** Complete Architecture provider population.
- **Template parameter `TRequirement`:** Lifecycle successor Requirement.

```cpp
template<class TProviderList, class TRequirement>
        struct ContractClauseSatisfiedInArchitecture<
            ShutsDownBefore<TRequirement>,
```

### `TContract`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one consolidated Contract across an Architecture.

- **Template parameter `TContract`:** Contract.
- **Template parameter `TProviderList`:** Complete Architecture provider population.

```cpp
template<class TContract, class TProviderList>
        struct ContractSatisfiedInArchitecture : std::true_type {};
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates every clause in one concrete Contract.

- **Template parameter `TProviderList`:** Complete Architecture provider population.
- **Template parameter `TClauses`:** Contract clauses being evaluated.

```cpp
template<class TProviderList, class... TClauses>
        struct ContractSatisfiedInArchitecture<
            Contract<TClauses...>,
```

### `TOwnerProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Resolves whether one lifecycle Requirement selects exactly one provider other than its owner.

- **Template parameter `TOwnerProvider`:** Provider owning the lifecycle clause.
- **Template parameter `TRequirement`:** Lifecycle target Requirement.
- **Template parameter `TProviderList`:** Complete Architecture provider population.

```cpp
template<
            class TOwnerProvider,
```

### `TOwnerProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one lifecycle target against a concrete provider population.

- **Template parameter `TOwnerProvider`:** Provider owning the lifecycle clause.
- **Template parameter `TRequirement`:** Lifecycle target Requirement.
- **Template parameter `TProviders`:** Architecture provider Types.

```cpp
template<
            class TOwnerProvider,
```

### `MatchCount`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Number of providers satisfying the lifecycle target Requirement.

```cpp
static constexpr std::size_t MatchCount =
                    SatisfyingProviderCountV<
                        TRequirement,
```

### `TargetProvider`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

First satisfying provider Type, used only after count validation.

```cpp
using TargetProvider = typename FirstSatisfyingProvider<
                    TRequirement,
```

### `IsValid`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the lifecycle target is unique and not the owning provider itself.

```cpp
static constexpr bool IsValid =
                    MatchCount == 1U &&
                    !std::is_same_v<
                        TOwnerProvider,
```

### `TOwnerProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one owner-aware provider Contract clause across an Architecture.

- **Template parameter `TOwnerProvider`:** Provider owning the Contract.
- **Template parameter `TClause`:** Contract clause being evaluated.
- **Template parameter `TProviderList`:** Complete Architecture provider population.

```cpp
template<
            class TOwnerProvider,
```

### `TOwnerProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Rejects ambiguous or self-referential initialization lifecycle targets.

- **Template parameter `TOwnerProvider`:** Provider owning the lifecycle clause.
- **Template parameter `TRequirement`:** Initialization predecessor Requirement.
- **Template parameter `TProviderList`:** Complete Architecture provider population.

```cpp
template<
            class TOwnerProvider,
```

### `TOwnerProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Rejects ambiguous or self-referential shutdown lifecycle targets.

- **Template parameter `TOwnerProvider`:** Provider owning the lifecycle clause.
- **Template parameter `TRequirement`:** Shutdown successor Requirement.
- **Template parameter `TProviderList`:** Complete Architecture provider population.

```cpp
template<
            class TOwnerProvider,
```

### `TOwnerProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one provider-owned consolidated Contract across an Architecture.

- **Template parameter `TOwnerProvider`:** Provider owning the Contract.
- **Template parameter `TContract`:** Contract.
- **Template parameter `TProviderList`:** Complete Architecture provider population.

```cpp
template<
            class TOwnerProvider,
```

### `TOwnerProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates every provider-owned clause in one concrete Contract.

- **Template parameter `TOwnerProvider`:** Provider owning the Contract.
- **Template parameter `TProviderList`:** Complete Architecture provider population.
- **Template parameter `TClauses`:** Contract clauses being evaluated.

```cpp
template<
            class TOwnerProvider,
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates every provider Contract represented by one ProviderList.

- **Template parameter `TProviderList`:** Providers whose Contracts are being validated.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.

```cpp
template<class TProviderList, class TArchitectureProviders>
        struct ProviderListContractsSatisfied;
```

### `TArchitectureProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates every concrete provider Contract.

- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TProviders`:** Providers whose Contracts are being validated.

```cpp
template<class TArchitectureProviders, class... TProviders>
        struct ProviderListContractsSatisfied<
            ProviderList<TProviders...>,
```

### `TClause`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether one lifecycle clause creates an ordering edge between two providers.

- **Template parameter `TClause`:** Contract clause being inspected.
- **Template parameter `TBefore`:** Provider Type proposed before the other provider.
- **Template parameter `TAfter`:** Provider Type proposed after the other provider.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TInitializationOrder`:** Whether initialization ordering is being derived.

```cpp
template<
            class TClause,
```

### `TBefore`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Creates an initialization edge from the uniquely selected predecessor to the owning provider.

- **Template parameter `TBefore`:** Candidate predecessor provider Type.
- **Template parameter `TAfter`:** Provider Type owning the InitializesAfter clause.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TRequirement`:** Requirement selecting the predecessor.

```cpp
template<
            class TBefore,
```

### `TBefore`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Creates a shutdown edge from the owning provider to the uniquely selected successor.

- **Template parameter `TBefore`:** Provider Type owning the ShutsDownBefore clause.
- **Template parameter `TAfter`:** Candidate shutdown successor provider Type.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TRequirement`:** Requirement selecting the successor.

```cpp
template<
            class TBefore,
```

### `TContract`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether one Contract establishes an ordering edge between two providers.

- **Template parameter `TContract`:** Contract being inspected.
- **Template parameter `TBefore`:** Provider Type proposed before the other provider.
- **Template parameter `TAfter`:** Provider Type proposed after the other provider.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TInitializationOrder`:** Whether initialization ordering is being derived.

```cpp
template<
            class TContract,
```

### `TBefore`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates every lifecycle clause in one consolidated Contract.

- **Template parameter `TBefore`:** Provider Type proposed before the other provider.
- **Template parameter `TAfter`:** Provider Type proposed after the other provider.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TInitializationOrder`:** Whether initialization ordering is being derived.
- **Template parameter `TClauses`:** Contract clauses being inspected.

```cpp
template<
            class TBefore,
```

### `TBefore`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether one directed lifecycle edge exists between two providers.

Initialization edges are declared by the after-provider's Contract.
Shutdown edges are declared by the before-provider's Contract.

- **Template parameter `TBefore`:** Provider Type proposed before the other provider.
- **Template parameter `TAfter`:** Provider Type proposed after the other provider.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TInitializationOrder`:** Whether initialization ordering is being derived.

```cpp
template<
            class TBefore,
```

### `TCandidate`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether one provider has any incoming lifecycle edge from the remaining provider set.

- **Template parameter `TCandidate`:** Provider Type being tested for readiness.
- **Template parameter `TRemainingProviders`:** Remaining providers in the topological sort.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TInitializationOrder`:** Whether initialization ordering is being derived.

```cpp
template<
            class TCandidate,
```

### `TCandidate`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Evaluates incoming edges from one concrete remaining provider pack.

- **Template parameter `TCandidate`:** Provider Type being tested for readiness.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TInitializationOrder`:** Whether initialization ordering is being derived.
- **Template parameter `TProviders`:** Remaining provider Types.

```cpp
template<
            class TCandidate,
```

### `TCandidates`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Finds the first declaration-order provider with no incoming lifecycle edge from the complete remaining set.

- **Template parameter `TCandidates`:** Candidate providers still being inspected for readiness.
- **Template parameter `TRemainingProviders`:** Complete remaining provider set used for incoming-edge checks.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TInitializationOrder`:** Whether initialization ordering is being derived.

```cpp
template<
            class TCandidates,
```

### `TRemainingProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

No provider can be selected after all candidates are exhausted.

- **Template parameter `TRemainingProviders`:** Complete remaining provider set.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TInitializationOrder`:** Whether initialization ordering is being derived.

```cpp
template<
            class TRemainingProviders,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Empty-selection sentinel indicating a lifecycle cycle.

```cpp
using Type = void;
```

### `TRemainingProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects the first candidate having no incoming edge from any provider still remaining.

- **Template parameter `TRemainingProviders`:** Complete remaining provider set.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TInitializationOrder`:** Whether initialization ordering is being derived.
- **Template parameter `TFirstProvider`:** Current candidate provider Type.
- **Template parameter `TRestProviders`:** Remaining candidate provider Types.

```cpp
template<
            class TRemainingProviders,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

First ready provider Type, or the result of inspecting the remaining candidates.

```cpp
using Type = std::conditional_t<
                !HasIncomingLifecycleEdge<
                    TFirstProvider,
```

### `TOrderedProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Builds one deterministic lifecycle provider ordering.

- **Template parameter `TOrderedProviders`:** Providers already placed in lifecycle order.
- **Template parameter `TRemainingProviders`:** Providers still requiring placement.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TInitializationOrder`:** Whether initialization ordering is being derived.

```cpp
template<
            class TOrderedProviders,
```

### `TOrderedProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Completes lifecycle ordering when no providers remain.

- **Template parameter `TOrderedProviders`:** Complete provider order.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TInitializationOrder`:** Whether initialization ordering is being derived.

```cpp
template<
            class TOrderedProviders,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Completed lifecycle order.

```cpp
using Type = TOrderedProviders;
```

### `TOrderedProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Continues lifecycle ordering after selecting one ready provider.

- **Template parameter `TOrderedProviders`:** Providers already placed in lifecycle order.
- **Template parameter `TRemainingProviders`:** Providers still requiring placement.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TInitializationOrder`:** Whether initialization ordering is being derived.
- **Template parameter `TReadyProvider`:** Selected ready provider Type.

```cpp
template<
            class TOrderedProviders,
```

### `NextRemaining`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Remaining provider set after consuming the ready provider.

```cpp
using NextRemaining = typename RemoveProviderFromList<
                TReadyProvider,
```

### `NextOrdered`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Ordered provider set after appending the ready provider.

```cpp
using NextOrdered = typename AppendProviderList<
                TOrderedProviders,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Completed deterministic lifecycle order.

```cpp
using Type = typename BuildLifecycleOrder<
                NextOrdered,
```

### `TOrderedProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Rejects a lifecycle dependency cycle when no remaining provider is ready.

- **Template parameter `TOrderedProviders`:** Providers placed before the cycle was detected.
- **Template parameter `TRemainingProviders`:** Providers participating in the unresolved cycle.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TInitializationOrder`:** Whether initialization ordering is being derived.

```cpp
template<
            class TOrderedProviders,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Unreachable fallback Type retained only to terminate template substitution after the diagnostic.

```cpp
using Type = TOrderedProviders;
```

### `TOrderedProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects one ready provider and continues deterministic lifecycle ordering.

- **Template parameter `TOrderedProviders`:** Providers already placed in lifecycle order.
- **Template parameter `TFirstProvider`:** First remaining provider Type.
- **Template parameter `TRestProviders`:** Remaining provider Types.
- **Template parameter `TArchitectureProviders`:** Complete Architecture provider population.
- **Template parameter `TInitializationOrder`:** Whether initialization ordering is being derived.

```cpp
template<
            class TOrderedProviders,
```

### `RemainingProviders`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Complete remaining provider list.

```cpp
using RemainingProviders = ProviderList<
                    TFirstProvider,
```

### `ReadyProvider`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

First provider ready under the requested lifecycle relation.

```cpp
using ReadyProvider = typename FindLifecycleReadyProvider<
                    RemainingProviders,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Completed deterministic lifecycle order.

```cpp
using Type = typename ContinueLifecycleOrder<
                    TOrderedProviders,
```

### `TDomain`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Counts Compositions representing one requested Domain.

```cpp
template<class TDomain, class... TCompositions>
        inline constexpr std::size_t CompositionCountForDomainV =
            (std::size_t{0U} + ... + (
                std::is_same_v<TDomain, typename TCompositions::CompositionDomain>
                    ? std::size_t{1U}
                    : std::size_t{0U}
            ));
```

### `TDomain`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Locates the Composition representing one requested Domain.

```cpp
template<class TDomain, class... TCompositions>
        struct FindComposition;
```

### `TDomain`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Represents an unsuccessful Composition lookup.

```cpp
template<class TDomain>
        struct FindComposition<TDomain>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Type returned when no Composition represents the requested Domain.

```cpp
using Type = void;
```

### `TDomain`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Continues a Composition lookup until the requested Domain is found.

```cpp
template<class TDomain, class TFirstComposition, class... TRestCompositions>
        struct FindComposition<TDomain, TFirstComposition, TRestCompositions...>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Composition representing the requested Domain, or the result of searching remaining Compositions.

```cpp
using Type = std::conditional_t<
                std::is_same_v<TDomain, typename TFirstComposition::CompositionDomain>,
```

### `Architecture`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Validates cross-domain dependencies across a set of already-valid Domain Compositions.

```cpp
template<class... TCompositions>
    struct Architecture
```

### `ProviderTypes`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Complete deterministic provider population across all participating Compositions.

```cpp
using ProviderTypes = typename Detail::ArchitectureProviderTypes<
            TCompositions...
        >::Type;
```

### `InitializationOrder`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Deterministic provider initialization order derived only from explicit InitializesAfter clauses.

```cpp
using InitializationOrder = typename Detail::BuildLifecycleOrder<
            ProviderList<>,
```

### `ShutdownOrder`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Deterministic provider shutdown order derived only from explicit ShutsDownBefore clauses.

```cpp
using ShutdownOrder = typename Detail::BuildLifecycleOrder<
            ProviderList<>,
```

### `AllContractsSatisfied`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether every provider Contract is satisfied across the complete Architecture.

```cpp
static constexpr bool AllContractsSatisfied =
            (
                Detail::ProviderListContractsSatisfied<
                    typename TCompositions::ProviderTypes,
                    ProviderTypes
                >::value &&
                ...
            );
```

### `ArchitectureDeclarationTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker used to identify Architecture declarations during compile-time inspection.

```cpp
using ArchitectureDeclarationTag = void;
```

### `CompositionCount`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Number of Domain Compositions participating in this Architecture.

```cpp
static constexpr std::size_t CompositionCount = sizeof...(TCompositions);
```

### `IsValid`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates that all participating Compositions and provider Contracts are valid.

```cpp
static constexpr bool IsValid =
            AllContractsSatisfied;
```

### `TDomain`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Resolves the unique Composition representing one requested Domain.

```cpp
template<class TDomain>
        struct ResolveComposition
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Composition representing the requested Domain.

```cpp
using Type = typename Detail::FindComposition<TDomain, TCompositions...>::Type;
```

### `TDomain`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns the unique Composition representing one requested Domain.

```cpp
template<class TDomain>
        using CompositionFor = typename ResolveComposition<TDomain>::Type;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Validates one Requirement before Architecture-wide provider matching is attempted.

```cpp
template<class TRequirement>
        struct ValidateRequirement
```

### `IsValid`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates that the Requirement is valid for this Architecture.

```cpp
static constexpr bool IsValid = true;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns every provider matching one Requirement qualification.

Matching is delegated to the Composition owning the Requirement Capability Domain.

- **Template parameter `TRequirement`:** Requirement being queried.

```cpp
template<class TRequirement>
        using Matches = std::conditional_t<
            ValidateRequirement<TRequirement>::IsValid,
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns the number of providers matching one Requirement qualification.

- **Template parameter `TRequirement`:** Requirement being queried.

```cpp
template<class TRequirement>
        static constexpr std::size_t MatchCount =
            Matches<TRequirement>::Count;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether at least one provider matches one Requirement qualification.

- **Template parameter `TRequirement`:** Requirement being queried.

```cpp
template<class TRequirement>
        static constexpr bool HasMatch =
            MatchCount<TRequirement> > 0U;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the Architecture provider population obeys one Requirement's cardinality.

- **Template parameter `TRequirement`:** Requirement being validated.

```cpp
template<class TRequirement>
        static constexpr bool SatisfiesRequirement =
            Detail::RequirementCardinalitySatisfied<
                TRequirement,
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Resolves Requirement matches using one explicit selection policy.

- **Template parameter `TRequirement`:** Requirement whose matching providers are being selected.
- **Template parameter `TSelectionPolicy`:** Explicit provider selection policy.

```cpp
template<
            class TRequirement,
```

### `JointMatches`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns providers jointly satisfying every supplied Requirement across the complete Architecture.

- **Template parameter `TRequirements`:** Requirements which must all be satisfied by the same provider Type.

```cpp
template<class... TRequirements>
        using JointMatches = typename Detail::FilterJointlySatisfyingProviders<
            Detail::RequirementList<TRequirements...>,
```

### `JointMatchCount`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns the number of providers jointly satisfying every supplied Requirement.

- **Template parameter `TRequirements`:** Requirements being jointly evaluated.

```cpp
template<class... TRequirements>
        static constexpr std::size_t JointMatchCount =
            JointMatches<TRequirements...>::Count;
```

### `TSelectionPolicy`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Resolves providers jointly satisfying every supplied Requirement using one explicit selection policy.

- **Template parameter `TSelectionPolicy`:** Explicit provider selection policy.
- **Template parameter `TRequirements`:** Requirements which must all be satisfied by the same provider Type.

```cpp
template<class TSelectionPolicy, class... TRequirements>
        using SelectJoint = typename Detail::ProviderListSelectionResult<
            TSelectionPolicy,
```

### `TContract`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the complete Architecture satisfies one reusable consumer Contract.

- **Template parameter `TContract`:** Consolidated consumer Contract.

```cpp
template<class TContract>
        static constexpr bool SatisfiesContract =
            Detail::ContractTraits<TContract>::IsValid &&
            Detail::ContractSatisfiedInArchitecture<
                TContract,
```

### `TContract`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Performs strict compile-time validation of one consumer Contract.

- **Template parameter `TContract`:** Consolidated consumer Contract.

```cpp
template<class TContract>
        struct ValidateContract
```

### `IsValid`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates successful consumer Contract validation.

```cpp
static constexpr bool IsValid = true;
```

