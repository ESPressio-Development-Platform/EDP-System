#pragma once

#include <cstdint>
#include <type_traits>

#include "../flags/FlagSet.hpp"
#include "Property.hpp"

namespace ESPressio::System::CompositionFramework {

    /// Base declaration for a compile-time capability Property whose value is a finite set of typed flags.
    ///
    /// Flag values use the same zero-based bit-position contract as ESPressio::System::FlagSet.
    /// The Property's compile-time value remains the selected unsigned storage type so it integrates
    /// with the existing PropertyValue and PropertySet machinery without changing their contracts.
    ///
    /// @tparam TCapability Capability whose compile-time characteristics include this flag set.
    /// @tparam TFlag Enum type defining the finite flag domain and zero-based bit positions.
    /// @tparam TStorage Unsigned integral type used as the complete compile-time flag representation.
    template<class TCapability, class TFlag, class TStorage = std::uint32_t>
    struct FlagProperty : Property<TCapability, TStorage> {

        static_assert(
            std::is_enum_v<TFlag>,
            "FlagProperty requires an enum flag type"
        );

        static_assert(
            std::is_integral_v<TStorage> &&
            std::is_unsigned_v<TStorage> &&
            !std::is_same_v<std::remove_cv_t<TStorage>, bool>,
            "FlagProperty storage must be an unsigned integral type other than bool"
        );

        // Flag-property metadata.

        /// Enum type defining the Property's finite flag domain.
        using FlagType = TFlag;

        /// Unsigned integral type used to store the complete Property value.
        using StorageType = TStorage;

        /// Strongly typed flag-set type used to construct and inspect the Property value.
        using FlagSetType = ESPressio::System::FlagSet<TFlag, TStorage>;

    };


    namespace Detail {

        /// Default flag-property metadata for types that are not concrete FlagProperty declarations.
        ///
        /// @tparam TProperty Type being inspected for flag-property metadata.
        /// @tparam TEnable SFINAE helper used when the inspected type exposes flag-property aliases.
        template<class TProperty, class TEnable = void>
        struct FlagPropertyTraits {

            // Flag-property metadata.

            /// Capability associated with the inspected type when one exists.
            using CapabilityType = void;

            /// Flag enum associated with the inspected type when one exists.
            using FlagType = void;

            /// Storage type associated with the inspected type when one exists.
            using StorageType = void;

            /// Strongly typed flag-set type associated with the inspected type when one exists.
            using FlagSetType = void;

            /// Indicates whether the inspected type is a valid concrete FlagProperty.
            static constexpr bool IsValid = false;

        };


        /// Extracts flag-property metadata from a concrete type derived from FlagProperty.
        ///
        /// @tparam TProperty Type being inspected for flag-property metadata.
        template<class TProperty>
        struct FlagPropertyTraits<
            TProperty,
            std::void_t<
                typename TProperty::CapabilityType,
                typename TProperty::FlagType,
                typename TProperty::StorageType,
                typename TProperty::FlagSetType
            >
        > {

            // Flag-property metadata.

            /// Capability associated with the inspected flag property.
            using CapabilityType = typename TProperty::CapabilityType;

            /// Flag enum associated with the inspected flag property.
            using FlagType = typename TProperty::FlagType;

            /// Storage type associated with the inspected flag property.
            using StorageType = typename TProperty::StorageType;

            /// Strongly typed flag-set type associated with the inspected flag property.
            using FlagSetType = typename TProperty::FlagSetType;

            /// Indicates whether the inspected type is a concrete FlagProperty derived from the matching declaration.
            static constexpr bool IsValid =
                IsPropertyV<TProperty> &&
                std::is_base_of_v<
                    FlagProperty<
                        CapabilityType,
                        FlagType,
                        StorageType
                    >,
                    TProperty
                > &&
                !std::is_same_v<
                    FlagProperty<
                        CapabilityType,
                        FlagType,
                        StorageType
                    >,
                    TProperty
                >;

        };


        /// Indicates whether every supplied compile-time flag belongs to the flag enum declared by a Property.
        ///
        /// @tparam TProperty Concrete FlagProperty whose flag domain is being validated.
        /// @tparam TFlags Compile-time flag values being validated.
        template<class TProperty, auto... TFlags>
        consteval bool FlagArgumentsUsePropertyDomain() noexcept {
            if constexpr (!FlagPropertyTraits<TProperty>::IsValid) {
                return false;
            } else {
                return (
                    std::is_same_v<
                        std::remove_cv_t<decltype(TFlags)>,
                        typename FlagPropertyTraits<TProperty>::FlagType
                    > && ...
                );
            }
        }


        /// Indicates whether every supplied compile-time flag fits the storage selected by a Property.
        ///
        /// @tparam TProperty Concrete FlagProperty whose storage capacity is being validated.
        /// @tparam TFlags Compile-time flag values being validated.
        template<class TProperty, auto... TFlags>
        consteval bool FlagArgumentsAreRepresentable() noexcept {
            if constexpr (!FlagArgumentsUsePropertyDomain<TProperty, TFlags...>()) {
                return false;
            } else {
                using FlagSetType = typename FlagPropertyTraits<TProperty>::FlagSetType;

                return (
                    FlagSetType{}.IsRepresentable(
                        TFlags
                    ) && ...
                );
            }
        }


        /// Builds the raw compile-time storage value represented by a typed flag pack.
        ///
        /// @tparam TProperty Concrete FlagProperty whose storage value is being produced.
        /// @tparam TFlags Compile-time flags to set in the produced value.
        template<class TProperty, auto... TFlags>
        consteval auto FlagPropertyStorageValue() noexcept {
            static_assert(
                FlagPropertyTraits<TProperty>::IsValid,
                "FlagPropertyValue requires a concrete FlagProperty"
            );

            static_assert(
                FlagArgumentsUsePropertyDomain<TProperty, TFlags...>(),
                "FlagPropertyValue flags must belong to the Property's flag enum domain"
            );

            static_assert(
                FlagArgumentsAreRepresentable<TProperty, TFlags...>(),
                "FlagPropertyValue contains a flag position outside the Property's storage capacity"
            );

            using FlagSetType = typename FlagPropertyTraits<TProperty>::FlagSetType;

            FlagSetType flags;

            if constexpr (sizeof...(TFlags) > 0U) {
                flags.Set(
                    TFlags...
                );
            }

            return flags.Value();
        }

    } // ESPressio::System::CompositionFramework::Detail


    // Flag-property inspection.

    /// Indicates whether a type represents a concrete flag-valued Composition Property.
    ///
    /// @tparam TProperty Type being inspected.
    template<class TProperty>
    inline constexpr bool IsFlagPropertyV = Detail::FlagPropertyTraits<TProperty>::IsValid;


    /// Associates zero or more typed compile-time flags with a concrete FlagProperty.
    ///
    /// This declaration is compatible with the existing PropertyValue/PropertySet machinery while
    /// preventing callers from hand-maintaining raw masks for normal flag-property advertisement.
    ///
    /// @tparam TProperty Concrete FlagProperty receiving the advertised flags.
    /// @tparam TFlags Typed enum values to advertise as set; an empty pack advertises no flags.
    template<class TProperty, auto... TFlags>
    struct FlagPropertyValue : PropertyValue<
        TProperty,
        Detail::FlagPropertyStorageValue<TProperty, TFlags...>()
    > {

        static_assert(
            IsFlagPropertyV<TProperty>,
            "FlagPropertyValue requires a concrete FlagProperty"
        );

    };


    /// Requires every supplied flag to be present in one advertised FlagProperty value.
    ///
    /// @tparam TProperty Concrete FlagProperty evaluated by this constraint.
    /// @tparam TFlags One or more typed flags which must all be set.
    template<class TProperty, auto... TFlags>
    struct HasAllFlags {

        static_assert(
            IsFlagPropertyV<TProperty>,
            "HasAllFlags requires a concrete FlagProperty"
        );

        static_assert(
            sizeof...(TFlags) > 0U,
            "HasAllFlags requires at least one flag"
        );

        static_assert(
            Detail::FlagArgumentsUsePropertyDomain<TProperty, TFlags...>(),
            "HasAllFlags flags must belong to the Property's flag enum domain"
        );

        static_assert(
            Detail::FlagArgumentsAreRepresentable<TProperty, TFlags...>(),
            "HasAllFlags contains a flag position outside the Property's storage capacity"
        );

        // Constraint metadata.

        /// Marker used to identify this declaration as a Property constraint.
        using ConstraintTag = void;

        /// FlagProperty evaluated by this constraint.
        using PropertyType = TProperty;

        /// Capability to which the evaluated FlagProperty belongs.
        using CapabilityType = typename TProperty::CapabilityType;

        // Constraint evaluation.

        /// Determines whether the supplied PropertySet contains every required flag.
        ///
        /// @tparam TPropertySet PropertySet advertised by a candidate capability Offer.
        template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept {
            if constexpr (!TPropertySet::template Contains<TProperty>) {
                return false;
            } else {
                using FlagSetType = typename Detail::FlagPropertyTraits<TProperty>::FlagSetType;

                const FlagSetType flags(
                    TPropertySet::template Value<TProperty>
                );

                return flags.HasAll(
                    TFlags...
                );
            }
        }

    };


    /// Requires at least one supplied flag to be present in one advertised FlagProperty value.
    ///
    /// @tparam TProperty Concrete FlagProperty evaluated by this constraint.
    /// @tparam TFlags One or more typed flags of which at least one must be set.
    template<class TProperty, auto... TFlags>
    struct HasAnyFlags {

        static_assert(
            IsFlagPropertyV<TProperty>,
            "HasAnyFlags requires a concrete FlagProperty"
        );

        static_assert(
            sizeof...(TFlags) > 0U,
            "HasAnyFlags requires at least one flag"
        );

        static_assert(
            Detail::FlagArgumentsUsePropertyDomain<TProperty, TFlags...>(),
            "HasAnyFlags flags must belong to the Property's flag enum domain"
        );

        static_assert(
            Detail::FlagArgumentsAreRepresentable<TProperty, TFlags...>(),
            "HasAnyFlags contains a flag position outside the Property's storage capacity"
        );

        // Constraint metadata.

        /// Marker used to identify this declaration as a Property constraint.
        using ConstraintTag = void;

        /// FlagProperty evaluated by this constraint.
        using PropertyType = TProperty;

        /// Capability to which the evaluated FlagProperty belongs.
        using CapabilityType = typename TProperty::CapabilityType;

        // Constraint evaluation.

        /// Determines whether the supplied PropertySet contains at least one requested flag.
        ///
        /// @tparam TPropertySet PropertySet advertised by a candidate capability Offer.
        template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept {
            if constexpr (!TPropertySet::template Contains<TProperty>) {
                return false;
            } else {
                using FlagSetType = typename Detail::FlagPropertyTraits<TProperty>::FlagSetType;

                const FlagSetType flags(
                    TPropertySet::template Value<TProperty>
                );

                return flags.HasAny(
                    TFlags...
                );
            }
        }

    };


    /// Requires every supplied flag to be absent from one advertised FlagProperty value.
    ///
    /// @tparam TProperty Concrete FlagProperty evaluated by this constraint.
    /// @tparam TFlags One or more typed flags which must all be clear.
    template<class TProperty, auto... TFlags>
    struct HasNoFlags {

        static_assert(
            IsFlagPropertyV<TProperty>,
            "HasNoFlags requires a concrete FlagProperty"
        );

        static_assert(
            sizeof...(TFlags) > 0U,
            "HasNoFlags requires at least one flag"
        );

        static_assert(
            Detail::FlagArgumentsUsePropertyDomain<TProperty, TFlags...>(),
            "HasNoFlags flags must belong to the Property's flag enum domain"
        );

        static_assert(
            Detail::FlagArgumentsAreRepresentable<TProperty, TFlags...>(),
            "HasNoFlags contains a flag position outside the Property's storage capacity"
        );

        // Constraint metadata.

        /// Marker used to identify this declaration as a Property constraint.
        using ConstraintTag = void;

        /// FlagProperty evaluated by this constraint.
        using PropertyType = TProperty;

        /// Capability to which the evaluated FlagProperty belongs.
        using CapabilityType = typename TProperty::CapabilityType;

        // Constraint evaluation.

        /// Determines whether the supplied PropertySet contains none of the prohibited flags.
        ///
        /// @tparam TPropertySet PropertySet advertised by a candidate capability Offer.
        template<class TPropertySet>
        static constexpr bool IsSatisfied() noexcept {
            if constexpr (!TPropertySet::template Contains<TProperty>) {
                return false;
            } else {
                using FlagSetType = typename Detail::FlagPropertyTraits<TProperty>::FlagSetType;

                const FlagSetType flags(
                    TPropertySet::template Value<TProperty>
                );

                return !flags.HasAny(
                    TFlags...
                );
            }
        }

    };

} // ESPressio::System::CompositionFramework
