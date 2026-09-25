#pragma once

#include <cstddef>
#include <cstdint>

namespace ESPressio::System {

    /// Stable numeric identity of one Field within an owning semantic Type.
    ///
    /// Field identity is local to a TypeIdentifier. Every value from 0 through 255 is valid,
    /// so default construction is deliberately unavailable and zero is not a sentinel.
    class FieldIdentifier final {
    private:

        // Retained identity state.

        /// Exact one-byte Field identity value.
        std::uint8_t _value;

    public:

        // Field identity metadata.

        /// Exact Field identity width in bytes.
        static constexpr std::size_t Size = 1U;

        /// Exact unsigned storage Type used by Field identifiers.
        using Storage = std::uint8_t;


        // Construction.

        /// Prevents accidental manufacture of Field zero when no explicit identity was supplied.
        FieldIdentifier() = delete;

        /// Creates a Field identity from its exact one-byte numeric value.
        ///
        /// @param value Type-local Field identity in the complete valid range 0..255.
        constexpr explicit FieldIdentifier(Storage value) noexcept :
            _value(value) {}


        // Value access.

        /// Returns the exact Type-local numeric Field identity.
        [[nodiscard]] constexpr Storage Value() const noexcept {
            return _value;
        }


        // Comparison.

        /// Compares two Type-local Field identities for exact numeric equality.
        [[nodiscard]] constexpr bool operator ==(const FieldIdentifier& other) const noexcept = default;

        /// Compares two Type-local Field identities for inequality.
        [[nodiscard]] constexpr bool operator !=(const FieldIdentifier& other) const noexcept {
            return !(*this == other);
        }

        /// Provides deterministic ascending numeric ordering for Field identities.
        [[nodiscard]] constexpr bool operator <(const FieldIdentifier& other) const noexcept {
            return _value < other._value;
        }

    };


    static_assert(
        sizeof(FieldIdentifier) == FieldIdentifier::Size,
        "FieldIdentifier must remain an exact one-byte semantic value"
    );

} // ESPressio::System
