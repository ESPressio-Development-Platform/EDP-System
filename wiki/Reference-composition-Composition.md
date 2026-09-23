# src/composition/Composition.hpp

**Primary classification:** PUBLIC COMPOSITION API

**Source baseline:** `9964ab2820cc5d290de69674000e011faaa684d9`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/9964ab2820cc5d290de69674000e011faaa684d9/src/composition/Composition.hpp)

## Direct includes

- `cstddef`
- `tuple`
- `type_traits`
- `utility`
- `Provider.hpp`
- `Selection.hpp`

## Documented declarations

### `FirstProviderType`

**Classification:** PUBLIC COMPOSITION API

Resolves the first Type in one compile-time provider pack.

- **Template parameter `TProviders`:** Provider Types represented by the pack.

```cpp
template<class... TProviders>
        struct FirstProviderType
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Missing-provider sentinel used for an empty pack.

```cpp
using Type = void;
```

### `TFirstProvider`

**Classification:** PUBLIC COMPOSITION API

Resolves the first Type in one non-empty provider pack.

- **Template parameter `TFirstProvider`:** First provider Type.
- **Template parameter `TRestProviders`:** Remaining provider Types.

```cpp
template<class TFirstProvider, class... TRestProviders>
        struct FirstProviderType<TFirstProvider, TRestProviders...>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

First provider Type.

```cpp
using Type = TFirstProvider;
```

### `TNeedle`

**Classification:** PUBLIC COMPOSITION API

Resolves one provider Type's zero-based index in a compile-time provider pack.

- **Template parameter `TNeedle`:** Provider Type whose index is requested.
- **Template parameter `TProviders`:** Provider Types being searched.

```cpp
template<class TNeedle, class... TProviders>
        struct ProviderTypeIndex;
```

### `TNeedle`

**Classification:** PUBLIC COMPOSITION API

Reports a missing provider Type.

- **Template parameter `TNeedle`:** Provider Type being searched.

```cpp
template<class TNeedle>
        struct ProviderTypeIndex<TNeedle>
```

### `Value`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Missing-provider sentinel index.

```cpp
static constexpr std::size_t Value =
                static_cast<std::size_t>(-1);
```

### `TNeedle`

**Classification:** PUBLIC COMPOSITION API

Resolves one provider Type recursively.

- **Template parameter `TNeedle`:** Provider Type being searched.
- **Template parameter `TFirstProvider`:** Current provider Type.
- **Template parameter `TRestProviders`:** Remaining provider Types.

```cpp
template<class TNeedle, class TFirstProvider, class... TRestProviders>
        struct ProviderTypeIndex<
            TNeedle,
```

### `RemainingIndex`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Index reported by the remaining provider pack.

```cpp
static constexpr std::size_t RemainingIndex =
                    ProviderTypeIndex<
                        TNeedle,
```

### `Value`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Zero-based provider index, or the missing-provider sentinel.

```cpp
static constexpr std::size_t Value =
                    std::is_same_v<
                        TNeedle,
```

### `LastProviderType`

**Classification:** PUBLIC COMPOSITION API

Resolves the last Type in one compile-time provider pack.

- **Template parameter `TProviders`:** Provider Types represented by the pack.

```cpp
template<class... TProviders>
        struct LastProviderType
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Missing-provider sentinel used for an empty pack.

```cpp
using Type = void;
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API

Resolves the last Type in one single-provider pack.

- **Template parameter `TProvider`:** Only provider Type.

```cpp
template<class TProvider>
        struct LastProviderType<TProvider>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Last provider Type.

```cpp
using Type = TProvider;
```

### `TFirstProvider`

**Classification:** PUBLIC COMPOSITION API

Resolves the last Type in one multi-provider pack.

- **Template parameter `TFirstProvider`:** First provider Type.
- **Template parameter `TRestProviders`:** Remaining provider Types.

```cpp
template<class TFirstProvider, class... TRestProviders>
        struct LastProviderType<TFirstProvider, TRestProviders...> : LastProviderType<TRestProviders...> {};
```

### `ProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Stores a deterministic compile-time list of provider Types returned by Composition queries.

- **Template parameter `TProviders`:** Provider Types represented by this list in declaration order.

```cpp
template<class... TProviders>
    struct ProviderList
```

### `Count`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Number of provider Types contained in this list.

```cpp
static constexpr std::size_t Count = sizeof...(TProviders);
```

### `IsEmpty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether this provider list is empty.

```cpp
static constexpr bool IsEmpty = Count == 0U;
```

### `Front`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

First provider Type, or void when this list is empty.

```cpp
using Front = typename Detail::FirstProviderType<TProviders...>::Type;
```

### `Back`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Last provider Type, or void when this list is empty.

```cpp
using Back = typename Detail::LastProviderType<TProviders...>::Type;
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the specified provider Type is contained in this list.

- **Template parameter `TProvider`:** Provider Type being queried.

```cpp
template<class TProvider>
        static constexpr bool Contains = (std::is_same_v<TProvider, TProviders> || ...);
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns the zero-based index of one provider Type, or Count when it is absent.

- **Template parameter `TProvider`:** Provider Type whose index is requested.

```cpp
template<class TProvider>
        static constexpr std::size_t IndexOf = []() constexpr
```

### `ContainsAll`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether every supplied provider Type is contained in this list.

- **Template parameter `TCandidates`:** Provider Types whose complete presence is required.

```cpp
template<class... TCandidates>
        static constexpr bool ContainsAll = (Contains<TCandidates> && ...);
```

### `ContainsAny`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether at least one supplied provider Type is contained in this list.

- **Template parameter `TCandidates`:** Provider Types of which any presence is sufficient.

```cpp
template<class... TCandidates>
        static constexpr bool ContainsAny = (Contains<TCandidates> || ...);
```

### `ResolveAt`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Resolves one provider Type by its zero-based list index.

- **Template parameter `TIndex`:** Zero-based provider index.

```cpp
template<std::size_t TIndex>
        struct ResolveAt
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Provider Type at the requested index.

```cpp
using Type = std::tuple_element_t<
                TIndex,
```

### `At`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Provider Type at the supplied zero-based index.

- **Template parameter `TIndex`:** Zero-based provider index.

```cpp
template<std::size_t TIndex>
        using At = typename ResolveAt<TIndex>::Type;
```

### `TCallable`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Invokes one templated callable once for every represented provider Type in declaration order.

The callable must support `operator()<TProvider>()` for every provider Type.
This operation retains no runtime state and is usable during constant evaluation.

- **Template parameter `TCallable`:** Templated callable Type.
- **Parameter `callable`:** Callable receiving each provider Type as a template argument.

```cpp
template<class TCallable>
        static constexpr void ForEachType(
            TCallable&& callable
        )
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Prepends one provider Type to a ProviderList.

- **Template parameter `TProvider`:** Provider Type to prepend.
- **Template parameter `TProviderList`:** Existing provider list.

```cpp
template<class TProvider, class TProviderList>
        struct PrependProviderList;
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Prepends one provider Type while preserving the existing list order.

- **Template parameter `TProvider`:** Provider Type to prepend.
- **Template parameter `TProviders`:** Existing provider Types.

```cpp
template<class TProvider, class... TProviders>
        struct PrependProviderList<
            TProvider,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Resulting provider list.

```cpp
using Type = ProviderList<
                TProvider,
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Appends one provider Type to a ProviderList.

- **Template parameter `TProviderList`:** Existing provider list.
- **Template parameter `TProvider`:** Provider Type to append.

```cpp
template<class TProviderList, class TProvider>
        struct AppendProviderList;
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Appends one provider Type while preserving the existing list order.

- **Template parameter `TProviders`:** Existing provider Types.
- **Template parameter `TProvider`:** Provider Type to append.

```cpp
template<class... TProviders, class TProvider>
        struct AppendProviderList<
            ProviderList<TProviders...>,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Resulting provider list.

```cpp
using Type = ProviderList<
                TProviders...,
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Appends one provider Type only when it is not already present in a ProviderList.

- **Template parameter `TProviderList`:** Existing provider list.
- **Template parameter `TProvider`:** Provider Type conditionally appended.

```cpp
template<class TProviderList, class TProvider>
        struct AppendUniqueProviderList;
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Conditionally appends one provider Type while preserving declaration order.

- **Template parameter `TProviders`:** Existing provider Types.
- **Template parameter `TProvider`:** Provider Type conditionally appended.

```cpp
template<class... TProviders, class TProvider>
        struct AppendUniqueProviderList<
            ProviderList<TProviders...>,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Provider list with the candidate appended only when previously absent.

```cpp
using Type = std::conditional_t<
                ProviderList<TProviders...>::template Contains<TProvider>,
```

### `TAccumulatedProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Adds every provider from one source list to one accumulated union.

- **Template parameter `TAccumulatedProviders`:** Providers already represented by the union.
- **Template parameter `TRemainingProviders`:** Source providers still requiring inspection.

```cpp
template<class TAccumulatedProviders, class TRemainingProviders>
        struct UnionProviderLists;
```

### `TAccumulatedProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Completes provider-list union when the source list is exhausted.

- **Template parameter `TAccumulatedProviders`:** Complete union result.

```cpp
template<class TAccumulatedProviders>
        struct UnionProviderLists<
            TAccumulatedProviders,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Complete union result.

```cpp
using Type = TAccumulatedProviders;
```

### `TAccumulatedProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Conditionally appends one source provider and continues union construction.

- **Template parameter `TAccumulatedProviders`:** Providers already represented by the union.
- **Template parameter `TFirstProvider`:** Current source provider Type.
- **Template parameter `TRestProviders`:** Remaining source provider Types.

```cpp
template<
            class TAccumulatedProviders,
```

### `NextAccumulated`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Union state after conditionally appending the current provider.

```cpp
using NextAccumulated = typename AppendUniqueProviderList<
                    TAccumulatedProviders,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Complete declaration-order-preserving union result.

```cpp
using Type = typename UnionProviderLists<
                    NextAccumulated,
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Filters one ProviderList using one compile-time unary predicate class template.

The predicate must expose a Boolean `value` member for each provider Type.

- **Template parameter `TProviderList`:** Providers being filtered.
- **Template parameter `TPredicate`:** Unary predicate class template.

```cpp
template<
            class TProviderList,
```

### `TPredicate`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Completes filtering when no providers remain.

- **Template parameter `TPredicate`:** Unary predicate class template.

```cpp
template<template<class> class TPredicate>
        struct FilterProviderList<
            ProviderList<>,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Empty filtering result.

```cpp
using Type = ProviderList<>;
```

### `TPredicate`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Filters one provider and continues recursively.

- **Template parameter `TPredicate`:** Unary predicate class template.
- **Template parameter `TFirstProvider`:** Current provider Type.
- **Template parameter `TRestProviders`:** Remaining provider Types.

```cpp
template<
            template<class> class TPredicate,
```

### `Remaining`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Filtering result for the remaining provider Types.

```cpp
using Remaining = typename FilterProviderList<
                    ProviderList<TRestProviders...>,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Ordered filtering result.

```cpp
using Type = std::conditional_t<
                    TPredicate<TFirstProvider>::value,
```

### `TLeftProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Concatenates two ProviderLists without changing either list's declaration order.

- **Template parameter `TLeftProviders`:** Left provider list.
- **Template parameter `TRightProviders`:** Right provider list.

```cpp
template<class TLeftProviders, class TRightProviders>
        struct ConcatProviderLists;
```

### `ConcatProviderLists`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Concatenates two concrete provider packs.

- **Template parameter `TLeftProviders`:** Provider Types from the left list.
- **Template parameter `TRightProviders`:** Provider Types from the right list.

```cpp
template<class... TLeftProviders, class... TRightProviders>
        struct ConcatProviderLists<
            ProviderList<TLeftProviders...>,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Concatenated provider list.

```cpp
using Type = ProviderList<
                TLeftProviders...,
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Removes every occurrence of one provider Type from a ProviderList.

- **Template parameter `TProvider`:** Provider Type to remove.
- **Template parameter `TProviderList`:** Provider list being filtered.

```cpp
template<class TProvider, class TProviderList>
        struct RemoveProviderFromList;
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Completes removal from an empty ProviderList.

- **Template parameter `TProvider`:** Provider Type being removed.

```cpp
template<class TProvider>
        struct RemoveProviderFromList<
            TProvider,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Empty removal result.

```cpp
using Type = ProviderList<>;
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Removes one provider Type recursively while preserving all remaining order.

- **Template parameter `TProvider`:** Provider Type being removed.
- **Template parameter `TFirstProvider`:** Current provider Type.
- **Template parameter `TRestProviders`:** Remaining provider Types.

```cpp
template<
            class TProvider,
```

### `Remaining`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Removal result for the remaining provider Types.

```cpp
using Remaining = typename RemoveProviderFromList<
                    TProvider,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Removal result preserving declaration order.

```cpp
using Type = std::conditional_t<
                    std::is_same_v<
                        TProvider,
```

### `TLeftProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Computes the declaration-order-preserving intersection of two ProviderLists.

- **Template parameter `TLeftProviders`:** Left provider list controlling result order.
- **Template parameter `TRightProviders`:** Right provider list used for membership checks.

```cpp
template<class TLeftProviders, class TRightProviders>
        struct IntersectProviderLists;
```

### `TRightProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Completes intersection when the left list is empty.

- **Template parameter `TRightProviders`:** Right provider list.

```cpp
template<class TRightProviders>
        struct IntersectProviderLists<
            ProviderList<>,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Empty intersection result.

```cpp
using Type = ProviderList<>;
```

### `TRightProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Computes one ordered intersection step.

- **Template parameter `TFirstProvider`:** Current left provider Type.
- **Template parameter `TRestProviders`:** Remaining left provider Types.
- **Template parameter `TRightProviders`:** Right provider list.

```cpp
template<
            class TRightProviders,
```

### `Remaining`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Intersection result for the remaining left provider Types.

```cpp
using Remaining = typename IntersectProviderLists<
                    ProviderList<TRestProviders...>,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Ordered intersection result.

```cpp
using Type = std::conditional_t<
                    TRightProviders::template Contains<TFirstProvider>,
```

### `TLeftProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Computes the declaration-order-preserving difference of two ProviderLists.

- **Template parameter `TLeftProviders`:** Left provider list controlling result order.
- **Template parameter `TRightProviders`:** Provider Types excluded from the result.

```cpp
template<class TLeftProviders, class TRightProviders>
        struct DifferenceProviderLists;
```

### `TRightProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Completes difference when the left list is empty.

- **Template parameter `TRightProviders`:** Right provider list.

```cpp
template<class TRightProviders>
        struct DifferenceProviderLists<
            ProviderList<>,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Empty difference result.

```cpp
using Type = ProviderList<>;
```

### `TRightProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Computes one ordered difference step.

- **Template parameter `TFirstProvider`:** Current left provider Type.
- **Template parameter `TRestProviders`:** Remaining left provider Types.
- **Template parameter `TRightProviders`:** Provider Types excluded from the result.

```cpp
template<
            class TRightProviders,
```

### `Remaining`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Difference result for the remaining left provider Types.

```cpp
using Remaining = typename DifferenceProviderLists<
                    ProviderList<TRestProviders...>,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Ordered difference result.

```cpp
using Type = std::conditional_t<
                    TRightProviders::template Contains<TFirstProvider>,
```

### `TNeedle`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether a type pack contains the specified type.

```cpp
template<class TNeedle, class... THaystack>
        inline constexpr bool ContainsTypeV = (std::is_same_v<TNeedle, THaystack> || ...);
```

### `TDomain`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Default provider metadata for Types that are not provider declarations.

```cpp
template<class TDomain, class TProvider, class = void>
        struct IsProviderDeclarationForDomain : std::false_type {};
```

### `TDomain`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Validates a concrete provider Type against a composition Domain.

```cpp
template<class TDomain, class TProvider>
        struct IsProviderDeclarationForDomain<
            TDomain,
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether one provider supplies the requested capability.

```cpp
template<class TProvider, class TCapability>
        inline constexpr bool ProviderOffersCapabilityV = TProvider::CompositionOffers::template Contains<TCapability>;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Counts providers that supply the requested capability.

```cpp
template<class TCapability, class... TProviders>
        inline constexpr std::size_t ProviderCountV =
            (std::size_t{0U} + ... + (ProviderOffersCapabilityV<TProviders, TCapability> ? std::size_t{1U} : std::size_t{0U}));
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects the first provider that supplies the requested capability.

```cpp
template<class TCapability, class... TProviders>
        struct FirstProvider;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Represents an unsuccessful provider lookup.

```cpp
template<class TCapability>
        struct FirstProvider<TCapability>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Type returned when no matching provider exists.

```cpp
using Type = void;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Continues a provider lookup until a provider supplying the requested capability is found.

```cpp
template<class TCapability, class TFirstProvider, class... TRestProviders>
        struct FirstProvider<TCapability, TFirstProvider, TRestProviders...>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

First provider supplying the requested capability, or the result of searching the remaining providers.

```cpp
using Type = std::conditional_t<
                ProviderOffersCapabilityV<TFirstProvider, TCapability>,
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Filters a provider pack to providers that supply the requested capability.

```cpp
template<class TCapability, class TAccumulatedProviders, class... TProviders>
        struct FilterProviders;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Completes provider filtering when no providers remain to inspect.

```cpp
template<class TCapability, class... TAccumulatedProviders>
        struct FilterProviders<TCapability, ProviderList<TAccumulatedProviders...>>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Provider list accumulated during filtering.

```cpp
using Type = ProviderList<TAccumulatedProviders...>;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Adds matching providers to the accumulated provider list and continues filtering.

```cpp
template<class TCapability, class... TAccumulatedProviders, class TFirstProvider, class... TRestProviders>
        struct FilterProviders<TCapability, ProviderList<TAccumulatedProviders...>, TFirstProvider, TRestProviders...>
```

### `NextProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Provider list to use for the next filtering step.

```cpp
using NextProviders = std::conditional_t<
                ProviderOffersCapabilityV<TFirstProvider, TCapability>,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Final provider list returned after the remaining providers are inspected.

```cpp
using Type = typename FilterProviders<TCapability, NextProviders, TRestProviders...>::Type;
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Determines whether one provider satisfies one capability requirement.

```cpp
template<class TProvider, class TRequirement, bool TOffersDeclarationCapability = ProviderOffersCapabilityV<TProvider, typename TRequirement::CapabilityType>>
        struct ProviderSatisfiesRequirement : std::false_type {};
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates a capability requirement against a provider that supplies the required capability.

```cpp
template<class TProvider, class TRequirement>
        struct ProviderSatisfiesRequirement<TProvider, TRequirement, true> : std::bool_constant<
            TRequirement::template OfferSatisfied<
                typename TProvider::CompositionOffers::template OfferFor<typename TRequirement::CapabilityType>
            >
        > {};
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Counts providers that satisfy one capability requirement.

```cpp
template<class TRequirement, class... TProviders>
        inline constexpr std::size_t SatisfyingProviderCountV =
            (std::size_t{0U} + ... + (ProviderSatisfiesRequirement<TProviders, TRequirement>::value ? std::size_t{1U} : std::size_t{0U}));
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects the first provider satisfying one complete capability requirement.

```cpp
template<class TRequirement, class... TProviders>
        struct FirstSatisfyingProvider;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Represents an unsuccessful satisfying-provider lookup.

```cpp
template<class TRequirement>
        struct FirstSatisfyingProvider<TRequirement>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Type returned when no provider satisfies the requested Requirement.

```cpp
using Type = void;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Continues a satisfying-provider lookup until one provider satisfies the requested Requirement.

```cpp
template<class TRequirement, class TFirstProvider, class... TRestProviders>
        struct FirstSatisfyingProvider<TRequirement, TFirstProvider, TRestProviders...>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

First provider satisfying the Requirement, or the result of searching the remaining providers.

```cpp
using Type = std::conditional_t<
                ProviderSatisfiesRequirement<TFirstProvider, TRequirement>::value,
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Filters a provider pack to providers satisfying one complete capability requirement.

```cpp
template<class TRequirement, class TAccumulatedProviders, class... TProviders>
        struct FilterSatisfyingProviders;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Completes satisfying-provider filtering when no providers remain to inspect.

```cpp
template<class TRequirement, class... TAccumulatedProviders>
        struct FilterSatisfyingProviders<TRequirement, ProviderList<TAccumulatedProviders...>>
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Provider list accumulated during filtering.

```cpp
using Type = ProviderList<TAccumulatedProviders...>;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Adds providers satisfying the Requirement to the accumulated list and continues filtering.

```cpp
template<class TRequirement, class... TAccumulatedProviders, class TFirstProvider, class... TRestProviders>
        struct FilterSatisfyingProviders<TRequirement, ProviderList<TAccumulatedProviders...>, TFirstProvider, TRestProviders...>
```

### `NextProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Provider list to use for the next filtering step.

```cpp
using NextProviders = std::conditional_t<
                ProviderSatisfiesRequirement<TFirstProvider, TRequirement>::value,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Final provider list returned after the remaining providers are inspected.

```cpp
using Type = typename FilterSatisfyingProviders<TRequirement, NextProviders, TRestProviders...>::Type;
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Forward declaration for provider-versus-RequirementList joint matching.

- **Template parameter `TProvider`:** Provider Type being inspected.
- **Template parameter `TRequirementList`:** Requirements which must all match.

```cpp
template<class TProvider, class TRequirementList>
        struct ProviderSatisfiesRequirementList;
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Forward declaration for best-Property filtering used by generic selection policies.

- **Template parameter `TProperty`:** Property used for ranking.
- **Template parameter `TMinimize`:** Whether lower values are preferred.
- **Template parameter `TProviderList`:** Candidate providers.

```cpp
template<
            class TProperty,
```

### `TRequirementList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Filters one ProviderList to providers satisfying every Requirement in one RequirementList.

- **Template parameter `TRequirementList`:** Requirements which must all match the same provider.
- **Template parameter `TAccumulatedProviders`:** Providers selected so far.
- **Template parameter `TRemainingProviders`:** Providers still requiring inspection.

```cpp
template<
            class TRequirementList,
```

### `TRequirementList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Completes joint-provider filtering when no providers remain.

- **Template parameter `TRequirementList`:** Requirements being jointly evaluated.
- **Template parameter `TAccumulatedProviders`:** Providers selected so far.

```cpp
template<class TRequirementList, class TAccumulatedProviders>
        struct FilterJointlySatisfyingProviders<
            TRequirementList,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Complete joint-match ProviderList.

```cpp
using Type = TAccumulatedProviders;
```

### `TRequirementList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one provider against the complete RequirementList and continues filtering.

- **Template parameter `TRequirementList`:** Requirements which must all match one provider.
- **Template parameter `TAccumulatedProviders`:** Providers selected so far.
- **Template parameter `TFirstProvider`:** Current provider Type.
- **Template parameter `TRestProviders`:** Remaining provider Types.

```cpp
template<
            class TRequirementList,
```

### `NextAccumulated`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Accumulated result after conditionally adding the current provider.

```cpp
using NextAccumulated = std::conditional_t<
                    ProviderSatisfiesRequirementList<
                        TFirstProvider,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Complete joint-match ProviderList.

```cpp
using Type = typename FilterJointlySatisfyingProviders<
                    TRequirementList,
```

### `TSelectionPolicy`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Applies one explicit selection policy to an already-qualified ProviderList.

Unlike SelectionResult, this form is not tied to one Capability Requirement and therefore
can select providers jointly satisfying several Requirements across several Capabilities.

- **Template parameter `TSelectionPolicy`:** Explicit provider selection policy.
- **Template parameter `TProviderList`:** Already-qualified provider candidates.

```cpp
template<class TSelectionPolicy, class TProviderList>
        struct ProviderListSelectionResult;
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects every provider in an already-qualified ProviderList.

- **Template parameter `TProviderList`:** Qualified provider candidates.

```cpp
template<class TProviderList>
        struct ProviderListSelectionResult<
            SelectAll,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Complete candidate ProviderList.

```cpp
using Type = TProviderList;
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects exactly one provider from an already-qualified ProviderList.

- **Template parameter `TProviderList`:** Qualified provider candidates.

```cpp
template<class TProviderList>
        struct ProviderListSelectionResult<
            SelectUnique,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Unique qualified provider Type.

```cpp
using Type = typename TProviderList::Front;
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects zero or one provider from an already-qualified ProviderList.

- **Template parameter `TProviderList`:** Qualified provider candidates.

```cpp
template<class TProviderList>
        struct ProviderListSelectionResult<
            SelectOptionalUnique,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Unique qualified provider Type, or void when no provider matches.

```cpp
using Type = typename TProviderList::Front;
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects the unique provider advertising the minimum Property value.

- **Template parameter `TProperty`:** Property used for ranking.
- **Template parameter `TProviderList`:** Qualified provider candidates.

```cpp
template<class TProperty, class TProviderList>
        struct ProviderListSelectionResult<
            SelectMinimum<TProperty>,
```

### `BestProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Providers tied for the minimum Property value.

```cpp
using BestProviders = typename BestProviderList<
                TProperty,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Provider Type advertising the unique minimum value.

```cpp
using Type = typename BestProviders::Front;
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects the unique provider advertising the maximum Property value.

- **Template parameter `TProperty`:** Property used for ranking.
- **Template parameter `TProviderList`:** Qualified provider candidates.

```cpp
template<class TProperty, class TProviderList>
        struct ProviderListSelectionResult<
            SelectMaximum<TProperty>,
```

### `BestProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Providers tied for the maximum Property value.

```cpp
using BestProviders = typename BestProviderList<
                TProperty,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Provider Type advertising the unique maximum value.

```cpp
using Type = typename BestProviders::Front;
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects every provider tied for the minimum Property value.

- **Template parameter `TProperty`:** Property used for ranking.
- **Template parameter `TProviderList`:** Qualified provider candidates.

```cpp
template<class TProperty, class TProviderList>
        struct ProviderListSelectionResult<
            SelectAllMinimum<TProperty>,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Providers tied for the minimum Property value.

```cpp
using Type = typename BestProviderList<
                TProperty,
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects every provider tied for the maximum Property value.

- **Template parameter `TProperty`:** Property used for ranking.
- **Template parameter `TProviderList`:** Qualified provider candidates.

```cpp
template<class TProperty, class TProviderList>
        struct ProviderListSelectionResult<
            SelectAllMaximum<TProperty>,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Providers tied for the maximum Property value.

```cpp
using Type = typename BestProviderList<
                TProperty,
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one Requirement's accepted provider count.

- **Template parameter `TRequirement`:** Consolidated Requirement declaration.
- **Template parameter `TProviderCount`:** Number of providers satisfying its qualification.

```cpp
template<
            class TRequirement,
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether one provider advertises a requested Property.

- **Template parameter `TProvider`:** Provider Type being inspected.
- **Template parameter `TProperty`:** Property whose presence is required.
- **Template parameter `TOffersDeclarationCapability`:** Whether the provider supplies the Property's owning Capability.

```cpp
template<
            class TProvider,
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Inspects Property presence after confirming the provider supplies its owning Capability.

- **Template parameter `TProvider`:** Provider Type being inspected.
- **Template parameter `TProperty`:** Property whose presence is required.

```cpp
template<class TProvider, class TProperty>
        struct ProviderAdvertisesProperty<
            TProvider,
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns one provider's compile-time Property value.

- **Template parameter `TProvider`:** Provider Type being inspected.
- **Template parameter `TProperty`:** Property whose value is requested.

```cpp
template<class TProvider, class TProperty>
        inline constexpr auto ProviderPropertyValueV =
            TProvider::CompositionOffers::template PropertiesFor<
                typename TProperty::CapabilityType
            >::template Value<TProperty>;
```

### `TCandidate`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether one provider is tied for the best Property value in a candidate pack.

- **Template parameter `TCandidate`:** Candidate provider Type.
- **Template parameter `TProperty`:** Property used for ordering.
- **Template parameter `TMinimize`:** Whether lower values are preferred.
- **Template parameter `TProviders`:** Complete candidate provider pack.

```cpp
template<
            class TCandidate,
```

### `TCandidate`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether one candidate is tied for the best Property value in a ProviderList.

- **Template parameter `TCandidate`:** Candidate provider Type.
- **Template parameter `TProperty`:** Property used for ordering.
- **Template parameter `TMinimize`:** Whether lower values are preferred.
- **Template parameter `TProviderList`:** Complete candidate ProviderList.

```cpp
template<
            class TCandidate,
```

### `TCandidate`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Evaluates one candidate against a concrete provider pack.

- **Template parameter `TCandidate`:** Candidate provider Type.
- **Template parameter `TProperty`:** Property used for ordering.
- **Template parameter `TMinimize`:** Whether lower values are preferred.
- **Template parameter `TProviders`:** Complete candidate provider pack.

```cpp
template<
            class TCandidate,
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Filters a ProviderList to every provider tied for one best Property value.

- **Template parameter `TProperty`:** Property used for ordering.
- **Template parameter `TMinimize`:** Whether lower values are preferred.
- **Template parameter `TCandidates`:** Complete candidate ProviderList.
- **Template parameter `TRemaining`:** Remaining providers being inspected.

```cpp
template<
            class TProperty,
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Completes best-provider filtering.

- **Template parameter `TProperty`:** Property used for ordering.
- **Template parameter `TMinimize`:** Whether lower values are preferred.
- **Template parameter `TCandidates`:** Complete candidate ProviderList.

```cpp
template<
            class TProperty,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Empty terminal result.

```cpp
using Type = ProviderList<>;
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one best-provider candidate and continues filtering.

- **Template parameter `TProperty`:** Property used for ordering.
- **Template parameter `TMinimize`:** Whether lower values are preferred.
- **Template parameter `TCandidateProviders`:** Complete candidate provider pack.
- **Template parameter `TFirstProvider`:** Current provider Type.
- **Template parameter `TRestProviders`:** Remaining provider Types.

```cpp
template<
            class TProperty,
```

### `Remaining`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Best-provider result for the remaining candidates.

```cpp
using Remaining = typename FilterBestProviders<
                    TProperty,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Ordered best-provider result.

```cpp
using Type = std::conditional_t<
                    IsBestProviderInList<
                        TFirstProvider,
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Validates and resolves every provider tied for one best Property value.

- **Template parameter `TProperty`:** Property used for ordering.
- **Template parameter `TMinimize`:** Whether lower values are preferred.
- **Template parameter `TProviderList`:** Candidate providers.

```cpp
template<
            class TProperty,
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Resolves best providers from one concrete candidate pack.

- **Template parameter `TProperty`:** Property used for ordering.
- **Template parameter `TMinimize`:** Whether lower values are preferred.
- **Template parameter `TProviders`:** Candidate provider Types.

```cpp
template<
            class TProperty,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Providers tied for the best Property value in original declaration order.

```cpp
using Type = typename FilterBestProviders<
                TProperty,
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Resolves one explicit selection policy against a Requirement match list.

- **Template parameter `TRequirement`:** Requirement whose matches are being selected.
- **Template parameter `TSelectionPolicy`:** Explicit selection policy.
- **Template parameter `TProviderList`:** Providers satisfying the Requirement qualification.

```cpp
template<
            class TRequirement,
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects every satisfying provider.

- **Template parameter `TRequirement`:** Requirement whose matches are being selected.
- **Template parameter `TProviderList`:** Matching providers.

```cpp
template<class TRequirement, class TProviderList>
        struct SelectionResult<
            TRequirement,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Complete matching ProviderList.

```cpp
using Type = TProviderList;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects exactly one satisfying provider.

- **Template parameter `TRequirement`:** Requirement whose matches are being selected.
- **Template parameter `TProviderList`:** Matching providers.

```cpp
template<class TRequirement, class TProviderList>
        struct SelectionResult<
            TRequirement,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Unique satisfying provider Type.

```cpp
using Type = typename TProviderList::Front;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects zero or one satisfying provider.

- **Template parameter `TRequirement`:** Requirement whose matches are being selected.
- **Template parameter `TProviderList`:** Matching providers.

```cpp
template<class TRequirement, class TProviderList>
        struct SelectionResult<
            TRequirement,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Unique satisfying provider Type, or void when no provider matches.

```cpp
using Type = typename TProviderList::Front;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects the unique provider advertising the minimum Property value.

- **Template parameter `TRequirement`:** Requirement whose matches are being ranked.
- **Template parameter `TProperty`:** Property used for ranking.
- **Template parameter `TProviderList`:** Matching providers.

```cpp
template<
            class TRequirement,
```

### `BestProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Providers tied for the minimum Property value.

```cpp
using BestProviders = typename BestProviderList<
                TProperty,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Provider Type advertising the unique minimum value.

```cpp
using Type = typename BestProviders::Front;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects the unique provider advertising the maximum Property value.

- **Template parameter `TRequirement`:** Requirement whose matches are being ranked.
- **Template parameter `TProperty`:** Property used for ranking.
- **Template parameter `TProviderList`:** Matching providers.

```cpp
template<
            class TRequirement,
```

### `BestProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Providers tied for the maximum Property value.

```cpp
using BestProviders = typename BestProviderList<
                TProperty,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Provider Type advertising the unique maximum value.

```cpp
using Type = typename BestProviders::Front;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects every provider tied for the minimum Property value.

- **Template parameter `TRequirement`:** Requirement whose matches are being ranked.
- **Template parameter `TProperty`:** Property used for ranking.
- **Template parameter `TProviderList`:** Matching providers.

```cpp
template<
            class TRequirement,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Providers tied for the minimum Property value.

```cpp
using Type = typename BestProviderList<
                TProperty,
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects every provider tied for the maximum Property value.

- **Template parameter `TRequirement`:** Requirement whose matches are being ranked.
- **Template parameter `TProperty`:** Property used for ranking.
- **Template parameter `TProviderList`:** Matching providers.

```cpp
template<
            class TRequirement,
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Providers tied for the maximum Property value.

```cpp
using Type = typename BestProviderList<
                TProperty,
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether one provider jointly satisfies every Requirement in a RequirementList.

- **Template parameter `TProvider`:** Provider Type being inspected.
- **Template parameter `TRequirementList`:** Requirements which must all match the same provider.

```cpp
template<class TProvider, class TRequirementList>
        struct ProviderSatisfiesRequirementList;
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one concrete Requirement pack against one provider.

- **Template parameter `TProvider`:** Provider Type being inspected.
- **Template parameter `TRequirements`:** Requirements which must all match.

```cpp
template<class TProvider, class... TRequirements>
        struct ProviderSatisfiesRequirementList<
            TProvider,
```

### `TRequirementList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Counts providers jointly satisfying every Requirement in one RequirementList.

- **Template parameter `TRequirementList`:** Requirements being jointly evaluated.
- **Template parameter `TProviderList`:** Candidate provider Types.

```cpp
template<class TRequirementList, class TProviderList>
        struct JointlySatisfyingProviderCount;
```

### `TRequirementList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Counts jointly satisfying providers in one concrete ProviderList.

- **Template parameter `TRequirementList`:** Requirements being jointly evaluated.
- **Template parameter `TProviders`:** Candidate provider Types.

```cpp
template<class TRequirementList, class... TProviders>
        struct JointlySatisfyingProviderCount<
            TRequirementList,
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Counts providers satisfying one Requirement inside one ProviderList.

- **Template parameter `TRequirement`:** Requirement being evaluated.
- **Template parameter `TProviderList`:** Candidate provider list.

```cpp
template<class TRequirement, class TProviderList>
        struct SatisfyingProviderCountInList;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Counts satisfying providers in one concrete provider pack.

- **Template parameter `TRequirement`:** Requirement being evaluated.
- **Template parameter `TProviders`:** Candidate provider Types.

```cpp
template<class TRequirement, class... TProviders>
        struct SatisfyingProviderCountInList<
            TRequirement,
```

### `TRequirementList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether every Requirement in one RequirementList has the supplied scope.

- **Template parameter `TRequirementList`:** Requirement list being inspected.
- **Template parameter `TScope`:** Scope required from every Requirement.

```cpp
template<class TRequirementList, RequirementScope TScope>
        struct RequirementsHaveScope;
```

### `RequirementsHaveScope`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one Requirement pack for a common scope.

- **Template parameter `TScope`:** Scope required from every Requirement.
- **Template parameter `TRequirements`:** Requirements being inspected.

```cpp
template<RequirementScope TScope, class... TRequirements>
        struct RequirementsHaveScope<
            RequirementList<TRequirements...>,
```

### `TRequirements`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Forward declaration for exhaustive distinct-provider assignment.

- **Template parameter `TRequirements`:** Requirements still requiring distinct providers.
- **Template parameter `TAvailableProviders`:** Provider Types still available for assignment.

```cpp
template<class TRequirements, class TAvailableProviders>
        struct DistinctAssignmentExists;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Tries available providers for one Requirement and recursively assigns the remainder.

- **Template parameter `TRequirement`:** Current Requirement being assigned.
- **Template parameter `TRestRequirements`:** Remaining Requirements after the current one.
- **Template parameter `TAvailableProviders`:** Complete currently available provider list.
- **Template parameter `TCandidates`:** Remaining candidate provider Types to try.

```cpp
template<
            class TRequirement,
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

No candidate can satisfy the current Requirement.

- **Template parameter `TRequirement`:** Current Requirement being assigned.
- **Template parameter `TRestRequirements`:** Remaining Requirements.
- **Template parameter `TAvailableProviders`:** Complete currently available provider list.

```cpp
template<
            class TRequirement,
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Tries one candidate provider before recursively trying the remaining candidates.

- **Template parameter `TRequirement`:** Current Requirement being assigned.
- **Template parameter `TRestRequirements`:** Remaining Requirements.
- **Template parameter `TAvailableProviders`:** Complete currently available provider list.
- **Template parameter `TFirstProvider`:** Current candidate provider Type.
- **Template parameter `TRestProviders`:** Remaining candidate provider Types.

```cpp
template<
            class TRequirement,
```

### `TAvailableProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

An empty Requirement list always has a complete distinct assignment.

- **Template parameter `TAvailableProviders`:** Provider Types which remain unused.

```cpp
template<class TAvailableProviders>
        struct DistinctAssignmentExists<
            RequirementList<>,
```

### `TAvailableProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Assigns the first Requirement and recursively assigns every remaining Requirement.

- **Template parameter `TAvailableProviders`:** Provider Types available for distinct assignment.
- **Template parameter `TFirstRequirement`:** Current Requirement being assigned.
- **Template parameter `TRestRequirements`:** Remaining Requirements.

```cpp
template<
            class TAvailableProviders,
```

### `TClause`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one provider Contract clause inside a single-domain Composition.

Cross-domain clauses are deferred until Architecture validation.

- **Template parameter `TClause`:** Contract clause being evaluated.
- **Template parameter `TProviderList`:** Providers participating in the Composition.

```cpp
template<class TClause, class TProviderList>
        struct ContractClauseSatisfiedInComposition : std::true_type {};
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one direct consolidated Requirement inside a Composition.

- **Template parameter `TProviderList`:** Providers participating in the Composition.
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

Evaluates one SameProvider clause when it targets the owning Composition Domain.

- **Template parameter `TProviderList`:** Providers participating in the Composition.
- **Template parameter `TRequirements`:** Requirements which must be jointly satisfied.

```cpp
template<class TProviderList, class... TRequirements>
        struct ContractClauseSatisfiedInComposition<
            SameProvider<TRequirements...>,
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one DistinctProviders clause when it targets only the owning Composition Domain.

- **Template parameter `TProviderList`:** Providers participating in the Composition.
- **Template parameter `TRequirements`:** Requirements requiring distinct assignment.

```cpp
template<class TProviderList, class... TRequirements>
        struct ContractClauseSatisfiedInComposition<
            DistinctProviders<TRequirements...>,
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one initialization lifecycle target when it uses SameDomain scope.

- **Template parameter `TProviderList`:** Providers participating in the Composition.
- **Template parameter `TRequirement`:** Lifecycle target Requirement.

```cpp
template<class TProviderList, class TRequirement>
        struct ContractClauseSatisfiedInComposition<
            InitializesAfter<TRequirement>,
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates one shutdown lifecycle target when it uses SameDomain scope.

- **Template parameter `TProviderList`:** Providers participating in the Composition.
- **Template parameter `TRequirement`:** Lifecycle target Requirement.

```cpp
template<class TProviderList, class TRequirement>
        struct ContractClauseSatisfiedInComposition<
            ShutsDownBefore<TRequirement>,
```

### `TContract`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates every clause in one consolidated provider Contract against a Composition.

- **Template parameter `TContract`:** Consolidated Contract.
- **Template parameter `TProviderList`:** Providers participating in the Composition.

```cpp
template<class TContract, class TProviderList>
        struct ContractSatisfiedInComposition : std::true_type {};
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Evaluates every clause in one concrete Contract.

- **Template parameter `TProviderList`:** Providers participating in the Composition.
- **Template parameter `TClauses`:** Contract clauses being evaluated.

```cpp
template<class TProviderList, class... TClauses>
        struct ContractSatisfiedInComposition<
            Contract<TClauses...>,
```

### `TOffersDeclaration`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Determines whether all offers in a Offers declaration obey exclusive-capability rules.

```cpp
template<class TOffersDeclaration, class TDomain, class... TProviders>
        struct OffersAreConflictFree;
```

### `TDomain`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Checks every offered capability against the complete provider pack.

```cpp
template<class... TOffers, class TDomain, class... TProviders>
        struct OffersAreConflictFree<Offers<TOffers...>, TDomain, TProviders...> : std::bool_constant<
            ((
                !IsExclusiveCapabilityForV<TDomain, typename TOffers::CapabilityType> ||
                ProviderCountV<typename TOffers::CapabilityType, TProviders...> <= 1U
            ) && ...)
        > {};
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Compile-time diagnostics describing whether one provider satisfies one Requirement qualification.

- **Template parameter `TProvider`:** Provider Type being inspected.
- **Template parameter `TRequirement`:** Requirement being evaluated.

```cpp
template<class TProvider, class TRequirement>
    struct ProviderMatch
```

### `OffersCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the provider supplies the requested Capability.

```cpp
static constexpr bool OffersCapability =
            Detail::ProviderOffersCapabilityV<
                TProvider,
```

### `ConstraintsSatisfied`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether every qualification constraint is satisfied when the Capability is supplied.

```cpp
static constexpr bool ConstraintsSatisfied = []() constexpr
```

### `IsSatisfied`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the provider satisfies the complete Requirement qualification.

```cpp
static constexpr bool IsSatisfied =
            OffersCapability &&
            ConstraintsSatisfied;
```

### `TLeftProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Concatenates two ProviderLists while preserving each list's declaration order.

- **Template parameter `TLeftProviders`:** Left ProviderList.
- **Template parameter `TRightProviders`:** Right ProviderList.

```cpp
template<class TLeftProviders, class TRightProviders>
    using ProviderListConcat = typename Detail::ConcatProviderLists<
        TLeftProviders,
```

### `TLeftProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns the declaration-order-preserving union of two ProviderLists.

Providers already represented by the left list retain their positions; new providers from the
right list are appended in right-list declaration order.

- **Template parameter `TLeftProviders`:** Left ProviderList controlling initial order.
- **Template parameter `TRightProviders`:** Right ProviderList contributing previously absent providers.

```cpp
template<class TLeftProviders, class TRightProviders>
    using ProviderListUnion = typename Detail::UnionProviderLists<
        TLeftProviders,
```

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Filters one ProviderList through a compile-time unary predicate.

- **Template parameter `TProviderList`:** ProviderList being filtered.
- **Template parameter `TPredicate`:** Predicate class template exposing Boolean `value`.

```cpp
template<
        class TProviderList,
```

### `TLeftProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns the ordered intersection of two ProviderLists.

- **Template parameter `TLeftProviders`:** Left ProviderList controlling output order.
- **Template parameter `TRightProviders`:** Right ProviderList used for membership.

```cpp
template<class TLeftProviders, class TRightProviders>
    using ProviderListIntersection = typename Detail::IntersectProviderLists<
        TLeftProviders,
```

### `TLeftProviders`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns provider Types present in the left ProviderList but absent from the right.

- **Template parameter `TLeftProviders`:** Left ProviderList controlling output order.
- **Template parameter `TRightProviders`:** Provider Types excluded from the result.

```cpp
template<class TLeftProviders, class TRightProviders>
    using ProviderListDifference = typename Detail::DifferenceProviderLists<
        TLeftProviders,
```

### `TDomain`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Validates and exposes the compile-time architecture formed by a set of providers within one domain.

```cpp
template<class TDomain, class... TProviders>
    struct Composition
```

### `NoCapabilityConflicts`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether every ExclusiveCapability has no more than one provider.

```cpp
static constexpr bool NoCapabilityConflicts =
            (Detail::OffersAreConflictFree<typename TProviders::CompositionOffers, TDomain, TProviders...>::value && ...);
```

### `AllContractsSatisfied`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether every same-domain provider Contract clause is satisfied.

```cpp
static constexpr bool AllContractsSatisfied =
            (
                Detail::ContractSatisfiedInComposition<
                    typename TProviders::CompositionContract,
                    ProviderList<TProviders...>
                >::value &&
                ...
            );
```

### `CompositionDeclarationTag`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Marker used to identify Composition declarations during architecture-level inspection.

```cpp
using CompositionDeclarationTag = void;
```

### `CompositionDomain`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Domain represented by this composition.

```cpp
using CompositionDomain = TDomain;
```

### `ProviderTypes`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Complete compile-time list of providers contained by this Composition.

```cpp
using ProviderTypes = ProviderList<TProviders...>;
```

### `ProviderCount`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Number of providers contained in this composition.

```cpp
static constexpr std::size_t ProviderCount = sizeof...(TProviders);
```

### `IsValid`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates that compile-time composition validation completed successfully.

```cpp
static constexpr bool IsValid =
            NoCapabilityConflicts &&
            AllContractsSatisfied;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Validates one Requirement before provider matching is attempted.

```cpp
template<class TRequirement>
        struct ValidateRequirement
```

### `IsValid`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates that the Requirement is valid for this Composition.

```cpp
static constexpr bool IsValid = true;
```

### `TRequirement`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns every provider matching one Requirement qualification.

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

Indicates whether the matching provider population obeys one Requirement's cardinality.

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

Returns providers jointly satisfying every supplied Requirement.

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

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Resolves the property set advertised by one provider for one capability.

```cpp
template<class TProvider, class TCapability>
        struct ResolveProviderProperties
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Property set advertised by the provider for the requested capability.

```cpp
using Type = typename TProvider::CompositionOffers::template PropertiesFor<TCapability>;
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns the property set advertised by a specific provider for a capability.

```cpp
template<class TProvider, class TCapability>
        using PropertiesForProvider = typename ResolveProviderProperties<TProvider, TCapability>::Type;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns the Property set advertised by the unique provider for a capability.

```cpp
template<class TCapability>
        using PropertiesFor = PropertiesForProvider<
            Select<
                Requirement<
                    TCapability,
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Indicates whether the single provider for a capability advertises the specified property.

```cpp
template<class TCapability, class TProperty>
        static constexpr bool HasProperty = PropertiesFor<TCapability>::template Contains<TProperty>;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns the value advertised for a property by the single provider for a capability.

```cpp
template<class TCapability, class TProperty>
        static constexpr typename TProperty::ValueType PropertyValue = PropertiesFor<TCapability>::template Value<TProperty>;
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Resolves the Attribute set advertised by one provider for one capability.

```cpp
template<class TProvider, class TCapability>
        struct ResolveProviderAttributes
```

### `Type`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Attribute set advertised by the provider for the requested capability.

```cpp
using Type = typename TProvider::CompositionOffers::template AttributesFor<TCapability>;
```

### `TProvider`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns the Attribute set advertised by a specific provider for a capability.

```cpp
template<class TProvider, class TCapability>
        using AttributesForProvider = typename ResolveProviderAttributes<TProvider, TCapability>::Type;
```

### `TCapability`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Returns the Attribute set advertised by the unique provider for a capability.

```cpp
template<class TCapability>
        using AttributesFor = AttributesForProvider<
            Select<
                Requirement<
                    TCapability,
```

