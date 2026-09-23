#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include "Provider.hpp"
#include "Selection.hpp"

namespace ESPressio::System::CompositionFramework {

    namespace Detail {

        /// Resolves the first Type in one compile-time provider pack.
        ///
        /// @tparam TProviders Provider Types represented by the pack.
        template<class... TProviders>
        struct FirstProviderType {

            /// Missing-provider sentinel used for an empty pack.
            using Type = void;

        };


        /// Resolves the first Type in one non-empty provider pack.
        ///
        /// @tparam TFirstProvider First provider Type.
        /// @tparam TRestProviders Remaining provider Types.
        template<class TFirstProvider, class... TRestProviders>
        struct FirstProviderType<TFirstProvider, TRestProviders...> {

            /// First provider Type.
            using Type = TFirstProvider;

        };


        /// Resolves one provider Type's zero-based index in a compile-time provider pack.
        ///
        /// @tparam TNeedle Provider Type whose index is requested.
        /// @tparam TProviders Provider Types being searched.
        template<class TNeedle, class... TProviders>
        struct ProviderTypeIndex;


        /// Reports a missing provider Type.
        ///
        /// @tparam TNeedle Provider Type being searched.
        template<class TNeedle>
        struct ProviderTypeIndex<TNeedle> {

            /// Missing-provider sentinel index.
            static constexpr std::size_t Value =
                static_cast<std::size_t>(-1);

        };


        /// Resolves one provider Type recursively.
        ///
        /// @tparam TNeedle Provider Type being searched.
        /// @tparam TFirstProvider Current provider Type.
        /// @tparam TRestProviders Remaining provider Types.
        template<class TNeedle, class TFirstProvider, class... TRestProviders>
        struct ProviderTypeIndex<
            TNeedle,
            TFirstProvider,
            TRestProviders...
        > {

            private:

                /// Index reported by the remaining provider pack.
                static constexpr std::size_t RemainingIndex =
                    ProviderTypeIndex<
                        TNeedle,
                        TRestProviders...
                    >::Value;


            public:

                /// Zero-based provider index, or the missing-provider sentinel.
                static constexpr std::size_t Value =
                    std::is_same_v<
                        TNeedle,
                        TFirstProvider
                    >
                        ? 0U
                        : (
                            RemainingIndex == static_cast<std::size_t>(-1)
                                ? static_cast<std::size_t>(-1)
                                : RemainingIndex + 1U
                        );

        };


        /// Resolves the last Type in one compile-time provider pack.
        ///
        /// @tparam TProviders Provider Types represented by the pack.
        template<class... TProviders>
        struct LastProviderType {

            /// Missing-provider sentinel used for an empty pack.
            using Type = void;

        };


        /// Resolves the last Type in one single-provider pack.
        ///
        /// @tparam TProvider Only provider Type.
        template<class TProvider>
        struct LastProviderType<TProvider> {

            /// Last provider Type.
            using Type = TProvider;

        };


        /// Resolves the last Type in one multi-provider pack.
        ///
        /// @tparam TFirstProvider First provider Type.
        /// @tparam TRestProviders Remaining provider Types.
        template<class TFirstProvider, class... TRestProviders>
        struct LastProviderType<TFirstProvider, TRestProviders...> : LastProviderType<TRestProviders...> {};

    } // ESPressio::System::CompositionFramework::Detail


    /// Stores a deterministic compile-time list of provider Types returned by Composition queries.
    ///
    /// @tparam TProviders Provider Types represented by this list in declaration order.
    template<class... TProviders>
    struct ProviderList {

        // Provider-list metadata.

        /// Number of provider Types contained in this list.
        static constexpr std::size_t Count = sizeof...(TProviders);

        /// Indicates whether this provider list is empty.
        static constexpr bool IsEmpty = Count == 0U;

        /// First provider Type, or void when this list is empty.
        using Front = typename Detail::FirstProviderType<TProviders...>::Type;

        /// Last provider Type, or void when this list is empty.
        using Back = typename Detail::LastProviderType<TProviders...>::Type;


        // Provider queries.

        /// Indicates whether the specified provider Type is contained in this list.
        ///
        /// @tparam TProvider Provider Type being queried.
        template<class TProvider>
        static constexpr bool Contains = (std::is_same_v<TProvider, TProviders> || ...);

        /// Returns the zero-based index of one provider Type, or Count when it is absent.
        ///
        /// @tparam TProvider Provider Type whose index is requested.
        template<class TProvider>
        static constexpr std::size_t IndexOf = []() constexpr {
            constexpr auto index = Detail::ProviderTypeIndex<
                TProvider,
                TProviders...
            >::Value;

            return index == static_cast<std::size_t>(-1)
                ? Count
                : index;
        }();


        /// Indicates whether every supplied provider Type is contained in this list.
        ///
        /// @tparam TCandidates Provider Types whose complete presence is required.
        template<class... TCandidates>
        static constexpr bool ContainsAll = (Contains<TCandidates> && ...);

        /// Indicates whether at least one supplied provider Type is contained in this list.
        ///
        /// @tparam TCandidates Provider Types of which any presence is sufficient.
        template<class... TCandidates>
        static constexpr bool ContainsAny = (Contains<TCandidates> || ...);


        // Indexed provider access.

        /// Resolves one provider Type by its zero-based list index.
        ///
        /// @tparam TIndex Zero-based provider index.
        template<std::size_t TIndex>
        struct ResolveAt {

            static_assert(
                TIndex < Count,
                "ProviderList index is outside the represented provider range"
            );

            /// Provider Type at the requested index.
            using Type = std::tuple_element_t<
                TIndex,
                std::tuple<TProviders...>
            >;

        };


        /// Provider Type at the supplied zero-based index.
        ///
        /// @tparam TIndex Zero-based provider index.
        template<std::size_t TIndex>
        using At = typename ResolveAt<TIndex>::Type;


        // Compile-time traversal.

        /// Invokes one templated callable once for every represented provider Type in declaration order.
        ///
        /// The callable must support `operator()<TProvider>()` for every provider Type.
        /// This operation retains no runtime state and is usable during constant evaluation.
        ///
        /// @tparam TCallable Templated callable Type.
        /// @param callable Callable receiving each provider Type as a template argument.
        template<class TCallable>
        static constexpr void ForEachType(
            TCallable&& callable
        ) {
            if constexpr (Count == 0U) {
                static_cast<void>(callable);
            } else {
                (
                    callable.template operator()<TProviders>(),
                    ...
                );
            }
        }

    };


    namespace Detail {

        /// Prepends one provider Type to a ProviderList.
        ///
        /// @tparam TProvider Provider Type to prepend.
        /// @tparam TProviderList Existing provider list.
        template<class TProvider, class TProviderList>
        struct PrependProviderList;


        /// Prepends one provider Type while preserving the existing list order.
        ///
        /// @tparam TProvider Provider Type to prepend.
        /// @tparam TProviders Existing provider Types.
        template<class TProvider, class... TProviders>
        struct PrependProviderList<
            TProvider,
            ProviderList<TProviders...>
        > {

            /// Resulting provider list.
            using Type = ProviderList<
                TProvider,
                TProviders...
            >;

        };


        /// Appends one provider Type to a ProviderList.
        ///
        /// @tparam TProviderList Existing provider list.
        /// @tparam TProvider Provider Type to append.
        template<class TProviderList, class TProvider>
        struct AppendProviderList;


        /// Appends one provider Type while preserving the existing list order.
        ///
        /// @tparam TProviders Existing provider Types.
        /// @tparam TProvider Provider Type to append.
        template<class... TProviders, class TProvider>
        struct AppendProviderList<
            ProviderList<TProviders...>,
            TProvider
        > {

            /// Resulting provider list.
            using Type = ProviderList<
                TProviders...,
                TProvider
            >;

        };


        /// Appends one provider Type only when it is not already present in a ProviderList.
        ///
        /// @tparam TProviderList Existing provider list.
        /// @tparam TProvider Provider Type conditionally appended.
        template<class TProviderList, class TProvider>
        struct AppendUniqueProviderList;


        /// Conditionally appends one provider Type while preserving declaration order.
        ///
        /// @tparam TProviders Existing provider Types.
        /// @tparam TProvider Provider Type conditionally appended.
        template<class... TProviders, class TProvider>
        struct AppendUniqueProviderList<
            ProviderList<TProviders...>,
            TProvider
        > {

            /// Provider list with the candidate appended only when previously absent.
            using Type = std::conditional_t<
                ProviderList<TProviders...>::template Contains<TProvider>,
                ProviderList<TProviders...>,
                ProviderList<
                    TProviders...,
                    TProvider
                >
            >;

        };


        /// Adds every provider from one source list to one accumulated union.
        ///
        /// @tparam TAccumulatedProviders Providers already represented by the union.
        /// @tparam TRemainingProviders Source providers still requiring inspection.
        template<class TAccumulatedProviders, class TRemainingProviders>
        struct UnionProviderLists;


        /// Completes provider-list union when the source list is exhausted.
        ///
        /// @tparam TAccumulatedProviders Complete union result.
        template<class TAccumulatedProviders>
        struct UnionProviderLists<
            TAccumulatedProviders,
            ProviderList<>
        > {

            /// Complete union result.
            using Type = TAccumulatedProviders;

        };


        /// Conditionally appends one source provider and continues union construction.
        ///
        /// @tparam TAccumulatedProviders Providers already represented by the union.
        /// @tparam TFirstProvider Current source provider Type.
        /// @tparam TRestProviders Remaining source provider Types.
        template<
            class TAccumulatedProviders,
            class TFirstProvider,
            class... TRestProviders
        >
        struct UnionProviderLists<
            TAccumulatedProviders,
            ProviderList<
                TFirstProvider,
                TRestProviders...
            >
        > {

            private:

                /// Union state after conditionally appending the current provider.
                using NextAccumulated = typename AppendUniqueProviderList<
                    TAccumulatedProviders,
                    TFirstProvider
                >::Type;


            public:

                /// Complete declaration-order-preserving union result.
                using Type = typename UnionProviderLists<
                    NextAccumulated,
                    ProviderList<TRestProviders...>
                >::Type;

        };


        /// Filters one ProviderList using one compile-time unary predicate class template.
        ///
        /// The predicate must expose a Boolean `value` member for each provider Type.
        ///
        /// @tparam TProviderList Providers being filtered.
        /// @tparam TPredicate Unary predicate class template.
        template<
            class TProviderList,
            template<class> class TPredicate
        >
        struct FilterProviderList;


        /// Completes filtering when no providers remain.
        ///
        /// @tparam TPredicate Unary predicate class template.
        template<template<class> class TPredicate>
        struct FilterProviderList<
            ProviderList<>,
            TPredicate
        > {

            /// Empty filtering result.
            using Type = ProviderList<>;

        };


        /// Filters one provider and continues recursively.
        ///
        /// @tparam TPredicate Unary predicate class template.
        /// @tparam TFirstProvider Current provider Type.
        /// @tparam TRestProviders Remaining provider Types.
        template<
            template<class> class TPredicate,
            class TFirstProvider,
            class... TRestProviders
        >
        struct FilterProviderList<
            ProviderList<
                TFirstProvider,
                TRestProviders...
            >,
            TPredicate
        > {

            private:

                /// Filtering result for the remaining provider Types.
                using Remaining = typename FilterProviderList<
                    ProviderList<TRestProviders...>,
                    TPredicate
                >::Type;


            public:

                /// Ordered filtering result.
                using Type = std::conditional_t<
                    TPredicate<TFirstProvider>::value,
                    typename PrependProviderList<
                        TFirstProvider,
                        Remaining
                    >::Type,
                    Remaining
                >;

        };


        /// Concatenates two ProviderLists without changing either list's declaration order.
        ///
        /// @tparam TLeftProviders Left provider list.
        /// @tparam TRightProviders Right provider list.
        template<class TLeftProviders, class TRightProviders>
        struct ConcatProviderLists;


        /// Concatenates two concrete provider packs.
        ///
        /// @tparam TLeftProviders Provider Types from the left list.
        /// @tparam TRightProviders Provider Types from the right list.
        template<class... TLeftProviders, class... TRightProviders>
        struct ConcatProviderLists<
            ProviderList<TLeftProviders...>,
            ProviderList<TRightProviders...>
        > {

            /// Concatenated provider list.
            using Type = ProviderList<
                TLeftProviders...,
                TRightProviders...
            >;

        };


        /// Removes every occurrence of one provider Type from a ProviderList.
        ///
        /// @tparam TProvider Provider Type to remove.
        /// @tparam TProviderList Provider list being filtered.
        template<class TProvider, class TProviderList>
        struct RemoveProviderFromList;


        /// Completes removal from an empty ProviderList.
        ///
        /// @tparam TProvider Provider Type being removed.
        template<class TProvider>
        struct RemoveProviderFromList<
            TProvider,
            ProviderList<>
        > {

            /// Empty removal result.
            using Type = ProviderList<>;

        };


        /// Removes one provider Type recursively while preserving all remaining order.
        ///
        /// @tparam TProvider Provider Type being removed.
        /// @tparam TFirstProvider Current provider Type.
        /// @tparam TRestProviders Remaining provider Types.
        template<
            class TProvider,
            class TFirstProvider,
            class... TRestProviders
        >
        struct RemoveProviderFromList<
            TProvider,
            ProviderList<
                TFirstProvider,
                TRestProviders...
            >
        > {

            private:

                /// Removal result for the remaining provider Types.
                using Remaining = typename RemoveProviderFromList<
                    TProvider,
                    ProviderList<TRestProviders...>
                >::Type;


            public:

                /// Removal result preserving declaration order.
                using Type = std::conditional_t<
                    std::is_same_v<
                        TProvider,
                        TFirstProvider
                    >,
                    Remaining,
                    typename PrependProviderList<
                        TFirstProvider,
                        Remaining
                    >::Type
                >;

        };


        /// Computes the declaration-order-preserving intersection of two ProviderLists.
        ///
        /// @tparam TLeftProviders Left provider list controlling result order.
        /// @tparam TRightProviders Right provider list used for membership checks.
        template<class TLeftProviders, class TRightProviders>
        struct IntersectProviderLists;


        /// Completes intersection when the left list is empty.
        ///
        /// @tparam TRightProviders Right provider list.
        template<class TRightProviders>
        struct IntersectProviderLists<
            ProviderList<>,
            TRightProviders
        > {

            /// Empty intersection result.
            using Type = ProviderList<>;

        };


        /// Computes one ordered intersection step.
        ///
        /// @tparam TFirstProvider Current left provider Type.
        /// @tparam TRestProviders Remaining left provider Types.
        /// @tparam TRightProviders Right provider list.
        template<
            class TRightProviders,
            class TFirstProvider,
            class... TRestProviders
        >
        struct IntersectProviderLists<
            ProviderList<
                TFirstProvider,
                TRestProviders...
            >,
            TRightProviders
        > {

            private:

                /// Intersection result for the remaining left provider Types.
                using Remaining = typename IntersectProviderLists<
                    ProviderList<TRestProviders...>,
                    TRightProviders
                >::Type;


            public:

                /// Ordered intersection result.
                using Type = std::conditional_t<
                    TRightProviders::template Contains<TFirstProvider>,
                    typename PrependProviderList<
                        TFirstProvider,
                        Remaining
                    >::Type,
                    Remaining
                >;

        };


        /// Computes the declaration-order-preserving difference of two ProviderLists.
        ///
        /// @tparam TLeftProviders Left provider list controlling result order.
        /// @tparam TRightProviders Provider Types excluded from the result.
        template<class TLeftProviders, class TRightProviders>
        struct DifferenceProviderLists;


        /// Completes difference when the left list is empty.
        ///
        /// @tparam TRightProviders Right provider list.
        template<class TRightProviders>
        struct DifferenceProviderLists<
            ProviderList<>,
            TRightProviders
        > {

            /// Empty difference result.
            using Type = ProviderList<>;

        };


        /// Computes one ordered difference step.
        ///
        /// @tparam TFirstProvider Current left provider Type.
        /// @tparam TRestProviders Remaining left provider Types.
        /// @tparam TRightProviders Provider Types excluded from the result.
        template<
            class TRightProviders,
            class TFirstProvider,
            class... TRestProviders
        >
        struct DifferenceProviderLists<
            ProviderList<
                TFirstProvider,
                TRestProviders...
            >,
            TRightProviders
        > {

            private:

                /// Difference result for the remaining left provider Types.
                using Remaining = typename DifferenceProviderLists<
                    ProviderList<TRestProviders...>,
                    TRightProviders
                >::Type;


            public:

                /// Ordered difference result.
                using Type = std::conditional_t<
                    TRightProviders::template Contains<TFirstProvider>,
                    Remaining,
                    typename PrependProviderList<
                        TFirstProvider,
                        Remaining
                    >::Type
                >;

        };


        /// Indicates whether a type pack contains the specified type.
        template<class TNeedle, class... THaystack>
        inline constexpr bool ContainsTypeV = (std::is_same_v<TNeedle, THaystack> || ...);


        /// Default provider metadata for Types that are not provider declarations.
        template<class TDomain, class TProvider, class = void>
        struct IsProviderDeclarationForDomain : std::false_type {};


        /// Validates a concrete provider Type against a composition Domain.
        template<class TDomain, class TProvider>
        struct IsProviderDeclarationForDomain<
            TDomain,
            TProvider,
            std::void_t<
                typename TProvider::ProviderDeclarationTag,
                typename TProvider::CompositionDomain,
                typename TProvider::CompositionOffers,
                typename TProvider::CompositionContract
            >
        > : std::bool_constant<
            std::is_same_v<typename TProvider::CompositionDomain, TDomain> &&
            TProvider::CompositionOffers::template IsForDomain<TDomain> &&
            ContractTraits<typename TProvider::CompositionContract>::IsValid &&
            TProvider::CompositionContract::template IsProviderContractFor<TDomain>
        > {};


        /// Indicates whether one provider supplies the requested capability.
        template<class TProvider, class TCapability>
        inline constexpr bool ProviderOffersCapabilityV = TProvider::CompositionOffers::template Contains<TCapability>;


        /// Counts providers that supply the requested capability.
        template<class TCapability, class... TProviders>
        inline constexpr std::size_t ProviderCountV =
            (std::size_t{0U} + ... + (ProviderOffersCapabilityV<TProviders, TCapability> ? std::size_t{1U} : std::size_t{0U}));


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
                ProviderOffersCapabilityV<TFirstProvider, TCapability>,
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
                ProviderOffersCapabilityV<TFirstProvider, TCapability>,
                ProviderList<TAccumulatedProviders..., TFirstProvider>,
                ProviderList<TAccumulatedProviders...>
            >;

            /// Final provider list returned after the remaining providers are inspected.
            using Type = typename FilterProviders<TCapability, NextProviders, TRestProviders...>::Type;

        };


        /// Determines whether one provider satisfies one capability requirement.
        template<class TProvider, class TNeed, bool TOffersDeclarationCapability = ProviderOffersCapabilityV<TProvider, typename TNeed::CapabilityType>>
        struct ProviderSatisfiesRequirement : std::false_type {};


        /// Evaluates a capability requirement against a provider that supplies the required capability.
        template<class TProvider, class TNeed>
        struct ProviderSatisfiesRequirement<TProvider, TNeed, true> : std::bool_constant<
            TNeed::template OfferSatisfied<
                typename TProvider::CompositionOffers::template OfferFor<typename TNeed::CapabilityType>
            >
        > {};


        /// Counts providers that satisfy one capability requirement.
        template<class TNeed, class... TProviders>
        inline constexpr std::size_t SatisfyingProviderCountV =
            (std::size_t{0U} + ... + (ProviderSatisfiesRequirement<TProviders, TNeed>::value ? std::size_t{1U} : std::size_t{0U}));


        /// Selects the first provider satisfying one complete capability requirement.
        template<class TNeed, class... TProviders>
        struct FirstSatisfyingProvider;


        /// Represents an unsuccessful satisfying-provider lookup.
        template<class TNeed>
        struct FirstSatisfyingProvider<TNeed> {

            // Lookup result.

            /// Type returned when no provider satisfies the requested Requirement.
            using Type = void;

        };


        /// Continues a satisfying-provider lookup until one provider satisfies the requested Requirement.
        template<class TNeed, class TFirstProvider, class... TRestProviders>
        struct FirstSatisfyingProvider<TNeed, TFirstProvider, TRestProviders...> {

            // Lookup result.

            /// First provider satisfying the Requirement, or the result of searching the remaining providers.
            using Type = std::conditional_t<
                ProviderSatisfiesRequirement<TFirstProvider, TNeed>::value,
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


        /// Adds providers satisfying the Requirement to the accumulated list and continues filtering.
        template<class TNeed, class... TAccumulatedProviders, class TFirstProvider, class... TRestProviders>
        struct FilterSatisfyingProviders<TNeed, ProviderList<TAccumulatedProviders...>, TFirstProvider, TRestProviders...> {

            // Filtering state.

            /// Provider list to use for the next filtering step.
            using NextProviders = std::conditional_t<
                ProviderSatisfiesRequirement<TFirstProvider, TNeed>::value,
                ProviderList<TAccumulatedProviders..., TFirstProvider>,
                ProviderList<TAccumulatedProviders...>
            >;

            /// Final provider list returned after the remaining providers are inspected.
            using Type = typename FilterSatisfyingProviders<TNeed, NextProviders, TRestProviders...>::Type;

        };


        /// Forward declaration for provider-versus-RequirementList joint matching.
        ///
        /// @tparam TProvider Provider Type being inspected.
        /// @tparam TRequirementList Requirements which must all match.
        template<class TProvider, class TRequirementList>
        struct ProviderSatisfiesRequirementList;


        /// Forward declaration for best-Property filtering used by generic selection policies.
        ///
        /// @tparam TProperty Property used for ranking.
        /// @tparam TMinimize Whether lower values are preferred.
        /// @tparam TProviderList Candidate providers.
        template<
            class TProperty,
            bool TMinimize,
            class TProviderList
        >
        struct BestProviderList;


        /// Filters one ProviderList to providers satisfying every Requirement in one RequirementList.
        ///
        /// @tparam TRequirementList Requirements which must all match the same provider.
        /// @tparam TAccumulatedProviders Providers selected so far.
        /// @tparam TRemainingProviders Providers still requiring inspection.
        template<
            class TRequirementList,
            class TAccumulatedProviders,
            class TRemainingProviders
        >
        struct FilterJointlySatisfyingProviders;


        /// Completes joint-provider filtering when no providers remain.
        ///
        /// @tparam TRequirementList Requirements being jointly evaluated.
        /// @tparam TAccumulatedProviders Providers selected so far.
        template<class TRequirementList, class TAccumulatedProviders>
        struct FilterJointlySatisfyingProviders<
            TRequirementList,
            TAccumulatedProviders,
            ProviderList<>
        > {

            /// Complete joint-match ProviderList.
            using Type = TAccumulatedProviders;

        };


        /// Evaluates one provider against the complete RequirementList and continues filtering.
        ///
        /// @tparam TRequirementList Requirements which must all match one provider.
        /// @tparam TAccumulatedProviders Providers selected so far.
        /// @tparam TFirstProvider Current provider Type.
        /// @tparam TRestProviders Remaining provider Types.
        template<
            class TRequirementList,
            class TAccumulatedProviders,
            class TFirstProvider,
            class... TRestProviders
        >
        struct FilterJointlySatisfyingProviders<
            TRequirementList,
            TAccumulatedProviders,
            ProviderList<
                TFirstProvider,
                TRestProviders...
            >
        > {

            private:

                /// Accumulated result after conditionally adding the current provider.
                using NextAccumulated = std::conditional_t<
                    ProviderSatisfiesRequirementList<
                        TFirstProvider,
                        TRequirementList
                    >::value,
                    typename AppendProviderList<
                        TAccumulatedProviders,
                        TFirstProvider
                    >::Type,
                    TAccumulatedProviders
                >;


            public:

                /// Complete joint-match ProviderList.
                using Type = typename FilterJointlySatisfyingProviders<
                    TRequirementList,
                    NextAccumulated,
                    ProviderList<TRestProviders...>
                >::Type;

        };


        /// Applies one explicit selection policy to an already-qualified ProviderList.
        ///
        /// Unlike SelectionResult, this form is not tied to one Capability Requirement and therefore
        /// can select providers jointly satisfying several Requirements across several Capabilities.
        ///
        /// @tparam TSelectionPolicy Explicit provider selection policy.
        /// @tparam TProviderList Already-qualified provider candidates.
        template<class TSelectionPolicy, class TProviderList>
        struct ProviderListSelectionResult;


        /// Selects every provider in an already-qualified ProviderList.
        ///
        /// @tparam TProviderList Qualified provider candidates.
        template<class TProviderList>
        struct ProviderListSelectionResult<
            SelectAll,
            TProviderList
        > {

            /// Complete candidate ProviderList.
            using Type = TProviderList;

        };


        /// Selects exactly one provider from an already-qualified ProviderList.
        ///
        /// @tparam TProviderList Qualified provider candidates.
        template<class TProviderList>
        struct ProviderListSelectionResult<
            SelectUnique,
            TProviderList
        > {

            static_assert(
                TProviderList::Count == 1U,
                "SelectUnique requires exactly one qualified provider"
            );

            /// Unique qualified provider Type.
            using Type = typename TProviderList::Front;

        };


        /// Selects zero or one provider from an already-qualified ProviderList.
        ///
        /// @tparam TProviderList Qualified provider candidates.
        template<class TProviderList>
        struct ProviderListSelectionResult<
            SelectOptionalUnique,
            TProviderList
        > {

            static_assert(
                TProviderList::Count <= 1U,
                "SelectOptionalUnique rejects ambiguous qualified providers"
            );

            /// Unique qualified provider Type, or void when no provider matches.
            using Type = typename TProviderList::Front;

        };


        /// Selects the unique provider advertising the minimum Property value.
        ///
        /// @tparam TProperty Property used for ranking.
        /// @tparam TProviderList Qualified provider candidates.
        template<class TProperty, class TProviderList>
        struct ProviderListSelectionResult<
            SelectMinimum<TProperty>,
            TProviderList
        > {

            /// Providers tied for the minimum Property value.
            using BestProviders = typename BestProviderList<
                TProperty,
                true,
                TProviderList
            >::Type;

            static_assert(
                BestProviders::Count == 1U,
                "SelectMinimum requires one unique minimum Property value"
            );

            /// Provider Type advertising the unique minimum value.
            using Type = typename BestProviders::Front;

        };


        /// Selects the unique provider advertising the maximum Property value.
        ///
        /// @tparam TProperty Property used for ranking.
        /// @tparam TProviderList Qualified provider candidates.
        template<class TProperty, class TProviderList>
        struct ProviderListSelectionResult<
            SelectMaximum<TProperty>,
            TProviderList
        > {

            /// Providers tied for the maximum Property value.
            using BestProviders = typename BestProviderList<
                TProperty,
                false,
                TProviderList
            >::Type;

            static_assert(
                BestProviders::Count == 1U,
                "SelectMaximum requires one unique maximum Property value"
            );

            /// Provider Type advertising the unique maximum value.
            using Type = typename BestProviders::Front;

        };


        /// Selects every provider tied for the minimum Property value.
        ///
        /// @tparam TProperty Property used for ranking.
        /// @tparam TProviderList Qualified provider candidates.
        template<class TProperty, class TProviderList>
        struct ProviderListSelectionResult<
            SelectAllMinimum<TProperty>,
            TProviderList
        > {

            /// Providers tied for the minimum Property value.
            using Type = typename BestProviderList<
                TProperty,
                true,
                TProviderList
            >::Type;

        };


        /// Selects every provider tied for the maximum Property value.
        ///
        /// @tparam TProperty Property used for ranking.
        /// @tparam TProviderList Qualified provider candidates.
        template<class TProperty, class TProviderList>
        struct ProviderListSelectionResult<
            SelectAllMaximum<TProperty>,
            TProviderList
        > {

            /// Providers tied for the maximum Property value.
            using Type = typename BestProviderList<
                TProperty,
                false,
                TProviderList
            >::Type;

        };


        /// Evaluates one Requirement's accepted provider count.
        ///
        /// @tparam TRequirement Consolidated Requirement declaration.
        /// @tparam TProviderCount Number of providers satisfying its qualification.
        template<
            class TRequirement,
            std::size_t TProviderCount
        >
        struct RequirementCardinalitySatisfied : std::bool_constant<
            TRequirement::AcceptsProviderCount(
                TProviderCount
            )
        > {};


        /// Indicates whether one provider advertises a requested Property.
        ///
        /// @tparam TProvider Provider Type being inspected.
        /// @tparam TProperty Property whose presence is required.
        /// @tparam TOffersDeclarationCapability Whether the provider supplies the Property's owning Capability.
        template<
            class TProvider,
            class TProperty,
            bool TOffersDeclarationCapability = ProviderOffersCapabilityV<
                TProvider,
                typename TProperty::CapabilityType
            >
        >
        struct ProviderAdvertisesProperty : std::false_type {};


        /// Inspects Property presence after confirming the provider supplies its owning Capability.
        ///
        /// @tparam TProvider Provider Type being inspected.
        /// @tparam TProperty Property whose presence is required.
        template<class TProvider, class TProperty>
        struct ProviderAdvertisesProperty<
            TProvider,
            TProperty,
            true
        > : std::bool_constant<
            TProvider::CompositionOffers::template PropertiesFor<
                typename TProperty::CapabilityType
            >::template Contains<TProperty>
        > {};


        /// Returns one provider's compile-time Property value.
        ///
        /// @tparam TProvider Provider Type being inspected.
        /// @tparam TProperty Property whose value is requested.
        template<class TProvider, class TProperty>
        inline constexpr auto ProviderPropertyValueV =
            TProvider::CompositionOffers::template PropertiesFor<
                typename TProperty::CapabilityType
            >::template Value<TProperty>;


        /// Indicates whether one provider is tied for the best Property value in a candidate pack.
        ///
        /// @tparam TCandidate Candidate provider Type.
        /// @tparam TProperty Property used for ordering.
        /// @tparam TMinimize Whether lower values are preferred.
        /// @tparam TProviders Complete candidate provider pack.
        template<
            class TCandidate,
            class TProperty,
            bool TMinimize,
            class... TProviders
        >
        inline constexpr bool IsBestProviderV = []() consteval {
            if constexpr (TMinimize) {
                return (
                    (
                        ProviderPropertyValueV<
                            TCandidate,
                            TProperty
                        > <=
                        ProviderPropertyValueV<
                            TProviders,
                            TProperty
                        >
                    ) &&
                    ...
                );
            } else {
                return (
                    (
                        ProviderPropertyValueV<
                            TCandidate,
                            TProperty
                        > >=
                        ProviderPropertyValueV<
                            TProviders,
                            TProperty
                        >
                    ) &&
                    ...
                );
            }
        }();


        /// Indicates whether one candidate is tied for the best Property value in a ProviderList.
        ///
        /// @tparam TCandidate Candidate provider Type.
        /// @tparam TProperty Property used for ordering.
        /// @tparam TMinimize Whether lower values are preferred.
        /// @tparam TProviderList Complete candidate ProviderList.
        template<
            class TCandidate,
            class TProperty,
            bool TMinimize,
            class TProviderList
        >
        struct IsBestProviderInList;


        /// Evaluates one candidate against a concrete provider pack.
        ///
        /// @tparam TCandidate Candidate provider Type.
        /// @tparam TProperty Property used for ordering.
        /// @tparam TMinimize Whether lower values are preferred.
        /// @tparam TProviders Complete candidate provider pack.
        template<
            class TCandidate,
            class TProperty,
            bool TMinimize,
            class... TProviders
        >
        struct IsBestProviderInList<
            TCandidate,
            TProperty,
            TMinimize,
            ProviderList<TProviders...>
        > : std::bool_constant<
            IsBestProviderV<
                TCandidate,
                TProperty,
                TMinimize,
                TProviders...
            >
        > {};


        /// Filters a ProviderList to every provider tied for one best Property value.
        ///
        /// @tparam TProperty Property used for ordering.
        /// @tparam TMinimize Whether lower values are preferred.
        /// @tparam TCandidates Complete candidate ProviderList.
        /// @tparam TRemaining Remaining providers being inspected.
        template<
            class TProperty,
            bool TMinimize,
            class TCandidates,
            class TRemaining
        >
        struct FilterBestProviders;


        /// Completes best-provider filtering.
        ///
        /// @tparam TProperty Property used for ordering.
        /// @tparam TMinimize Whether lower values are preferred.
        /// @tparam TCandidates Complete candidate ProviderList.
        template<
            class TProperty,
            bool TMinimize,
            class TCandidates
        >
        struct FilterBestProviders<
            TProperty,
            TMinimize,
            TCandidates,
            ProviderList<>
        > {

            /// Empty terminal result.
            using Type = ProviderList<>;

        };


        /// Evaluates one best-provider candidate and continues filtering.
        ///
        /// @tparam TProperty Property used for ordering.
        /// @tparam TMinimize Whether lower values are preferred.
        /// @tparam TCandidateProviders Complete candidate provider pack.
        /// @tparam TFirstProvider Current provider Type.
        /// @tparam TRestProviders Remaining provider Types.
        template<
            class TProperty,
            bool TMinimize,
            class TCandidates,
            class TFirstProvider,
            class... TRestProviders
        >
        struct FilterBestProviders<
            TProperty,
            TMinimize,
            TCandidates,
            ProviderList<
                TFirstProvider,
                TRestProviders...
            >
        > {

            private:

                /// Best-provider result for the remaining candidates.
                using Remaining = typename FilterBestProviders<
                    TProperty,
                    TMinimize,
                    TCandidates,
                    ProviderList<TRestProviders...>
                >::Type;


            public:

                /// Ordered best-provider result.
                using Type = std::conditional_t<
                    IsBestProviderInList<
                        TFirstProvider,
                        TProperty,
                        TMinimize,
                        TCandidates
                    >::value,
                    typename PrependProviderList<
                        TFirstProvider,
                        Remaining
                    >::Type,
                    Remaining
                >;

        };


        /// Validates and resolves every provider tied for one best Property value.
        ///
        /// @tparam TProperty Property used for ordering.
        /// @tparam TMinimize Whether lower values are preferred.
        /// @tparam TProviderList Candidate providers.
        template<
            class TProperty,
            bool TMinimize,
            class TProviderList
        >
        struct BestProviderList;


        /// Resolves best providers from one concrete candidate pack.
        ///
        /// @tparam TProperty Property used for ordering.
        /// @tparam TMinimize Whether lower values are preferred.
        /// @tparam TProviders Candidate provider Types.
        template<
            class TProperty,
            bool TMinimize,
            class... TProviders
        >
        struct BestProviderList<
            TProperty,
            TMinimize,
            ProviderList<TProviders...>
        > {

            static_assert(
                IsPropertyV<TProperty>,
                "Best-provider selection requires a concrete composition Property"
            );

            static_assert(
                (ProviderAdvertisesProperty<TProviders, TProperty>::value && ...),
                "Every ranked provider must advertise the Property used by the selection policy"
            );

            /// Providers tied for the best Property value in original declaration order.
            using Type = typename FilterBestProviders<
                TProperty,
                TMinimize,
                ProviderList<TProviders...>,
                ProviderList<TProviders...>
            >::Type;

        };


        /// Resolves one explicit selection policy against a Requirement match list.
        ///
        /// @tparam TRequirement Requirement whose matches are being selected.
        /// @tparam TSelectionPolicy Explicit selection policy.
        /// @tparam TProviderList Providers satisfying the Requirement qualification.
        template<
            class TRequirement,
            class TSelectionPolicy,
            class TProviderList
        >
        struct SelectionResult;


        /// Selects every satisfying provider.
        ///
        /// @tparam TRequirement Requirement whose matches are being selected.
        /// @tparam TProviderList Matching providers.
        template<class TRequirement, class TProviderList>
        struct SelectionResult<
            TRequirement,
            SelectAll,
            TProviderList
        > {

            /// Complete matching ProviderList.
            using Type = TProviderList;

        };


        /// Selects exactly one satisfying provider.
        ///
        /// @tparam TRequirement Requirement whose matches are being selected.
        /// @tparam TProviderList Matching providers.
        template<class TRequirement, class TProviderList>
        struct SelectionResult<
            TRequirement,
            SelectUnique,
            TProviderList
        > {

            static_assert(
                TProviderList::Count == 1U,
                "SelectUnique requires exactly one satisfying provider"
            );

            /// Unique satisfying provider Type.
            using Type = typename TProviderList::Front;

        };


        /// Selects zero or one satisfying provider.
        ///
        /// @tparam TRequirement Requirement whose matches are being selected.
        /// @tparam TProviderList Matching providers.
        template<class TRequirement, class TProviderList>
        struct SelectionResult<
            TRequirement,
            SelectOptionalUnique,
            TProviderList
        > {

            static_assert(
                TProviderList::Count <= 1U,
                "SelectOptionalUnique rejects ambiguous provider matches"
            );

            /// Unique satisfying provider Type, or void when no provider matches.
            using Type = typename TProviderList::Front;

        };


        /// Selects the unique provider advertising the minimum Property value.
        ///
        /// @tparam TRequirement Requirement whose matches are being ranked.
        /// @tparam TProperty Property used for ranking.
        /// @tparam TProviderList Matching providers.
        template<
            class TRequirement,
            class TProperty,
            class TProviderList
        >
        struct SelectionResult<
            TRequirement,
            SelectMinimum<TProperty>,
            TProviderList
        > {

            static_assert(
                IsPropertyForV<
                    typename TRequirement::CapabilityType,
                    TProperty
                >,
                "SelectMinimum Property must belong to the Requirement Capability"
            );

            /// Providers tied for the minimum Property value.
            using BestProviders = typename BestProviderList<
                TProperty,
                true,
                TProviderList
            >::Type;

            static_assert(
                BestProviders::Count == 1U,
                "SelectMinimum requires one unique minimum Property value"
            );

            /// Provider Type advertising the unique minimum value.
            using Type = typename BestProviders::Front;

        };


        /// Selects the unique provider advertising the maximum Property value.
        ///
        /// @tparam TRequirement Requirement whose matches are being ranked.
        /// @tparam TProperty Property used for ranking.
        /// @tparam TProviderList Matching providers.
        template<
            class TRequirement,
            class TProperty,
            class TProviderList
        >
        struct SelectionResult<
            TRequirement,
            SelectMaximum<TProperty>,
            TProviderList
        > {

            static_assert(
                IsPropertyForV<
                    typename TRequirement::CapabilityType,
                    TProperty
                >,
                "SelectMaximum Property must belong to the Requirement Capability"
            );

            /// Providers tied for the maximum Property value.
            using BestProviders = typename BestProviderList<
                TProperty,
                false,
                TProviderList
            >::Type;

            static_assert(
                BestProviders::Count == 1U,
                "SelectMaximum requires one unique maximum Property value"
            );

            /// Provider Type advertising the unique maximum value.
            using Type = typename BestProviders::Front;

        };


        /// Selects every provider tied for the minimum Property value.
        ///
        /// @tparam TRequirement Requirement whose matches are being ranked.
        /// @tparam TProperty Property used for ranking.
        /// @tparam TProviderList Matching providers.
        template<
            class TRequirement,
            class TProperty,
            class TProviderList
        >
        struct SelectionResult<
            TRequirement,
            SelectAllMinimum<TProperty>,
            TProviderList
        > {

            static_assert(
                IsPropertyForV<
                    typename TRequirement::CapabilityType,
                    TProperty
                >,
                "SelectAllMinimum Property must belong to the Requirement Capability"
            );

            /// Providers tied for the minimum Property value.
            using Type = typename BestProviderList<
                TProperty,
                true,
                TProviderList
            >::Type;

        };


        /// Selects every provider tied for the maximum Property value.
        ///
        /// @tparam TRequirement Requirement whose matches are being ranked.
        /// @tparam TProperty Property used for ranking.
        /// @tparam TProviderList Matching providers.
        template<
            class TRequirement,
            class TProperty,
            class TProviderList
        >
        struct SelectionResult<
            TRequirement,
            SelectAllMaximum<TProperty>,
            TProviderList
        > {

            static_assert(
                IsPropertyForV<
                    typename TRequirement::CapabilityType,
                    TProperty
                >,
                "SelectAllMaximum Property must belong to the Requirement Capability"
            );

            /// Providers tied for the maximum Property value.
            using Type = typename BestProviderList<
                TProperty,
                false,
                TProviderList
            >::Type;

        };


        /// Indicates whether one provider jointly satisfies every Requirement in a RequirementList.
        ///
        /// @tparam TProvider Provider Type being inspected.
        /// @tparam TRequirementList Requirements which must all match the same provider.
        template<class TProvider, class TRequirementList>
        struct ProviderSatisfiesRequirementList;


        /// Evaluates one concrete Requirement pack against one provider.
        ///
        /// @tparam TProvider Provider Type being inspected.
        /// @tparam TRequirements Requirements which must all match.
        template<class TProvider, class... TRequirements>
        struct ProviderSatisfiesRequirementList<
            TProvider,
            RequirementList<TRequirements...>
        > : std::bool_constant<
            (ProviderSatisfiesRequirement<TProvider, TRequirements>::value && ...)
        > {};


        /// Counts providers jointly satisfying every Requirement in one RequirementList.
        ///
        /// @tparam TRequirementList Requirements being jointly evaluated.
        /// @tparam TProviderList Candidate provider Types.
        template<class TRequirementList, class TProviderList>
        struct JointlySatisfyingProviderCount;


        /// Counts jointly satisfying providers in one concrete ProviderList.
        ///
        /// @tparam TRequirementList Requirements being jointly evaluated.
        /// @tparam TProviders Candidate provider Types.
        template<class TRequirementList, class... TProviders>
        struct JointlySatisfyingProviderCount<
            TRequirementList,
            ProviderList<TProviders...>
        > : std::integral_constant<
            std::size_t,
            (
                std::size_t{0U} +
                ... +
                (
                    ProviderSatisfiesRequirementList<
                        TProviders,
                        TRequirementList
                    >::value
                        ? std::size_t{1U}
                        : std::size_t{0U}
                )
            )
        > {};


        /// Counts providers satisfying one Requirement inside one ProviderList.
        ///
        /// @tparam TRequirement Requirement being evaluated.
        /// @tparam TProviderList Candidate provider list.
        template<class TRequirement, class TProviderList>
        struct SatisfyingProviderCountInList;


        /// Counts satisfying providers in one concrete provider pack.
        ///
        /// @tparam TRequirement Requirement being evaluated.
        /// @tparam TProviders Candidate provider Types.
        template<class TRequirement, class... TProviders>
        struct SatisfyingProviderCountInList<
            TRequirement,
            ProviderList<TProviders...>
        > : std::integral_constant<
            std::size_t,
            SatisfyingProviderCountV<
                TRequirement,
                TProviders...
            >
        > {};


        /// Indicates whether every Requirement in one RequirementList has the supplied scope.
        ///
        /// @tparam TRequirementList Requirement list being inspected.
        /// @tparam TScope Scope required from every Requirement.
        template<class TRequirementList, RequirementScope TScope>
        struct RequirementsHaveScope;


        /// Evaluates one Requirement pack for a common scope.
        ///
        /// @tparam TScope Scope required from every Requirement.
        /// @tparam TRequirements Requirements being inspected.
        template<RequirementScope TScope, class... TRequirements>
        struct RequirementsHaveScope<
            RequirementList<TRequirements...>,
            TScope
        > : std::bool_constant<
            ((TRequirements::Scope == TScope) && ...)
        > {};


        /// Forward declaration for exhaustive distinct-provider assignment.
        ///
        /// @tparam TRequirements Requirements still requiring distinct providers.
        /// @tparam TAvailableProviders Provider Types still available for assignment.
        template<class TRequirements, class TAvailableProviders>
        struct DistinctAssignmentExists;


        /// Tries available providers for one Requirement and recursively assigns the remainder.
        ///
        /// @tparam TRequirement Current Requirement being assigned.
        /// @tparam TRestRequirements Remaining Requirements after the current one.
        /// @tparam TAvailableProviders Complete currently available provider list.
        /// @tparam TCandidates Remaining candidate provider Types to try.
        template<
            class TRequirement,
            class TRestRequirements,
            class TAvailableProviders,
            class TCandidates
        >
        struct TryDistinctProviderAssignments;


        /// No candidate can satisfy the current Requirement.
        ///
        /// @tparam TRequirement Current Requirement being assigned.
        /// @tparam TRestRequirements Remaining Requirements.
        /// @tparam TAvailableProviders Complete currently available provider list.
        template<
            class TRequirement,
            class TRestRequirements,
            class TAvailableProviders
        >
        struct TryDistinctProviderAssignments<
            TRequirement,
            TRestRequirements,
            TAvailableProviders,
            ProviderList<>
        > : std::false_type {};


        /// Tries one candidate provider before recursively trying the remaining candidates.
        ///
        /// @tparam TRequirement Current Requirement being assigned.
        /// @tparam TRestRequirements Remaining Requirements.
        /// @tparam TAvailableProviders Complete currently available provider list.
        /// @tparam TFirstProvider Current candidate provider Type.
        /// @tparam TRestProviders Remaining candidate provider Types.
        template<
            class TRequirement,
            class TRestRequirements,
            class TAvailableProviders,
            class TFirstProvider,
            class... TRestProviders
        >
        struct TryDistinctProviderAssignments<
            TRequirement,
            TRestRequirements,
            TAvailableProviders,
            ProviderList<
                TFirstProvider,
                TRestProviders...
            >
        > : std::bool_constant<
            (
                ProviderSatisfiesRequirement<
                    TFirstProvider,
                    TRequirement
                >::value &&
                DistinctAssignmentExists<
                    TRestRequirements,
                    typename RemoveProviderFromList<
                        TFirstProvider,
                        TAvailableProviders
                    >::Type
                >::value
            ) ||
            TryDistinctProviderAssignments<
                TRequirement,
                TRestRequirements,
                TAvailableProviders,
                ProviderList<TRestProviders...>
            >::value
        > {};


        /// An empty Requirement list always has a complete distinct assignment.
        ///
        /// @tparam TAvailableProviders Provider Types which remain unused.
        template<class TAvailableProviders>
        struct DistinctAssignmentExists<
            RequirementList<>,
            TAvailableProviders
        > : std::true_type {};


        /// Assigns the first Requirement and recursively assigns every remaining Requirement.
        ///
        /// @tparam TAvailableProviders Provider Types available for distinct assignment.
        /// @tparam TFirstRequirement Current Requirement being assigned.
        /// @tparam TRestRequirements Remaining Requirements.
        template<
            class TAvailableProviders,
            class TFirstRequirement,
            class... TRestRequirements
        >
        struct DistinctAssignmentExists<
            RequirementList<
                TFirstRequirement,
                TRestRequirements...
            >,
            TAvailableProviders
        > : TryDistinctProviderAssignments<
            TFirstRequirement,
            RequirementList<TRestRequirements...>,
            TAvailableProviders,
            TAvailableProviders
        > {};


        /// Evaluates one provider Contract clause inside a single-domain Composition.
        ///
        /// Cross-domain clauses are deferred until Architecture validation.
        ///
        /// @tparam TClause Contract clause being evaluated.
        /// @tparam TProviderList Providers participating in the Composition.
        template<class TClause, class TProviderList>
        struct ContractClauseSatisfiedInComposition : std::true_type {};


        /// Evaluates one direct consolidated Requirement inside a Composition.
        ///
        /// @tparam TProviderList Providers participating in the Composition.
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
        struct ContractClauseSatisfiedInComposition<
            Requirement<
                TCapability,
                TScope,
                TCardinality,
                TConstraints...
            >,
            TProviderList
        > : std::bool_constant<
            TScope != RequirementScope::SameDomain ||
            RequirementCardinalitySatisfied<
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
            >::value
        > {};


        /// Evaluates one SameProvider clause when it targets the owning Composition Domain.
        ///
        /// @tparam TProviderList Providers participating in the Composition.
        /// @tparam TRequirements Requirements which must be jointly satisfied.
        template<class TProviderList, class... TRequirements>
        struct ContractClauseSatisfiedInComposition<
            SameProvider<TRequirements...>,
            TProviderList
        > : std::bool_constant<
            (
                !RequirementsHaveScope<
                    RequirementList<TRequirements...>,
                    RequirementScope::SameDomain
                >::value ||
                JointlySatisfyingProviderCount<
                    RequirementList<TRequirements...>,
                    TProviderList
                >::value > 0U
            )
        > {};


        /// Evaluates one DistinctProviders clause when it targets only the owning Composition Domain.
        ///
        /// @tparam TProviderList Providers participating in the Composition.
        /// @tparam TRequirements Requirements requiring distinct assignment.
        template<class TProviderList, class... TRequirements>
        struct ContractClauseSatisfiedInComposition<
            DistinctProviders<TRequirements...>,
            TProviderList
        > : std::bool_constant<
            !RequirementsHaveScope<
                RequirementList<TRequirements...>,
                RequirementScope::SameDomain
            >::value ||
            DistinctAssignmentExists<
                RequirementList<TRequirements...>,
                TProviderList
            >::value
        > {};


        /// Evaluates one initialization lifecycle target when it uses SameDomain scope.
        ///
        /// @tparam TProviderList Providers participating in the Composition.
        /// @tparam TRequirement Lifecycle target Requirement.
        template<class TProviderList, class TRequirement>
        struct ContractClauseSatisfiedInComposition<
            InitializesAfter<TRequirement>,
            TProviderList
        > : std::bool_constant<
            TRequirement::Scope != RequirementScope::SameDomain ||
            SatisfyingProviderCountInList<
                TRequirement,
                TProviderList
            >::value == 1U
        > {};


        /// Evaluates one shutdown lifecycle target when it uses SameDomain scope.
        ///
        /// @tparam TProviderList Providers participating in the Composition.
        /// @tparam TRequirement Lifecycle target Requirement.
        template<class TProviderList, class TRequirement>
        struct ContractClauseSatisfiedInComposition<
            ShutsDownBefore<TRequirement>,
            TProviderList
        > : std::bool_constant<
            TRequirement::Scope != RequirementScope::SameDomain ||
            SatisfyingProviderCountInList<
                TRequirement,
                TProviderList
            >::value == 1U
        > {};


        /// Evaluates every clause in one consolidated provider Contract against a Composition.
        ///
        /// @tparam TContract Consolidated Contract.
        /// @tparam TProviderList Providers participating in the Composition.
        template<class TContract, class TProviderList>
        struct ContractSatisfiedInComposition : std::true_type {};


        /// Evaluates every clause in one concrete Contract.
        ///
        /// @tparam TProviderList Providers participating in the Composition.
        /// @tparam TClauses Contract clauses being evaluated.
        template<class TProviderList, class... TClauses>
        struct ContractSatisfiedInComposition<
            Contract<TClauses...>,
            TProviderList
        > : std::bool_constant<
            (
                ContractClauseSatisfiedInComposition<
                    TClauses,
                    TProviderList
                >::value &&
                ...
            )
        > {};


        /// Determines whether all offers in a Offers declaration obey exclusive-capability rules.
        template<class TOffersDeclaration, class TDomain, class... TProviders>
        struct OffersAreConflictFree;


        /// Checks every offered capability against the complete provider pack.
        template<class... TOffers, class TDomain, class... TProviders>
        struct OffersAreConflictFree<Offers<TOffers...>, TDomain, TProviders...> : std::bool_constant<
            ((
                !IsExclusiveCapabilityForV<TDomain, typename TOffers::CapabilityType> ||
                ProviderCountV<typename TOffers::CapabilityType, TProviders...> <= 1U
            ) && ...)
        > {};

    } // ESPressio::System::CompositionFramework::Detail


    /// Compile-time diagnostics describing whether one provider satisfies one Requirement qualification.
    ///
    /// @tparam TProvider Provider Type being inspected.
    /// @tparam TRequirement Requirement being evaluated.
    template<class TProvider, class TRequirement>
    struct ProviderMatch {

        static_assert(
            Detail::RequirementTraits<TRequirement>::IsValid,
            "ProviderMatch requires a consolidated Requirement"
        );

        // Match diagnostics.

        /// Indicates whether the provider supplies the requested Capability.
        static constexpr bool OffersCapability =
            Detail::ProviderOffersCapabilityV<
                TProvider,
                typename TRequirement::CapabilityType
            >;

        /// Indicates whether every qualification constraint is satisfied when the Capability is supplied.
        static constexpr bool ConstraintsSatisfied = []() constexpr {
            if constexpr (!OffersCapability) {
                return false;
            } else {
                using OfferType = typename TProvider::CompositionOffers::template OfferFor<
                    typename TRequirement::CapabilityType
                >;

                return TRequirement::template OfferSatisfied<OfferType>;
            }
        }();

        /// Indicates whether the provider satisfies the complete Requirement qualification.
        static constexpr bool IsSatisfied =
            OffersCapability &&
            ConstraintsSatisfied;

    };


    /// Concatenates two ProviderLists while preserving each list's declaration order.
    ///
    /// @tparam TLeftProviders Left ProviderList.
    /// @tparam TRightProviders Right ProviderList.
    template<class TLeftProviders, class TRightProviders>
    using ProviderListConcat = typename Detail::ConcatProviderLists<
        TLeftProviders,
        TRightProviders
    >::Type;

    /// Returns the declaration-order-preserving union of two ProviderLists.
    ///
    /// Providers already represented by the left list retain their positions; new providers from the
    /// right list are appended in right-list declaration order.
    ///
    /// @tparam TLeftProviders Left ProviderList controlling initial order.
    /// @tparam TRightProviders Right ProviderList contributing previously absent providers.
    template<class TLeftProviders, class TRightProviders>
    using ProviderListUnion = typename Detail::UnionProviderLists<
        TLeftProviders,
        TRightProviders
    >::Type;

    /// Filters one ProviderList through a compile-time unary predicate.
    ///
    /// @tparam TProviderList ProviderList being filtered.
    /// @tparam TPredicate Predicate class template exposing Boolean `value`.
    template<
        class TProviderList,
        template<class> class TPredicate
    >
    using ProviderListFilter = typename Detail::FilterProviderList<
        TProviderList,
        TPredicate
    >::Type;


    /// Returns the ordered intersection of two ProviderLists.
    ///
    /// @tparam TLeftProviders Left ProviderList controlling output order.
    /// @tparam TRightProviders Right ProviderList used for membership.
    template<class TLeftProviders, class TRightProviders>
    using ProviderListIntersection = typename Detail::IntersectProviderLists<
        TLeftProviders,
        TRightProviders
    >::Type;

    /// Returns provider Types present in the left ProviderList but absent from the right.
    ///
    /// @tparam TLeftProviders Left ProviderList controlling output order.
    /// @tparam TRightProviders Provider Types excluded from the result.
    template<class TLeftProviders, class TRightProviders>
    using ProviderListDifference = typename Detail::DifferenceProviderLists<
        TLeftProviders,
        TRightProviders
    >::Type;


    /// Validates and exposes the compile-time architecture formed by a set of providers within one domain.
    template<class TDomain, class... TProviders>
    struct Composition {

        static_assert(
            IsDomainV<TDomain>,
            "Composition requires a concrete composition Domain"
        );

        static_assert(
            (Detail::IsProviderDeclarationForDomain<TDomain, TProviders>::value && ...),
            "Composition contains a provider that is invalid or belongs to another domain"
        );

        // Internal composition validation.

        /// Indicates whether every ExclusiveCapability has no more than one provider.
        static constexpr bool NoCapabilityConflicts =
            (Detail::OffersAreConflictFree<typename TProviders::CompositionOffers, TDomain, TProviders...>::value && ...);

        /// Indicates whether every same-domain provider Contract clause is satisfied.
        static constexpr bool AllContractsSatisfied =
            (
                Detail::ContractSatisfiedInComposition<
                    typename TProviders::CompositionContract,
                    ProviderList<TProviders...>
                >::value &&
                ...
            );

        static_assert(
            NoCapabilityConflicts,
            "Composition contains multiple providers for an ExclusiveCapability"
        );

        static_assert(
            AllContractsSatisfied,
            "Composition contains an unsatisfied same-domain provider Contract"
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
        static constexpr bool IsValid =
            NoCapabilityConflicts &&
            AllContractsSatisfied;

        // Requirement queries.

        /// Validates one Requirement before provider matching is attempted.
        template<class TRequirement>
        struct ValidateRequirement {

            static_assert(
                Detail::RequirementTraits<TRequirement>::IsValid,
                "Composition provider queries require a consolidated Requirement"
            );

            static_assert(
                std::is_same_v<typename TRequirement::CompositionDomain, TDomain>,
                "Requirement Capability does not belong to this Composition Domain"
            );

            /// Indicates that the Requirement is valid for this Composition.
            static constexpr bool IsValid = true;

        };


        /// Returns every provider matching one Requirement qualification.
        ///
        /// @tparam TRequirement Requirement being queried.
        template<class TRequirement>
        using Matches = std::conditional_t<
            ValidateRequirement<TRequirement>::IsValid,
            typename Detail::FilterSatisfyingProviders<
                TRequirement,
                ProviderList<>,
                TProviders...
            >::Type,
            ProviderList<>
        >;

        /// Returns the number of providers matching one Requirement qualification.
        ///
        /// @tparam TRequirement Requirement being queried.
        template<class TRequirement>
        static constexpr std::size_t MatchCount =
            Matches<TRequirement>::Count;

        /// Indicates whether at least one provider matches one Requirement qualification.
        ///
        /// @tparam TRequirement Requirement being queried.
        template<class TRequirement>
        static constexpr bool HasMatch =
            MatchCount<TRequirement> > 0U;

        /// Indicates whether the matching provider population obeys one Requirement's cardinality.
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


        // Joint provider queries.

        /// Returns providers jointly satisfying every supplied Requirement.
        ///
        /// @tparam TRequirements Requirements which must all be satisfied by the same provider Type.
        template<class... TRequirements>
        using JointMatches = typename Detail::FilterJointlySatisfyingProviders<
            Detail::RequirementList<TRequirements...>,
            ProviderList<>,
            ProviderList<TProviders...>
        >::Type;

        /// Returns the number of providers jointly satisfying every supplied Requirement.
        ///
        /// @tparam TRequirements Requirements being jointly evaluated.
        template<class... TRequirements>
        static constexpr std::size_t JointMatchCount =
            JointMatches<TRequirements...>::Count;

        /// Resolves providers jointly satisfying every supplied Requirement using one explicit selection policy.
        ///
        /// @tparam TSelectionPolicy Explicit provider selection policy.
        /// @tparam TRequirements Requirements which must all be satisfied by the same provider Type.
        template<class TSelectionPolicy, class... TRequirements>
        using SelectJoint = typename Detail::ProviderListSelectionResult<
            TSelectionPolicy,
            JointMatches<TRequirements...>
        >::Type;


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
                TProvider::CompositionOffers::template Contains<TCapability>,
                "Property query provider does not supply the requested capability"
            );

            // Resolution result.

            /// Property set advertised by the provider for the requested capability.
            using Type = typename TProvider::CompositionOffers::template PropertiesFor<TCapability>;

        };


        /// Returns the property set advertised by a specific provider for a capability.
        template<class TProvider, class TCapability>
        using PropertiesForProvider = typename ResolveProviderProperties<TProvider, TCapability>::Type;

        /// Returns the Property set advertised by the unique provider for a capability.
        template<class TCapability>
        using PropertiesFor = PropertiesForProvider<
            Select<
                Requirement<
                    TCapability,
                    RequirementScope::SameDomain,
                    ExactlyProviders<1U>
                >,
                SelectUnique
            >,
            TCapability
        >;

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
                TProvider::CompositionOffers::template Contains<TCapability>,
                "Attribute query provider does not supply the requested capability"
            );

            // Resolution result.

            /// Attribute set advertised by the provider for the requested capability.
            using Type = typename TProvider::CompositionOffers::template AttributesFor<TCapability>;

        };


        /// Returns the Attribute set advertised by a specific provider for a capability.
        template<class TProvider, class TCapability>
        using AttributesForProvider = typename ResolveProviderAttributes<TProvider, TCapability>::Type;

        /// Returns the Attribute set advertised by the unique provider for a capability.
        template<class TCapability>
        using AttributesFor = AttributesForProvider<
            Select<
                Requirement<
                    TCapability,
                    RequirementScope::SameDomain,
                    ExactlyProviders<1U>
                >,
                SelectUnique
            >,
            TCapability
        >;

    };

} // ESPressio::System::CompositionFramework
