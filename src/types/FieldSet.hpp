#pragma once

#include <cstddef>
#include <type_traits>

#include "FieldBinding.hpp"

namespace ESPressio::System {

    namespace Detail {

        /// Indicates whether one Type is an exact System FieldBinding specialization.
        ///
        /// @tparam TType Candidate binding Type.
        template<class TType>
        struct IsFieldBinding final : std::false_type {};


        /// Recognizes exact System FieldBinding specializations.
        ///
        /// @tparam TMember Pointer to the represented non-static data member.
        /// @tparam TIdentifier Stable Type-local Field identity.
        template<auto TMember, FieldIdentifier::Storage TIdentifier>
        struct IsFieldBinding<FieldBinding<TMember, TIdentifier>> final : std::true_type {};


        /// Compile-time token representing one concrete C++ member independently of its Field identity.
        ///
        /// @tparam TMember Pointer to the represented non-static data member.
        template<auto TMember>
        struct FieldMemberToken final {};


        /// Compile-time token representing one numeric Field identity independently of its member.
        ///
        /// @tparam TIdentifier Stable Type-local Field identity.
        template<FieldIdentifier::Storage TIdentifier>
        struct FieldIdentifierToken final {};


        /// Determines whether a Type pack contains no duplicate Types.
        ///
        /// @tparam TTypes Types being checked for uniqueness.
        template<class... TTypes>
        struct AreUniqueTypes;


        /// Empty Type packs contain no duplicates.
        template<>
        struct AreUniqueTypes<> final : std::true_type {};


        /// Recursively validates that the first Type differs from every remaining Type and that the remainder is unique.
        ///
        /// @tparam TFirst First Type in the candidate pack.
        /// @tparam TRest Remaining Types in the candidate pack.
        template<class TFirst, class... TRest>
        struct AreUniqueTypes<TFirst, TRest...> final : std::bool_constant<
            (!std::is_same_v<TFirst, TRest> && ...) &&
            AreUniqueTypes<TRest...>::value
        > {};


        /// Selects the common owning Type represented by one FieldSet.
        ///
        /// Empty FieldSets deliberately expose void because no member exists from which an owner can be inferred.
        ///
        /// @tparam TFields Field bindings represented by the set.
        template<class... TFields>
        struct FieldSetOwner final {

            /// No owning Type can be inferred from an empty FieldSet.
            using Type = void;

        };


        /// Selects the owner from the first Field binding in a non-empty set.
        ///
        /// @tparam TFirst First Field binding.
        /// @tparam TRest Remaining Field bindings.
        template<class TFirst, class... TRest>
        struct FieldSetOwner<TFirst, TRest...> final {

            /// Owning C++ semantic Type inferred from the first binding.
            using Type = typename TFirst::Owner;

        };

    } // ESPressio::System::Detail


    /// Canonical compile-time enumerable Field schema for one semantic Type.
    ///
    /// @tparam TFields FieldBinding Types belonging to exactly one owning semantic Type.
    template<class... TFields>
    struct FieldSet final {
    private:

        // Common owner inferred from the represented bindings.

        /// Owning Type inferred from the first binding, or void for an empty FieldSet.
        using InferredOwner = typename Detail::FieldSetOwner<TFields...>::Type;

    public:

        static_assert(
            (Detail::IsFieldBinding<TFields>::value && ...),
            "FieldSet entries must be exact System::FieldBinding Types"
        );

        static_assert(
            (std::is_same_v<typename TFields::Owner, InferredOwner> && ...),
            "FieldSet bindings must all belong to the same owning Type"
        );

        static_assert(
            Detail::AreUniqueTypes<
                Detail::FieldIdentifierToken<TFields::Identifier.Value()>...
            >::value,
            "FieldSet must not contain duplicate FieldIdentifier values"
        );

        static_assert(
            Detail::AreUniqueTypes<
                Detail::FieldMemberToken<TFields::Member>...
            >::value,
            "FieldSet must not bind the same data member more than once"
        );


        // Field schema metadata.

        /// Common owning Type for non-empty sets, or void for an empty FieldSet.
        using Owner = InferredOwner;

        /// Number of Fields represented by this schema.
        static constexpr std::size_t Count = sizeof...(TFields);


        // Compile-time enumeration.

        /// Invokes a templated callable once for each FieldBinding in declaration order.
        ///
        /// @tparam TCallable Callable exposing a templated nullary operator for each FieldBinding Type.
        /// @param callable Callable instance used for compile-time or runtime schema traversal.
        template<class TCallable>
        static constexpr void ForEach(TCallable&& callable) {
            (callable.template operator()<TFields>(), ...);
        }

    };

} // ESPressio::System
