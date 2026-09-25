#pragma once

#include <type_traits>

#include "FieldSet.hpp"
#include "IdentifiedType.hpp"

namespace ESPressio::System {

    namespace Detail {

        /// Indicates whether one Type is an exact System FieldSet specialization.
        ///
        /// @tparam TType Candidate Field schema Type.
        template<class TType>
        struct IsFieldSet final : std::false_type {};


        /// Recognizes exact System FieldSet specializations.
        ///
        /// @tparam TFields Field bindings represented by the set.
        template<class... TFields>
        struct IsFieldSet<FieldSet<TFields...>> final : std::true_type {};


        /// Indicates whether a semantic Type declares a canonical nested Fields schema Type.
        ///
        /// @tparam TType Semantic Type being inspected.
        template<class TType>
        concept HasFieldsMember = requires {
            typename TType::Fields;
        };


        /// Determines whether a semantic Type satisfies the complete System schema contract.
        ///
        /// @tparam TType Semantic Type being inspected.
        template<class TType>
        consteval bool IsSchemaType() {
            if constexpr (!IdentifiedType<TType>) {
                return false;
            } else if constexpr (!HasFieldsMember<TType>) {
                return false;
            } else if constexpr (!IsFieldSet<typename TType::Fields>::value) {
                return false;
            } else {
                using Fields = typename TType::Fields;

                if constexpr (Fields::Count == 0U) {
                    return true;
                } else {
                    return std::is_same_v<typename Fields::Owner, TType>;
                }
            }
        }


        /// Reads one semantic Type's canonical Fields schema with focused diagnostics.
        ///
        /// @tparam TType Semantic schema Type whose Fields declaration is being read.
        template<class TType>
        struct ReadFields final {

            static_assert(
                IdentifiedType<TType>,
                "Schema Types must satisfy System::IdentifiedType"
            );

            static_assert(
                HasFieldsMember<TType>,
                "Schema Types must declare a nested Fields Type"
            );

            /// Canonical FieldSet declared directly by the semantic Type.
            using Type = typename TType::Fields;

            static_assert(
                IsFieldSet<Type>::value,
                "Schema Type Fields must be an exact System::FieldSet specialization"
            );

            static_assert(
                !IsFieldSet<Type>::value ||
                Type::Count == 0U ||
                std::is_same_v<typename Type::Owner, TType>,
                "Schema Type Fields must bind members owned by that exact semantic Type"
            );

        };


        /// Validates one candidate FieldBinding before exposing common readers.
        ///
        /// @tparam TField Candidate FieldBinding Type.
        template<class TField>
        struct ReadFieldBinding final {

            static_assert(
                IsFieldBinding<TField>::value,
                "Field metadata readers require an exact System::FieldBinding Type"
            );

            /// Validated FieldBinding Type.
            using Type = TField;

        };

    } // ESPressio::System::Detail


    /// Predicate identifying universally identified semantic Types with one valid canonical FieldSet.
    ///
    /// @tparam TType Semantic Type whose schema contract is being validated.
    template<class TType>
    concept SchemaType = Detail::IsSchemaType<TType>();


    /// Canonical compile-time reader for one schema Type's FieldSet.
    ///
    /// @tparam TType Semantic schema Type whose Fields are requested.
    template<class TType>
    using FieldsOf = typename Detail::ReadFields<TType>::Type;


    /// Canonical compile-time reader for one FieldBinding's strong Field identity.
    ///
    /// @tparam TField FieldBinding whose Type-local identity is requested.
    template<class TField>
    inline constexpr FieldIdentifier FieldIdentifierOf = Detail::ReadFieldBinding<TField>::Type::Identifier;


    /// Canonical owning Type reader for one FieldBinding.
    ///
    /// @tparam TField FieldBinding whose owning semantic Type is requested.
    template<class TField>
    using FieldOwnerOf = typename Detail::ReadFieldBinding<TField>::Type::Owner;


    /// Canonical member value Type reader for one FieldBinding.
    ///
    /// @tparam TField FieldBinding whose represented value Type is requested.
    template<class TField>
    using FieldValueOf = typename Detail::ReadFieldBinding<TField>::Type::Value;


    /// Enumerates every FieldBinding declared by one SchemaType in canonical declaration order.
    ///
    /// @tparam TType Semantic SchemaType whose Fields are traversed.
    /// @tparam TCallable Callable exposing a templated nullary operator for each FieldBinding Type.
    /// @param callable Callable instance used for traversal.
    template<class TType, class TCallable>
    requires SchemaType<TType>
    constexpr void ForEachField(TCallable&& callable) {
        FieldsOf<TType>::ForEach(callable);
    }

} // ESPressio::System
