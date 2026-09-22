#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace ESPressio::System {

    /// Describes one explicit desired state for a strongly typed flag.
    ///
    /// @tparam TFlag Enum type defining the flag domain.
    template<class TFlag>
    class FlagState final {
    private:

        static_assert(
            std::is_enum_v<TFlag>,
            "FlagState requires an enum flag type"
        );

        // Explicit flag state.

        /// Flag whose desired state is represented.
        TFlag _flag;

        /// Desired set/clear state of the flag.
        bool _isSet;

    public:

        // Construction.

        /// Creates an explicit state assignment for one flag.
        ///
        /// @param flag Flag whose state is being described.
        /// @param isSet True when the flag should be set; false when it should be clear.
        constexpr FlagState(
            TFlag flag,
            bool isSet
        ) noexcept :
            _flag(flag),
            _isSet(isSet) {}


        // State access.

        /// Returns the flag whose state is represented.
        constexpr TFlag Flag() const noexcept {
            return _flag;
        }

        /// Indicates whether the represented flag should be set.
        constexpr bool IsSet() const noexcept {
            return _isSet;
        }

    };


    /// Compact strongly typed set of enum-domain flags backed by one unsigned integer.
    ///
    /// Enum values represent zero-based bit positions in the selected storage type.
    /// Values outside the storage capacity are not representable and never mutate storage.
    ///
    /// @tparam TFlag Enum type defining the flag domain and zero-based bit positions.
    /// @tparam TStorage Unsigned integral type used as the complete persistent representation.
    template<class TFlag, class TStorage = std::uint32_t>
    class FlagSet final {
    private:

        static_assert(
            std::is_enum_v<TFlag>,
            "FlagSet requires an enum flag type"
        );

        static_assert(
            std::is_integral_v<TStorage> &&
            std::is_unsigned_v<TStorage> &&
            !std::is_same_v<std::remove_cv_t<TStorage>, bool>,
            "FlagSet storage must be an unsigned integral type other than bool"
        );

        // Internal type metadata.

        /// Underlying integer type used by the flag enum.
        using FlagUnderlying = std::underlying_type_t<TFlag>;

        /// Unsigned form of the enum's underlying integer type.
        using UnsignedFlagUnderlying = std::make_unsigned_t<FlagUnderlying>;

        /// Explicit desired state representation used by state-aware construction.
        using StateType = FlagState<TFlag>;

        // Storage metadata.

        /// Number of independently representable flag positions.
        static constexpr std::size_t StorageBitCount = sizeof(TStorage) * 8U;

        // Flag storage.

        /// Complete bit representation of all current flag states.
        TStorage _bits{0U};


        // Flag validation.

        /// Indicates whether a flag's bit position can be represented by the selected storage type.
        ///
        /// @param flag Flag whose bit position should be validated.
        static constexpr bool IsFlagRepresentable(TFlag flag) noexcept {
            const auto position = static_cast<FlagUnderlying>(flag);

            if constexpr (std::is_signed_v<FlagUnderlying>) {
                if (position < 0) return false;
            }

            const auto unsignedPosition = static_cast<UnsignedFlagUnderlying>(position);

            return static_cast<std::uintmax_t>(unsignedPosition) < static_cast<std::uintmax_t>(StorageBitCount);
        }

        /// Indicates whether every supplied flag can be represented by the selected storage type.
        ///
        /// @tparam TAdditionalFlags Additional flag argument types, each required to match TFlag.
        /// @param flag First flag whose bit position should be validated.
        /// @param additionalFlags Additional flags whose bit positions should be validated.
        template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        static constexpr bool AreFlagsRepresentable(
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) noexcept {
            return IsFlagRepresentable(flag) && (IsFlagRepresentable(additionalFlags) && ...);
        }


        // Mask construction.

        /// Returns the single-bit mask for one representable flag, or zero when it cannot be represented.
        ///
        /// @param flag Flag whose bit mask should be produced.
        static constexpr TStorage Mask(TFlag flag) noexcept {
            if (!IsFlagRepresentable(flag)) return static_cast<TStorage>(0U);

            const auto position = static_cast<UnsignedFlagUnderlying>(static_cast<FlagUnderlying>(flag));

            return static_cast<TStorage>(
                static_cast<TStorage>(1U) << static_cast<std::size_t>(position)
            );
        }

        /// Returns the combined mask for one or more flags.
        ///
        /// @tparam TAdditionalFlags Additional flag argument types, each required to match TFlag.
        /// @param flag First flag whose bit should be represented in the mask.
        /// @param additionalFlags Additional flags whose bits should be represented in the mask.
        template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        static constexpr TStorage Mask(
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) noexcept {
            TStorage result = Mask(flag);

            ((result = static_cast<TStorage>(result | Mask(additionalFlags))), ...);

            return result;
        }


        // Explicit-state application.

        /// Applies one explicit flag state to the current representation.
        ///
        /// @param state Explicit state to apply.
        constexpr void ApplyState(const StateType& state) noexcept {
            Assign(
                state.IsSet(),
                state.Flag()
            );
        }

    public:

        // Public type metadata.

        /// Enum type defining the flag domain.
        using Flag = TFlag;

        /// Unsigned integer type used as the complete persistent representation.
        using Storage = TStorage;

        /// Explicit per-flag state type accepted by state-aware constructors.
        using State = StateType;

        /// Number of independently representable flag positions.
        static constexpr std::size_t Capacity = StorageBitCount;


        // Construction.

        /// Creates an empty flag set with every flag clear.
        constexpr FlagSet() noexcept = default;

        /// Creates a flag set from its complete raw storage representation.
        ///
        /// @param value Complete bit representation to preserve exactly.
        constexpr explicit FlagSet(Storage value) noexcept :
            _bits(value) {}

        /// Creates a flag set with the supplied flags initially set.
        ///
        /// Unrepresentable values are ignored so no invalid shift can occur.
        ///
        /// @tparam TAdditionalFlags Additional flag argument types, each required to match TFlag.
        /// @param flag First flag to set.
        /// @param additionalFlags Additional flags to set.
        template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        constexpr explicit FlagSet(
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) noexcept :
            _bits(Mask(flag, additionalFlags...)) {}

        /// Creates a zero-based flag set and applies explicit state assignments from left to right.
        ///
        /// Later assignments to the same flag override earlier assignments.
        /// Unrepresentable values are ignored.
        ///
        /// @tparam TAdditionalStates Additional state argument types, each required to match State.
        /// @param state First explicit state assignment.
        /// @param additionalStates Additional explicit state assignments.
        template<class... TAdditionalStates>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalStates>, State>) && ...)
        constexpr explicit FlagSet(
            State state,
            TAdditionalStates... additionalStates
        ) noexcept {
            ApplyState(state);
            (ApplyState(additionalStates), ...);
        }

        /// Creates a flag set from raw storage and then applies explicit state overrides from left to right.
        ///
        /// Later assignments to the same flag override earlier assignments.
        /// Unrepresentable values are ignored.
        ///
        /// @tparam TAdditionalStates Additional state argument types, each required to match State.
        /// @param value Initial complete bit representation.
        /// @param state First explicit state override.
        /// @param additionalStates Additional explicit state overrides.
        template<class... TAdditionalStates>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalStates>, State>) && ...)
        constexpr explicit FlagSet(
            Storage value,
            State state,
            TAdditionalStates... additionalStates
        ) noexcept :
            _bits(value) {
            ApplyState(state);
            (ApplyState(additionalStates), ...);
        }


        // Representation access.

        /// Returns the complete raw storage representation.
        constexpr Storage Value() const noexcept {
            return _bits;
        }

        /// Indicates whether no represented flags are currently set.
        constexpr bool IsEmpty() const noexcept {
            return _bits == static_cast<Storage>(0U);
        }

        /// Indicates whether a flag's bit position can be represented by this flag-set type.
        ///
        /// @param flag Flag whose position should be checked.
        constexpr bool IsRepresentable(TFlag flag) const noexcept {
            return IsFlagRepresentable(flag);
        }


        // Flag mutation.

        /// Clears every represented flag.
        constexpr void Clear() noexcept {
            _bits = static_cast<Storage>(0U);
        }

        /// Sets one or more flags while preserving every other represented flag.
        ///
        /// Unrepresentable values are ignored so no invalid shift can occur.
        ///
        /// @tparam TAdditionalFlags Additional flag argument types, each required to match TFlag.
        /// @param flag First flag to set.
        /// @param additionalFlags Additional flags to set.
        template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        constexpr void Set(
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) noexcept {
            _bits = static_cast<Storage>(_bits | Mask(flag, additionalFlags...));
        }

        /// Clears one or more flags while preserving every other represented flag.
        ///
        /// Unrepresentable values are ignored so no invalid shift can occur.
        ///
        /// @tparam TAdditionalFlags Additional flag argument types, each required to match TFlag.
        /// @param flag First flag to clear.
        /// @param additionalFlags Additional flags to clear.
        template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        constexpr void Clear(
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) noexcept {
            const auto mask = Mask(flag, additionalFlags...);
            _bits = static_cast<Storage>(_bits & static_cast<Storage>(~mask));
        }

        /// Flips one or more flags while preserving every other represented flag.
        ///
        /// Unrepresentable values are ignored so no invalid shift can occur.
        ///
        /// @tparam TAdditionalFlags Additional flag argument types, each required to match TFlag.
        /// @param flag First flag to flip.
        /// @param additionalFlags Additional flags to flip.
        template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        constexpr void Flip(
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) noexcept {
            _bits = static_cast<Storage>(_bits ^ Mask(flag, additionalFlags...));
        }

        /// Assigns one common set/clear state to one or more flags.
        ///
        /// Unrepresentable values are ignored so no invalid shift can occur.
        ///
        /// @tparam TAdditionalFlags Additional flag argument types, each required to match TFlag.
        /// @param isSet True to set the supplied flags; false to clear them.
        /// @param flag First flag to assign.
        /// @param additionalFlags Additional flags to assign.
        template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        constexpr void Assign(
            bool isSet,
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) noexcept {
            if (isSet) {
                Set(flag, additionalFlags...);
            } else {
                Clear(flag, additionalFlags...);
            }
        }

        /// Assigns selected raw bits from a state value while preserving bits outside the mask.
        ///
        /// @param mask Raw bit mask selecting the bits to replace.
        /// @param state Raw state whose selected bits should be assigned.
        constexpr void AssignMask(
            Storage mask,
            Storage state
        ) noexcept {
            _bits = static_cast<Storage>(
                static_cast<Storage>(_bits & static_cast<Storage>(~mask)) |
                static_cast<Storage>(state & mask)
            );
        }


        // Flag predicates.

        /// Indicates whether one representable flag is currently set.
        ///
        /// @param flag Flag whose current state should be inspected.
        constexpr bool IsSet(TFlag flag) const noexcept {
            const auto mask = Mask(flag);

            return mask != static_cast<Storage>(0U) && (_bits & mask) != static_cast<Storage>(0U);
        }

        /// Indicates whether at least one of the supplied representable flags is currently set.
        ///
        /// Unrepresentable values do not contribute a represented bit to the predicate.
        ///
        /// @tparam TAdditionalFlags Additional flag argument types, each required to match TFlag.
        /// @param flag First flag to inspect.
        /// @param additionalFlags Additional flags to inspect.
        template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        constexpr bool HasAny(
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) const noexcept {
            const auto mask = Mask(flag, additionalFlags...);

            return mask != static_cast<Storage>(0U) && (_bits & mask) != static_cast<Storage>(0U);
        }

        /// Indicates whether every supplied flag is representable and currently set.
        ///
        /// @tparam TAdditionalFlags Additional flag argument types, each required to match TFlag.
        /// @param flag First flag to inspect.
        /// @param additionalFlags Additional flags to inspect.
        template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        constexpr bool HasAll(
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) const noexcept {
            if (!AreFlagsRepresentable(flag, additionalFlags...)) return false;

            const auto mask = Mask(flag, additionalFlags...);

            return (_bits & mask) == mask;
        }

    };

} // ESPressio::System
