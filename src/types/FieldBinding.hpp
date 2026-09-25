#pragma once

#include <type_traits>

#include "FieldIdentifier.hpp"

namespace ESPressio::System {

    namespace Detail {

        /// Fallback traits for values which are not pointers to non-static data members.
        ///
        /// @tparam TPointer Candidate pointer Type being inspected.
        template<class TPointer>
        struct MemberObjectPointerTraits final {

            /// Indicates that the candidate is not a valid data-member pointer.
            static constexpr bool IsValid = false;

            /// Placeholder owner Type used only to keep diagnostics well-formed.
            using Owner = void;

            /// Placeholder member value Type used only to keep diagnostics well-formed.
            using Value = void;

        };


        /// Extracts owning and value Types from one pointer to a non-static data member.
        ///
        /// @tparam TValue Value Type stored by the represented data member.
        /// @tparam TOwner C++ Type which owns the represented data member.
        template<class TValue, class TOwner>
        struct MemberObjectPointerTraits<TValue TOwner::*> final {

            /// Indicates that the inspected pointer is a valid data-member pointer.
            static constexpr bool IsValid = true;

            /// C++ Type which owns the represented data member.
            using Owner = TOwner;

            /// Value Type stored by the represented data member.
            using Value = TValue;

        };

    } // ESPressio::System::Detail


    /// Compile-time association between one concrete C++ data member and its stable Type-local Field identity.
    ///
    /// @tparam TMember Pointer to the represented non-static data member.
    /// @tparam TIdentifier Stable one-byte Field identity allocated within the owning Type.
    template<auto TMember, FieldIdentifier::Storage TIdentifier>
    struct FieldBinding final {
    private:

        // Member-pointer traits used to expose strongly typed schema metadata.

        /// Canonical member-pointer traits for the represented member.
        using MemberTraits = Detail::MemberObjectPointerTraits<decltype(TMember)>;

    public:

        static_assert(
            MemberTraits::IsValid,
            "FieldBinding requires a pointer to a non-static data member"
        );


        // Bound schema metadata.

        /// C++ Type which owns the represented data member.
        using Owner = typename MemberTraits::Owner;

        /// Value Type stored by the represented data member.
        using Value = typename MemberTraits::Value;

        /// Pointer to the represented non-static data member.
        inline static constexpr auto Member = TMember;

        /// Stable numeric identity of the represented Field within Owner.
        inline static constexpr FieldIdentifier Identifier{TIdentifier};

    };

} // ESPressio::System
