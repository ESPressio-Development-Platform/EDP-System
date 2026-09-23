# src/composition/Contract.hpp

**Primary classification:** INTERNAL PROVIDER API

**Source baseline:** `9964ab2820cc5d290de69674000e011faaa684d9`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/9964ab2820cc5d290de69674000e011faaa684d9/src/composition/Contract.hpp)

## Direct includes

- `cstddef`
- `cstdint`
- `limits`
- `type_traits`
- `Requirement.hpp`

## Documented declarations

### `RequirementScope`

**Classification:** INTERNAL PROVIDER API

Requirement scope restricting where a provider may satisfy one compile-time Requirement.

```cpp
enum class RequirementScope : std::uint8_t
```

### `ProviderCardinality`

**Classification:** INTERNAL PROVIDER API

Compile-time provider-count interval attached to one Requirement.

- **Template parameter `TMinimum`:** Minimum number of satisfying providers.
- **Template parameter `TMaximum`:** Maximum number of satisfying providers.

```cpp
template<std::size_t TMinimum, std::size_t TMaximum>
    struct ProviderCardinality
```

### `Minimum`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Minimum accepted satisfying-provider count.

```cpp
static constexpr std::size_t Minimum = TMinimum;
```

### `Maximum`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Maximum accepted satisfying-provider count.

```cpp
static constexpr std::size_t Maximum = TMaximum;
```

### `IsSatisfied`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether a satisfying-provider count falls inside this cardinality interval.

- **Parameter `count`:** Number of providers satisfying the associated Requirement.

```cpp
static constexpr bool IsSatisfied(
            std::size_t count
        ) noexcept
```

### `ExactlyProviders`

**Classification:** INTERNAL PROVIDER API

Requires exactly the specified number of satisfying providers.

- **Template parameter `TCount`:** Required provider count.

```cpp
template<std::size_t TCount>
    using ExactlyProviders = ProviderCardinality<TCount, TCount>;
```

### `AtLeastProviders`

**Classification:** INTERNAL PROVIDER API

Requires at least the specified number of satisfying providers.

- **Template parameter `TCount`:** Minimum accepted provider count.

```cpp
template<std::size_t TCount>
    using AtLeastProviders = ProviderCardinality<
        TCount,
```

### `AtMostProviders`

**Classification:** INTERNAL PROVIDER API

Requires no more than the specified number of satisfying providers.

- **Template parameter `TCount`:** Maximum accepted provider count.

```cpp
template<std::size_t TCount>
    using AtMostProviders = ProviderCardinality<0U, TCount>;
```

### `ProviderCountBetween`

**Classification:** INTERNAL PROVIDER API

Requires a satisfying-provider count inside the supplied inclusive interval.

- **Template parameter `TMinimum`:** Minimum accepted provider count.
- **Template parameter `TMaximum`:** Maximum accepted provider count.

```cpp
template<std::size_t TMinimum, std::size_t TMaximum>
    using ProviderCountBetween = ProviderCardinality<TMinimum, TMaximum>;
```

### `AnyProviderCount`

**Classification:** INTERNAL PROVIDER API

Accepts any satisfying-provider count, including zero.

```cpp
using AnyProviderCount = ProviderCardinality<
        0U,
```

### `TCapability`

**Classification:** INTERNAL PROVIDER API

Declares one capability requirement together with its scope, cardinality and qualification constraints.

A Requirement describes qualification and architectural necessity. It does not choose a provider
when several providers satisfy the qualification; selection remains an explicit query concern.

- **Template parameter `TCapability`:** Capability required by this declaration.
- **Template parameter `TScope`:** Scope in which a satisfying provider may be found.
- **Template parameter `TCardinality`:** Accepted count of providers satisfying the complete requirement.
- **Template parameter `TConstraints`:** Compile-time Property, FlagProperty or Attribute constraints.

```cpp
template<
        class TCapability,
```

### `RequirementTag`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Marker identifying the consolidated Requirement declaration.

```cpp
using RequirementTag = void;
```

### `CapabilityType`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Capability required by this declaration.

```cpp
using CapabilityType = TCapability;
```

### `CompositionDomain`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Domain inherited from the requested capability.

```cpp
using CompositionDomain = typename Detail::CapabilityTraits<TCapability>::DomainType;
```

### `Cardinality`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Cardinality policy attached to this Requirement.

```cpp
using Cardinality = TCardinality;
```

### `Scope`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Scope in which satisfying providers are permitted.

```cpp
static constexpr RequirementScope Scope = TScope;
```

### `ConstraintCount`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Number of qualification constraints attached to this Requirement.

```cpp
static constexpr std::size_t ConstraintCount = sizeof...(TConstraints);
```

### `AcceptsProviderCount`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether a satisfying-provider count obeys this Requirement's cardinality.

- **Parameter `count`:** Number of providers satisfying the Requirement qualification.

```cpp
static constexpr bool AcceptsProviderCount(
            std::size_t count
        ) noexcept
```

### `TPropertySet`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether a supplied PropertySet satisfies every property-only constraint.

Attribute constraints cannot be satisfied from a PropertySet-only view.

- **Template parameter `TPropertySet`:** PropertySet being inspected.

```cpp
template<class TPropertySet>
        static constexpr bool PropertiesSatisfied =
            (Detail::ConstraintSatisfiedByProperties<TConstraints, TPropertySet>::value && ...);
```

### `TOffer`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether one complete Offer satisfies every qualification constraint.

- **Template parameter `TOffer`:** Offer being inspected.

```cpp
template<class TOffer>
        static constexpr bool OfferSatisfied = (
            Detail::RequirementConstraintSatisfiedByOffer<
                TConstraints,
                TOffer
            >() &&
            ...
        );
```

### `TRequirement`

**Classification:** INTERNAL PROVIDER API

Default consolidated Requirement metadata for unrelated Types.

- **Template parameter `TRequirement`:** Type being inspected.
- **Template parameter `TEnable`:** SFINAE helper used when Requirement metadata is present.

```cpp
template<class TRequirement, class TEnable = void>
        struct RequirementTraits
```

### `IsValid`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether the inspected Type is a consolidated Requirement.

```cpp
static constexpr bool IsValid = false;
```

### `TRequirement`

**Classification:** INTERNAL PROVIDER API

Extracts consolidated Requirement metadata.

- **Template parameter `TRequirement`:** Requirement Type being inspected.

```cpp
template<class TRequirement>
        struct RequirementTraits<
            TRequirement,
```

### `IsValid`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether the inspected Type references a concrete Capability.

```cpp
static constexpr bool IsValid = IsCapabilityV<typename TRequirement::CapabilityType>;
```

### `RequirementList`

**Classification:** INTERNAL PROVIDER API

Stores a compile-time list of consolidated Requirement Types.

- **Template parameter `TRequirements`:** Requirement Types represented by this internal list.

```cpp
template<class... TRequirements>
        struct RequirementList {};
```

### `ContractClauseList`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Stores a compile-time list of Contract clause Types.

- **Template parameter `TClauses`:** Contract clauses represented by this internal list.

```cpp
template<class... TClauses>
        struct ContractClauseList {};
```

### `TClause`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Default SameProvider metadata for unrelated Types.

- **Template parameter `TClause`:** Type being inspected.
- **Template parameter `TEnable`:** SFINAE helper used when SameProvider metadata is present.

```cpp
template<class TClause, class TEnable = void>
        struct SameProviderTraits
```

### `IsValid`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether the inspected Type is a SameProvider clause.

```cpp
static constexpr bool IsValid = false;
```

### `TClause`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Default DistinctProviders metadata for unrelated Types.

- **Template parameter `TClause`:** Type being inspected.
- **Template parameter `TEnable`:** SFINAE helper used when DistinctProviders metadata is present.

```cpp
template<class TClause, class TEnable = void>
        struct DistinctProvidersTraits
```

### `IsValid`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether the inspected Type is a DistinctProviders clause.

```cpp
static constexpr bool IsValid = false;
```

### `TClause`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Default lifecycle-clause metadata for unrelated Types.

- **Template parameter `TClause`:** Type being inspected.
- **Template parameter `TEnable`:** SFINAE helper used when lifecycle metadata is present.

```cpp
template<class TClause, class TEnable = void>
        struct LifecycleClauseTraits
```

### `IsValid`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether the inspected Type is a lifecycle-ordering clause.

```cpp
static constexpr bool IsValid = false;
```

### `SameProvider`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Requires one concrete provider Type to satisfy every contained Requirement.

- **Template parameter `TRequirements`:** Requirements which must be jointly satisfied by one provider Type.

```cpp
template<class... TRequirements>
    struct SameProvider
```

### `SameProviderTag`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Marker identifying a SameProvider clause.

```cpp
using SameProviderTag = void;
```

### `Count`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Number of Requirements joined by this clause.

```cpp
static constexpr std::size_t Count = sizeof...(TRequirements);
```

### `Requirements`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Requirements represented by this clause.

```cpp
using Requirements = Detail::RequirementList<TRequirements...>;
```

### `TClause`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Extracts SameProvider metadata.

- **Template parameter `TClause`:** SameProvider clause being inspected.

```cpp
template<class TClause>
        struct SameProviderTraits<
            TClause,
```

### `IsValid`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether the inspected Type is a SameProvider clause.

```cpp
static constexpr bool IsValid = true;
```

### `DistinctProviders`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Requires the contained Requirements to be satisfiable by pairwise-distinct provider Types.

- **Template parameter `TRequirements`:** Requirements requiring distinct provider assignments.

```cpp
template<class... TRequirements>
    struct DistinctProviders
```

### `DistinctProvidersTag`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Marker identifying a DistinctProviders clause.

```cpp
using DistinctProvidersTag = void;
```

### `Count`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Number of Requirements joined by this clause.

```cpp
static constexpr std::size_t Count = sizeof...(TRequirements);
```

### `Requirements`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Requirements represented by this clause.

```cpp
using Requirements = Detail::RequirementList<TRequirements...>;
```

### `TClause`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Extracts DistinctProviders metadata.

- **Template parameter `TClause`:** DistinctProviders clause being inspected.

```cpp
template<class TClause>
        struct DistinctProvidersTraits<
            TClause,
```

### `IsValid`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether the inspected Type is a DistinctProviders clause.

```cpp
static constexpr bool IsValid = true;
```

### `TRequirement`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Declares that the owning provider must initialize after the uniquely selected provider satisfying one Requirement.

- **Template parameter `TRequirement`:** Requirement identifying the lifecycle predecessor.

```cpp
template<class TRequirement>
    struct InitializesAfter
```

### `LifecycleClauseTag`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Marker identifying a lifecycle-ordering clause.

```cpp
using LifecycleClauseTag = void;
```

### `RequirementType`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Requirement identifying the initialization predecessor.

```cpp
using RequirementType = TRequirement;
```

### `IsInitializationClause`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates that this clause participates in initialization ordering.

```cpp
static constexpr bool IsInitializationClause = true;
```

### `IsShutdownClause`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates that this clause does not participate in shutdown ordering.

```cpp
static constexpr bool IsShutdownClause = false;
```

### `TRequirement`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Declares that the owning provider must shut down before the uniquely selected provider satisfying one Requirement.

- **Template parameter `TRequirement`:** Requirement identifying the shutdown successor.

```cpp
template<class TRequirement>
    struct ShutsDownBefore
```

### `LifecycleClauseTag`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Marker identifying a lifecycle-ordering clause.

```cpp
using LifecycleClauseTag = void;
```

### `RequirementType`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Requirement identifying the shutdown successor.

```cpp
using RequirementType = TRequirement;
```

### `IsInitializationClause`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates that this clause does not participate in initialization ordering.

```cpp
static constexpr bool IsInitializationClause = false;
```

### `IsShutdownClause`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates that this clause participates in shutdown ordering.

```cpp
static constexpr bool IsShutdownClause = true;
```

### `TClause`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Extracts lifecycle-clause metadata.

- **Template parameter `TClause`:** Lifecycle clause being inspected.

```cpp
template<class TClause>
        struct LifecycleClauseTraits<
            TClause,
```

### `IsValid`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether the inspected Type is a lifecycle-ordering clause.

```cpp
static constexpr bool IsValid = true;
```

### `TClause`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether one Type is a valid Contract clause.

- **Template parameter `TClause`:** Type being classified.

```cpp
template<class TClause>
        inline constexpr bool IsContractClauseV =
            RequirementTraits<TClause>::IsValid ||
            SameProviderTraits<TClause>::IsValid ||
            DistinctProvidersTraits<TClause>::IsValid ||
            LifecycleClauseTraits<TClause>::IsValid;
```

### `TDomain`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether one consolidated Requirement is legal inside a provider Contract.

- **Template parameter `TDomain`:** Domain owning the provider Contract.
- **Template parameter `TRequirement`:** Requirement being validated.

```cpp
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
```

### `TDomain`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Validates one Contract clause against an owning provider Domain.

Unrelated relationship clauses are accepted here and validated by their dedicated specializations.

- **Template parameter `TDomain`:** Domain owning the provider Contract.
- **Template parameter `TClause`:** Contract clause being inspected.

```cpp
template<class TDomain, class TClause>
        struct ProviderClauseIsValid : std::true_type {};
```

### `TDomain`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Validates one direct Requirement against an owning provider Domain.

- **Template parameter `TDomain`:** Domain owning the provider Contract.
- **Template parameter `TCapability`:** Capability requested by the Requirement.
- **Template parameter `TScope`:** Scope attached to the Requirement.
- **Template parameter `TCardinality`:** Provider cardinality attached to the Requirement.
- **Template parameter `TConstraints`:** Qualification constraints attached to the Requirement.

```cpp
template<
            class TDomain,
```

### `TRequirementList`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether every Requirement in one RequirementList uses the same scope and Capability Domain.

- **Template parameter `TRequirementList`:** Requirement list being inspected.

```cpp
template<class TRequirementList>
        struct RequirementsShareScopeAndDomain;
```

### `RequirementsShareScopeAndDomain`

**Classification:** INTERNAL PROVIDER API · source access: `public`

An empty Requirement list vacuously shares scope and Domain.

```cpp
template<>
        struct RequirementsShareScopeAndDomain<
            RequirementList<>
        > : std::true_type {};
```

### `TRequirement`

**Classification:** INTERNAL PROVIDER API · source access: `public`

A single Requirement trivially shares scope and Domain with itself.

- **Template parameter `TRequirement`:** Only Requirement in the list.

```cpp
template<class TRequirement>
        struct RequirementsShareScopeAndDomain<
            RequirementList<TRequirement>
        > : std::true_type {};
```

### `TFirstRequirement`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Compares every remaining Requirement with the first Requirement.

- **Template parameter `TFirstRequirement`:** First Requirement controlling the expected scope and Domain.
- **Template parameter `TRestRequirements`:** Remaining Requirements.

```cpp
template<class TFirstRequirement, class... TRestRequirements>
        struct RequirementsShareScopeAndDomain<
            RequirementList<
                TFirstRequirement,
```

### `TDomain`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Validates nested SameProvider requirements against an owning provider Domain.

- **Template parameter `TDomain`:** Domain owning the provider Contract.
- **Template parameter `TRequirements`:** Nested Requirements.

```cpp
template<class TDomain, class... TRequirements>
        struct ProviderClauseIsValid<
            TDomain,
```

### `TDomain`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Validates nested DistinctProviders requirements against an owning provider Domain.

- **Template parameter `TDomain`:** Domain owning the provider Contract.
- **Template parameter `TRequirements`:** Nested Requirements.

```cpp
template<class TDomain, class... TRequirements>
        struct ProviderClauseIsValid<
            TDomain,
```

### `TDomain`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Validates one lifecycle Requirement against an owning provider Domain.

- **Template parameter `TDomain`:** Domain owning the provider Contract.
- **Template parameter `TClause`:** Lifecycle clause being inspected.

```cpp
template<class TDomain, class TClause>
        struct ProviderLifecycleClauseIsValid : std::false_type {};
```

### `TDomain`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Validates InitializesAfter against an owning provider Domain.

- **Template parameter `TDomain`:** Domain owning the provider Contract.
- **Template parameter `TRequirement`:** Lifecycle predecessor Requirement.

```cpp
template<class TDomain, class TRequirement>
        struct ProviderLifecycleClauseIsValid<
            TDomain,
```

### `TDomain`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Validates ShutsDownBefore against an owning provider Domain.

- **Template parameter `TDomain`:** Domain owning the provider Contract.
- **Template parameter `TRequirement`:** Lifecycle successor Requirement.

```cpp
template<class TDomain, class TRequirement>
        struct ProviderLifecycleClauseIsValid<
            TDomain,
```

### `TDomain`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Routes lifecycle Contract clauses through their dedicated validation.

- **Template parameter `TDomain`:** Domain owning the provider Contract.
- **Template parameter `TClause`:** Contract clause being inspected.

```cpp
template<class TDomain, class TClause>
        inline constexpr bool ProviderClauseIsValidResolvedV =
            LifecycleClauseTraits<TClause>::IsValid
                ? ProviderLifecycleClauseIsValid<TDomain, TClause>::value
                : ProviderClauseIsValid<TDomain, TClause>::value;
```

### `TClause`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether one Contract clause is a direct Requirement with the supplied scope.

- **Template parameter `TClause`:** Contract clause being inspected.
- **Template parameter `TScope`:** Requirement scope being matched.

```cpp
template<class TClause, RequirementScope TScope>
        struct DirectRequirementMatchesScope : std::false_type {};
```

### `TScope`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Matches one direct Requirement against the supplied scope.

- **Template parameter `TCapability`:** Capability requested by the Requirement.
- **Template parameter `TRequirementScope`:** Scope attached to the Requirement.
- **Template parameter `TCardinality`:** Provider cardinality attached to the Requirement.
- **Template parameter `TConstraints`:** Qualification constraints attached to the Requirement.
- **Template parameter `TScope`:** Requirement scope being matched.

```cpp
template<
            RequirementScope TScope,
```

### `TScope`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Collects direct Requirements in one Contract matching the supplied scope.

- **Template parameter `TScope`:** Requirement scope to collect.
- **Template parameter `TAccumulatedRequirements`:** RequirementList accumulated so far.
- **Template parameter `TClauses`:** Remaining Contract clauses.

```cpp
template<
            RequirementScope TScope,
```

### `TScope`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Completes scoped Requirement collection.

- **Template parameter `TScope`:** Requirement scope being collected.
- **Template parameter `TRequirements`:** Accumulated Requirements.

```cpp
template<
            RequirementScope TScope,
```

### `Type`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Direct Requirements matching the requested scope.

```cpp
using Type = RequirementList<TRequirements...>;
```

### `TScope`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Appends one direct Requirement to an accumulated RequirementList when its scope matches.

- **Template parameter `TScope`:** Requirement scope being collected.
- **Template parameter `TClause`:** Current Contract clause.
- **Template parameter `TAccumulatedRequirements`:** RequirementList accumulated so far.

```cpp
template<
            RequirementScope TScope,
```

### `TScope`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Conditionally appends one clause to a concrete Requirement pack.

- **Template parameter `TScope`:** Requirement scope being collected.
- **Template parameter `TClause`:** Current Contract clause.
- **Template parameter `TRequirements`:** Accumulated Requirements.

```cpp
template<
            RequirementScope TScope,
```

### `Type`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Updated Requirement list.

```cpp
using Type = std::conditional_t<
                DirectRequirementMatchesScope<
                    TClause,
```

### `TScope`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Inspects one Contract clause and continues scoped Requirement collection.

- **Template parameter `TScope`:** Requirement scope being collected.
- **Template parameter `TAccumulatedRequirements`:** RequirementList accumulated so far.
- **Template parameter `TFirstClause`:** Current Contract clause.
- **Template parameter `TRestClauses`:** Remaining Contract clauses.

```cpp
template<
            RequirementScope TScope,
```

### `NextRequirements`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Next Requirement list after conditionally adding the current direct Requirement.

```cpp
using NextRequirements = typename AppendRequirementForScope<
                TScope,
```

### `Type`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Final scoped Requirement list.

```cpp
using Type = typename CollectRequirementsByScope<
                TScope,
```

### `TContract`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Default Contract metadata for unrelated Types.

- **Template parameter `TContract`:** Type being inspected.
- **Template parameter `TEnable`:** SFINAE helper used when Contract metadata is present.

```cpp
template<class TContract, class TEnable = void>
        struct ContractTraits
```

### `IsValid`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether the inspected Type is a Contract.

```cpp
static constexpr bool IsValid = false;
```

### `Contract`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Groups reusable compile-time architectural requirements for a provider or standalone consumer.

A Contract contains no runtime state. Provider Contracts may use SameDomain and ExternalDomain
Requirements. Standalone consumer Contracts may additionally use AnyDomain Requirements.

- **Template parameter `TClauses`:** Requirement and relationship clauses forming this Contract.

```cpp
template<class... TClauses>
    struct Contract
```

### `ContractTag`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Marker identifying this consolidated Contract declaration.

```cpp
using ContractTag = void;
```

### `Count`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Number of clauses represented by this Contract.

```cpp
static constexpr std::size_t Count = sizeof...(TClauses);
```

### `Clauses`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Complete compile-time Contract clause list.

```cpp
using Clauses = Detail::ContractClauseList<TClauses...>;
```

### `TDomain`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether this Contract is legal for a provider owned by the supplied Domain.

- **Template parameter `TDomain`:** Provider Domain against which scope relationships are validated.

```cpp
template<class TDomain>
        static constexpr bool IsProviderContractFor =
            IsDomainV<TDomain> &&
            (Detail::ProviderClauseIsValidResolvedV<TDomain, TClauses> && ...);
```

### `RequirementsForScope`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Returns direct Requirements in this Contract having the requested scope.

Relationship/lifecycle clauses remain represented in Clauses and are validated separately.

- **Template parameter `TScope`:** Requirement scope to collect.

```cpp
template<RequirementScope TScope>
        using RequirementsForScope = typename Detail::CollectRequirementsByScope<
            TScope,
```

### `TContract`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Extracts Contract metadata.

- **Template parameter `TContract`:** Contract Type being inspected.

```cpp
template<class TContract>
        struct ContractTraits<
            TContract,
```

### `IsValid`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Indicates whether the inspected Type is a Contract.

```cpp
static constexpr bool IsValid = true;
```

