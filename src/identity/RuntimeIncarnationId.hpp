#pragma once

#include <cstdint>

namespace ESPressio::System::Identity {

    /// Durable 32-bit identity of one running incarnation of a device.
    ///
    /// Zero is Invalid/Unspecified. A RuntimeIncarnationIdentity provider is
    /// responsible for durably advancing this value exactly once for each new
    /// application runtime before the value is exposed to SystemIdentity.
    class RuntimeIncarnationId final {
    private:

        // Incarnation storage.

        /// Exact durable incarnation scalar.
        std::uint32_t _value{};

    public:

        // Construction.

        /// Creates the Invalid/Unspecified zero incarnation.
        constexpr RuntimeIncarnationId() noexcept = default;

        /// Creates an incarnation from a provider-established durable value.
        ///
        /// @param value Non-zero durable incarnation scalar.
        constexpr explicit RuntimeIncarnationId(std::uint32_t value) noexcept :
            _value(value) {}


        // Value access.

        /// Returns the exact incarnation scalar.
        constexpr std::uint32_t Value() const noexcept {
            return _value;
        }

        /// Indicates whether this incarnation contains a valid non-zero value.
        constexpr bool IsValid() const noexcept {
            return _value != 0U;
        }

        /// Indicates whether this incarnation contains a valid non-zero value.
        constexpr explicit operator bool() const noexcept {
            return IsValid();
        }


        // Comparison.

        /// Compares incarnation values for equality.
        ///
        /// @param other Incarnation to compare with this value.
        constexpr bool operator ==(const RuntimeIncarnationId& other) const noexcept {
            return _value == other._value;
        }

        /// Compares incarnation values for inequality.
        ///
        /// @param other Incarnation to compare with this value.
        constexpr bool operator !=(const RuntimeIncarnationId& other) const noexcept {
            return !(*this == other);
        }

        /// Provides deterministic ordering by the durable incarnation scalar.
        ///
        /// @param other Incarnation to compare with this value.
        constexpr bool operator <(const RuntimeIncarnationId& other) const noexcept {
            return _value < other._value;
        }

    };


    static_assert(
        sizeof(RuntimeIncarnationId) == sizeof(std::uint32_t),
        "RuntimeIncarnationId must remain an exact 4-byte semantic value"
    );

} // ESPressio::System::Identity
