#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace ESPressio::System::Identity {

    /// Permanent transport-independent 128-bit identity of one device.
    ///
    /// The value identifies the device itself. It is not a transport address,
    /// runtime incarnation, authentication credential, product classification,
    /// or subsystem-specific identifier. The all-zero representation is
    /// reserved as Invalid/Unspecified.
    class DeviceIdentifier final {
    private:

        // Identifier storage.

        /// Complete fixed-width identifier representation.
        std::array<std::uint8_t, 16U> _bytes{};

    public:

        // Identifier metadata.

        /// Identifier width in bytes.
        static constexpr std::size_t Size = 16U;

        /// Complete fixed-width identifier storage type.
        using Storage = std::array<std::uint8_t, Size>;


        // Construction.

        /// Creates the Invalid/Unspecified all-zero identifier.
        constexpr DeviceIdentifier() noexcept = default;

        /// Creates an identifier from its complete 128-bit representation.
        ///
        /// @param bytes Exact identifier bytes in canonical storage order.
        constexpr explicit DeviceIdentifier(const Storage& bytes) noexcept :
            _bytes(bytes) {}


        // Value access.

        /// Returns the complete immutable identifier representation without copying it.
        constexpr const Storage& Bytes() const noexcept {
            return _bytes;
        }

        /// Indicates whether the identifier is the reserved all-zero value.
        constexpr bool IsZero() const noexcept {
            for (const auto value : _bytes) {
                if (value != 0U) return false;
            }

            return true;
        }

        /// Indicates whether the identifier contains a valid non-zero device identity.
        constexpr bool IsValid() const noexcept {
            return !IsZero();
        }

        /// Indicates whether the identifier contains a valid non-zero device identity.
        constexpr explicit operator bool() const noexcept {
            return IsValid();
        }


        // Comparison.

        /// Compares identifiers by exact byte equality.
        ///
        /// @param other Identifier to compare with this value.
        constexpr bool operator ==(const DeviceIdentifier& other) const noexcept {
            for (std::size_t index = 0U; index < Size; ++index) {
                if (_bytes[index] != other._bytes[index]) return false;
            }

            return true;
        }

        /// Compares identifiers for inequality.
        ///
        /// @param other Identifier to compare with this value.
        constexpr bool operator !=(const DeviceIdentifier& other) const noexcept {
            return !(*this == other);
        }

        /// Provides deterministic lexicographic ordering by canonical identifier bytes.
        ///
        /// @param other Identifier to compare with this value.
        constexpr bool operator <(const DeviceIdentifier& other) const noexcept {
            for (std::size_t index = 0U; index < Size; ++index) {
                if (_bytes[index] < other._bytes[index]) return true;
                if (_bytes[index] > other._bytes[index]) return false;
            }

            return false;
        }

    };


    static_assert(
        sizeof(DeviceIdentifier) == DeviceIdentifier::Size,
        "DeviceIdentifier must remain an exact 16-byte semantic value"
    );

} // ESPressio::System::Identity
