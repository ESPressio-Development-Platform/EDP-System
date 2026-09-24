#pragma once

#include <type_traits>

#include "TypeIdentifier.hpp"

namespace ESPressio::System {

    namespace Detail {

        /// Indicates whether a Type declares a directly readable static Identifier member.
        ///
        /// @tparam TType Type whose declaration is being inspected.
        template<class TType>
        concept HasTypeIdentifierMember = requires {
            TType::Identifier;
        };


        /// Determines whether a declared Identifier has the exact System TypeIdentifier domain.
        ///
        /// @tparam TType Type whose Identifier declaration is being inspected.
        template<class TType>
        consteval bool HasExactTypeIdentifier() {
            if constexpr (!HasTypeIdentifierMember<TType>) return false;

            return std::is_same_v<
                std::remove_cv_t<decltype(TType::Identifier)>,
                TypeIdentifier
            >;
        }

        /// Determines whether the exact Identifier declaration is a constant expression.
        ///
        /// @tparam TType Type whose Identifier declaration is being inspected.
        template<class TType>
        consteval bool HasConstantTypeIdentifier() {
            if constexpr (!HasExactTypeIdentifier<TType>()) return false;

            return requires {
                typename std::bool_constant<TType::Identifier.IsValid()>;
            };
        }

        /// Determines whether a Type satisfies the complete universal identification contract.
        ///
        /// @tparam TType Type whose universal identification contract is being inspected.
        template<class TType>
        consteval bool IsIdentifiedType() {
            if constexpr (!HasConstantTypeIdentifier<TType>()) return false;

            return TType::Identifier.IsValid();
        }

        /// Reads one universal Type identity while emitting focused contract diagnostics.
        ///
        /// @tparam TType Type whose universal identifier is being read.
        template<class TType>
        consteval TypeIdentifier ReadTypeIdentifier() {
            static_assert(
                HasTypeIdentifierMember<TType>,
                "Identified Types must declare a static Identifier member"
            );

            if constexpr (!HasTypeIdentifierMember<TType>) {
                return TypeIdentifier{};
            } else {
                static_assert(
                    HasExactTypeIdentifier<TType>(),
                    "Identified Type Identifier must have the exact System::TypeIdentifier Type"
                );

                if constexpr (!HasExactTypeIdentifier<TType>()) {
                    return TypeIdentifier{};
                } else {
                    static_assert(
                        HasConstantTypeIdentifier<TType>(),
                        "Identified Type Identifier must be a constant-expression declaration"
                    );

                    if constexpr (!HasConstantTypeIdentifier<TType>()) {
                        return TypeIdentifier{};
                    } else {
                        static_assert(
                            TType::Identifier.IsValid(),
                            "Identified Type Identifier must contain valid non-zero authority and local components"
                        );

                        return TType::Identifier;
                    }
                }
            }
        }

    } // ESPressio::System::Detail


    /// Predicate identifying Types which directly declare a valid universal TypeIdentifier.
    ///
    /// @tparam TType Semantic Type whose direct Identifier declaration is being validated.
    template<class TType>
    concept IdentifiedType = Detail::IsIdentifiedType<TType>();


    /// Canonical compile-time reader for a semantic Type's universal TypeIdentifier.
    ///
    /// Invalid declarations fail with focused diagnostics rather than becoming alternate metadata.
    ///
    /// @tparam TType Semantic Type whose direct universal Identifier declaration is being read.
    template<class TType>
    inline constexpr TypeIdentifier TypeIdentifierOf = Detail::ReadTypeIdentifier<TType>();

} // ESPressio::System
