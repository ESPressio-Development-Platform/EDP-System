#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace ESPressio::System {

    /// Strong exact-width identifier for one Type inside a governed Type authority.
    ///
    /// The identifier retains exactly five canonical bytes. Zero is reserved as
    /// Invalid/Unspecified within every authority and must never identify an allocated Type.
    class TypeLocalIdentifier final {
    private:

        // Canonical authority-local storage.

        /// Exact five-byte authority-local Type representation in canonical byte order.
        std::array<std::uint8_t, 5U> _bytes{};

    public:

        // Authority-local metadata.

        /// Exact authority-local Type width in bytes.
        static constexpr std::size_t Size = 5U;

        /// Complete fixed-width authority-local Type storage.
        using Storage = std::array<std::uint8_t, Size>;


        // Construction.

        /// Creates the Invalid/Unspecified all-zero authority-local Type identity.
        constexpr TypeLocalIdentifier() noexcept = default;

        /// Creates an authority-local Type identity from its complete canonical bytes.
        ///
        /// @param bytes Exact authority-local Type bytes in canonical order.
        constexpr explicit TypeLocalIdentifier(const Storage& bytes) noexcept :
            _bytes(bytes) {}


        // Value access.

        /// Returns the immutable canonical authority-local bytes without copying them.
        constexpr const Storage& Bytes() const noexcept {
            return _bytes;
        }

        /// Returns the 40-bit value in an explicit 64-bit host integer for tooling and diagnostics.
        constexpr std::uint64_t Value() const noexcept {
            return
                (static_cast<std::uint64_t>(_bytes[0U]) << 32U) |
                (static_cast<std::uint64_t>(_bytes[1U]) << 24U) |
                (static_cast<std::uint64_t>(_bytes[2U]) << 16U) |
                (static_cast<std::uint64_t>(_bytes[3U]) << 8U) |
                static_cast<std::uint64_t>(_bytes[4U]);
        }

        /// Indicates whether the authority-local Type identity is the reserved all-zero value.
        constexpr bool IsZero() const noexcept {
            return Value() == 0U;
        }

        /// Indicates whether the authority-local Type identity contains a valid non-zero value.
        constexpr bool IsValid() const noexcept {
            return !IsZero();
        }

        /// Indicates whether the authority-local Type identity contains a valid non-zero value.
        constexpr explicit operator bool() const noexcept {
            return IsValid();
        }


        // Comparison.

        /// Compares authority-local Type identities by exact canonical byte equality.
        ///
        /// @param other Authority-local Type identity to compare with this value.
        constexpr bool operator ==(const TypeLocalIdentifier& other) const noexcept {
            for (std::size_t index = 0U; index < Size; ++index) {
                if (_bytes[index] != other._bytes[index]) return false;
            }

            return true;
        }

        /// Compares authority-local Type identities for inequality.
        ///
        /// @param other Authority-local Type identity to compare with this value.
        constexpr bool operator !=(const TypeLocalIdentifier& other) const noexcept {
            return !(*this == other);
        }

        /// Provides deterministic lexicographic ordering by canonical authority-local bytes.
        ///
        /// @param other Authority-local Type identity to compare with this value.
        constexpr bool operator <(const TypeLocalIdentifier& other) const noexcept {
            for (std::size_t index = 0U; index < Size; ++index) {
                if (_bytes[index] < other._bytes[index]) return true;
                if (_bytes[index] > other._bytes[index]) return false;
            }

            return false;
        }

    };


    static_assert(
        sizeof(TypeLocalIdentifier) == TypeLocalIdentifier::Size,
        "TypeLocalIdentifier must remain an exact 5-byte semantic value"
    );

} // ESPressio::System
