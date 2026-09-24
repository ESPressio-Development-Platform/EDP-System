#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace ESPressio::System {

    /// Strong exact-width identifier for one globally governed Type authority.
    ///
    /// The identifier retains exactly three canonical bytes. Zero is reserved as
    /// Invalid/Unspecified and must never identify an allocated authority.
    class TypeAuthorityIdentifier final {
    private:

        // Canonical authority storage.

        /// Exact three-byte authority representation in canonical byte order.
        std::array<std::uint8_t, 3U> _bytes{};

    public:

        // Authority metadata.

        /// Exact authority width in bytes.
        static constexpr std::size_t Size = 3U;

        /// Complete fixed-width authority storage Type.
        using Storage = std::array<std::uint8_t, Size>;


        // Construction.

        /// Creates the Invalid/Unspecified all-zero authority.
        constexpr TypeAuthorityIdentifier() noexcept = default;

        /// Creates an authority from its complete canonical byte representation.
        ///
        /// @param bytes Exact authority bytes in canonical order.
        constexpr explicit TypeAuthorityIdentifier(const Storage& bytes) noexcept :
            _bytes(bytes) {}


        // Value access.

        /// Returns the immutable canonical authority bytes without copying them.
        constexpr const Storage& Bytes() const noexcept {
            return _bytes;
        }

        /// Returns the authority as an explicit host integer for tooling and diagnostics.
        constexpr std::uint32_t Value() const noexcept {
            return
                (static_cast<std::uint32_t>(_bytes[0U]) << 16U) |
                (static_cast<std::uint32_t>(_bytes[1U]) << 8U) |
                static_cast<std::uint32_t>(_bytes[2U]);
        }

        /// Indicates whether the authority is the reserved all-zero value.
        constexpr bool IsZero() const noexcept {
            return Value() == 0U;
        }

        /// Indicates whether the authority contains a valid allocated non-zero identity.
        constexpr bool IsValid() const noexcept {
            return !IsZero();
        }

        /// Indicates whether the authority contains a valid allocated non-zero identity.
        constexpr explicit operator bool() const noexcept {
            return IsValid();
        }


        // Comparison.

        /// Compares authorities by exact canonical byte equality.
        ///
        /// @param other Authority to compare with this value.
        constexpr bool operator ==(const TypeAuthorityIdentifier& other) const noexcept {
            for (std::size_t index = 0U; index < Size; ++index) {
                if (_bytes[index] != other._bytes[index]) return false;
            }

            return true;
        }

        /// Compares authorities for inequality.
        ///
        /// @param other Authority to compare with this value.
        constexpr bool operator !=(const TypeAuthorityIdentifier& other) const noexcept {
            return !(*this == other);
        }

        /// Provides deterministic lexicographic ordering by canonical authority bytes.
        ///
        /// @param other Authority to compare with this value.
        constexpr bool operator <(const TypeAuthorityIdentifier& other) const noexcept {
            for (std::size_t index = 0U; index < Size; ++index) {
                if (_bytes[index] < other._bytes[index]) return true;
                if (_bytes[index] > other._bytes[index]) return false;
            }

            return false;
        }

    };


    static_assert(
        sizeof(TypeAuthorityIdentifier) == TypeAuthorityIdentifier::Size,
        "TypeAuthorityIdentifier must remain an exact 3-byte semantic value"
    );

} // ESPressio::System
