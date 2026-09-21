#pragma once

#include <cstddef>
#include <type_traits>

namespace ESPressio::System::CompositionFramework {

    /// Stores one structural compile-time string value suitable for C++20 non-type template parameters.
    template<std::size_t TSize>
    struct FixedString {

        // Compile-time string data.

        /// Character data including the terminating null character supplied by a string literal.
        char Characters[TSize];

        // Public construction.

        /// Copies a string literal into this structural compile-time value.
        constexpr FixedString(const char (&value)[TSize]) noexcept : Characters{} {
            for (std::size_t index = 0U; index < TSize; ++index) {
                Characters[index] = value[index];
            }
        }

        // Compile-time string metadata.

        /// Number of characters including the terminating null character.
        static constexpr std::size_t Size = TSize;

        /// Number of meaningful characters excluding the terminating null character.
        static constexpr std::size_t Length = TSize > 0U ? TSize - 1U : 0U;

    };


    /// Deduces the structural compile-time string size from a string literal.
    template<std::size_t TSize>
    FixedString(const char (&)[TSize]) -> FixedString<TSize>;


    /// Compares two structural compile-time strings for exact textual equality.
    template<std::size_t TLeftSize, std::size_t TRightSize>
    constexpr bool operator==(
        const FixedString<TLeftSize>& left,
        const FixedString<TRightSize>& right
    ) noexcept {
        if constexpr (TLeftSize != TRightSize) { return false; }

        for (std::size_t index = 0U; index < TLeftSize; ++index) {
            if (left.Characters[index] != right.Characters[index]) { return false; }
        }

        return true;
    }


    /// Compares two structural compile-time strings for textual inequality.
    template<std::size_t TLeftSize, std::size_t TRightSize>
    constexpr bool operator!=(
        const FixedString<TLeftSize>& left,
        const FixedString<TRightSize>& right
    ) noexcept {
        return !(left == right);
    }


    /// Wraps one compile-time string so it can be used as a generic non-type template value.
    template<FixedString TValue>
    struct TextValue {};


    /// Creates a generic compile-time text value usable wherever an auto non-type template parameter is accepted.
    template<FixedString TValue>
    inline constexpr TextValue<TValue> Text{};


    /// Compares two compile-time text wrappers for exact textual equality.
    template<FixedString TLeftValue, FixedString TRightValue>
    constexpr bool operator==(
        TextValue<TLeftValue>,
        TextValue<TRightValue>
    ) noexcept {
        return TLeftValue == TRightValue;
    }


    /// Compares two compile-time text wrappers for textual inequality.
    template<FixedString TLeftValue, FixedString TRightValue>
    constexpr bool operator!=(
        TextValue<TLeftValue> left,
        TextValue<TRightValue> right
    ) noexcept {
        return !(left == right);
    }


    /// Declares one open-ended compile-time descriptor attached to a capability Offer.
    template<FixedString TName, auto TValue>
    struct Attribute {

        // Attribute metadata.

        /// Marker used to identify Attribute declarations during compile-time inspection.
        using AttributeTag = void;

    };

    /// Convenience alias for an Attribute whose value is compile-time text.
    template<FixedString TName, FixedString TValue>
    using TextAttribute = Attribute<TName, Text<TValue>>;


    namespace Detail {

        /// Default attribute metadata for types that are not Attribute declarations.
        template<class TAttribute, class = void>
        struct AttributeTraits {

            // Attribute metadata.

            /// Indicates whether the inspected type is a valid Attribute declaration.
            static constexpr bool IsValid = false;

        };


        /// Extracts compile-time metadata from one concrete Attribute declaration.
        template<FixedString TName, auto TValue>
        struct AttributeTraits<
            Attribute<TName, TValue>,
            void
        > {

            // Attribute metadata.

            /// Indicates whether the inspected type is a valid Attribute declaration.
            static constexpr bool IsValid = true;

            /// Open-ended compile-time Attribute name.
            static constexpr auto Name = TName;

            /// Compile-time Attribute value used only by Composition matching.
            static constexpr auto Value = TValue;

        };


        /// Indicates whether two Attribute declarations use the same open-ended name.
        template<class TLeftAttribute, class TRightAttribute>
        inline constexpr bool SameAttributeNameV =
            AttributeTraits<TLeftAttribute>::Name == AttributeTraits<TRightAttribute>::Name;


        /// Indicates whether every Attribute name in one pack is unique.
        template<class... TAttributes>
        struct UniqueAttributes;


        /// Empty Attribute packs contain no duplicate names.
        template<>
        struct UniqueAttributes<> : std::true_type {};


        /// Checks the first Attribute name against the remaining declarations and continues recursively.
        template<class TFirstAttribute, class... TRestAttributes>
        struct UniqueAttributes<TFirstAttribute, TRestAttributes...> : std::bool_constant<
            ((!SameAttributeNameV<TFirstAttribute, TRestAttributes>) && ...) &&
            UniqueAttributes<TRestAttributes...>::value
        > {};


        /// Locates an Attribute by its open-ended name within a compile-time Attribute pack.
        template<FixedString TName, class... TAttributes>
        struct FindAttribute;


        /// Represents an unsuccessful Attribute lookup.
        template<FixedString TName>
        struct FindAttribute<TName> {

            // Lookup result.

            /// Type returned when no matching Attribute exists.
            using Type = void;

        };


        /// Continues an Attribute lookup until a declaration with the requested name is found.
        template<FixedString TName, class TFirstAttribute, class... TRestAttributes>
        struct FindAttribute<TName, TFirstAttribute, TRestAttributes...> {

            // Lookup result.

            /// Matching Attribute declaration, or the result of searching the remaining declarations.
            using Type = std::conditional_t<
                AttributeTraits<TFirstAttribute>::Name == TName,
                TFirstAttribute,
                typename FindAttribute<TName, TRestAttributes...>::Type
            >;

        };

    } // ESPressio::System::CompositionFramework::Detail


    // Attribute inspection.

    /// Indicates whether a type represents a valid open-ended compile-time Attribute.
    template<class TAttribute>
    inline constexpr bool IsAttributeV = Detail::AttributeTraits<TAttribute>::IsValid;


    /// Stores the compile-time Attributes advertised by one capability Offer.
    template<class... TAttributes>
    struct AttributeSet {

        static_assert(
            (IsAttributeV<TAttributes> && ...),
            "AttributeSet entries must be Attribute declarations"
        );

        static_assert(
            Detail::UniqueAttributes<TAttributes...>::value,
            "AttributeSet contains the same Attribute name more than once"
        );

        // Attribute-set metadata.

        /// Number of Attributes contained in this set.
        static constexpr std::size_t Count = sizeof...(TAttributes);

        // Attribute queries.

        /// Indicates whether this set contains an Attribute with the requested name.
        template<FixedString TName>
        static constexpr bool Contains = !std::is_void_v<typename Detail::FindAttribute<TName, TAttributes...>::Type>;

        /// Resolves one Attribute from this set by name.
        template<FixedString TName>
        struct Resolve {

            static_assert(
                Contains<TName>,
                "Requested Attribute is not present in this AttributeSet"
            );

            // Lookup result.

            /// Attribute declaration associated with the requested name.
            using Type = typename Detail::FindAttribute<TName, TAttributes...>::Type;

        };


        /// Indicates whether the requested Attribute exists and exactly matches the supplied compile-time value.
        template<FixedString TName, auto TExpectedValue>
        static constexpr bool Matches = []() constexpr {
            if constexpr (!Contains<TName>) {
                return false;
            } else {
                using AttributeType = typename Resolve<TName>::Type;
                constexpr auto actualValue = Detail::AttributeTraits<AttributeType>::Value;

                if constexpr (requires { actualValue == TExpectedValue; }) {
                    return actualValue == TExpectedValue;
                }

                return false;
            }
        }();

    };


    /// Requires an open-ended Attribute to equal the supplied compile-time value.
    template<FixedString TName, auto TExpectedValue>
    struct AttributeEquals {

        // Attribute-constraint metadata.

        /// Marker used to identify Attribute constraints during compile-time inspection.
        using AttributeConstraintTag = void;

        /// Required open-ended Attribute name.
        static constexpr auto Name = TName;

        /// Required compile-time Attribute value.
        static constexpr auto ExpectedValue = TExpectedValue;

        // Constraint evaluation.

        /// Determines whether the supplied Attribute set contains the required exact value.
        template<class TAttributeSet>
        static constexpr bool IsSatisfied() {
            return TAttributeSet::template Matches<TName, TExpectedValue>;
        }

    };

    /// Convenience alias requiring a compile-time text Attribute to equal the supplied text.
    template<FixedString TName, FixedString TExpectedValue>
    using TextAttributeEquals = AttributeEquals<TName, Text<TExpectedValue>>;


    namespace Detail {

        /// Default Attribute-constraint metadata for types that are not Attribute constraints.
        template<class TConstraint, class = void>
        struct AttributeConstraintTraits {

            // Constraint metadata.

            /// Indicates whether the inspected type is a valid Attribute constraint.
            static constexpr bool IsValid = false;

        };


        /// Extracts metadata from an Attribute constraint declaration.
        template<class TConstraint>
        struct AttributeConstraintTraits<
            TConstraint,
            std::void_t<typename TConstraint::AttributeConstraintTag>
        > {

            // Constraint metadata.

            /// Indicates whether the inspected type is a valid Attribute constraint.
            static constexpr bool IsValid = true;

        };

    } // ESPressio::System::CompositionFramework::Detail


    /// Indicates whether a type represents a valid open-ended Attribute constraint.
    template<class TConstraint>
    inline constexpr bool IsAttributeConstraintV = Detail::AttributeConstraintTraits<TConstraint>::IsValid;

} // ESPressio::System::CompositionFramework
