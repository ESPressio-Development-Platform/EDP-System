#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "TypeAuthorityIdentifier.hpp"
#include "TypeLocalIdentifier.hpp"

namespace ESPressio::System {

    /// Universal canonical identity of one EDP semantic Type.
    ///
    /// The first three bytes are the globally governed Type authority and the
    /// remaining five bytes are the authority-local Type value. The retained
    /// representation is canonical byte storage rather than a host-endian integer.
    class TypeIdentifier final {
    private:

        // Canonical complete Type identity storage.

        /// Exact eight-byte Type identity representation in canonical byte order.
        std::array<std::uint8_t, 8U> _bytes{};

    public:

        // Type identity metadata.

        /// Exact complete Type identity width in bytes.
        static constexpr std::size_t Size = 8U;

        /// Complete fixed-width Type identity storage.
        using Storage = std::array<std::uint8_t, Size>;


        // Construction.

        /// Creates the Invalid/Unspecified all-zero Type identity.
        constexpr TypeIdentifier() noexcept = default;

        /// Creates a Type identity from its complete canonical eight-byte representation.
        ///
        /// @param bytes Exact Type identity bytes in canonical order.
        constexpr explicit TypeIdentifier(const Storage& bytes) noexcept :
            _bytes(bytes) {}

        /// Creates a Type identity from its strong authority and authority-local components.
        ///
        /// @param authority Globally governed Type authority.
        /// @param localIdentifier Authority-local Type identity allocated by that authority.
        constexpr TypeIdentifier(
            const TypeAuthorityIdentifier& authority,
            const TypeLocalIdentifier& localIdentifier
        ) noexcept :
            _bytes{
                authority.Bytes()[0U],
                authority.Bytes()[1U],
                authority.Bytes()[2U],
                localIdentifier.Bytes()[0U],
                localIdentifier.Bytes()[1U],
                localIdentifier.Bytes()[2U],
                localIdentifier.Bytes()[3U],
                localIdentifier.Bytes()[4U]
            } {}


        // Value access.

        /// Returns the immutable canonical eight-byte Type identity without copying it.
        constexpr const Storage& Bytes() const noexcept {
            return _bytes;
        }

        /// Returns the strong authority component represented by bytes zero through two.
        constexpr TypeAuthorityIdentifier Authority() const noexcept {
            return TypeAuthorityIdentifier(
                TypeAuthorityIdentifier::Storage{
                    _bytes[0U],
                    _bytes[1U],
                    _bytes[2U]
                }
            );
        }

        /// Returns the strong authority-local Type component represented by bytes three through seven.
        constexpr TypeLocalIdentifier LocalIdentifier() const noexcept {
            return TypeLocalIdentifier(
                TypeLocalIdentifier::Storage{
                    _bytes[3U],
                    _bytes[4U],
                    _bytes[5U],
                    _bytes[6U],
                    _bytes[7U]
                }
            );
        }

        /// Indicates whether the complete representation is the reserved all-zero value.
        constexpr bool IsZero() const noexcept {
            for (const auto value : _bytes) {
                if (value != 0U) return false;
            }

            return true;
        }

        /// Indicates whether both governed identity components contain valid non-zero values.
        constexpr bool IsValid() const noexcept {
            return Authority().IsValid() && LocalIdentifier().IsValid();
        }

        /// Indicates whether both governed identity components contain valid non-zero values.
        constexpr explicit operator bool() const noexcept {
            return IsValid();
        }


        // Comparison.

        /// Compares complete Type identities by exact canonical byte equality.
        ///
        /// @param other Type identity to compare with this value.
        constexpr bool operator ==(const TypeIdentifier& other) const noexcept {
            for (std::size_t index = 0U; index < Size; ++index) {
                if (_bytes[index] != other._bytes[index]) return false;
            }

            return true;
        }

        /// Compares complete Type identities for inequality.
        ///
        /// @param other Type identity to compare with this value.
        constexpr bool operator !=(const TypeIdentifier& other) const noexcept {
            return !(*this == other);
        }

        /// Provides deterministic lexicographic ordering by canonical Type identity bytes.
        ///
        /// @param other Type identity to compare with this value.
        constexpr bool operator <(const TypeIdentifier& other) const noexcept {
            for (std::size_t index = 0U; index < Size; ++index) {
                if (_bytes[index] < other._bytes[index]) return true;
                if (_bytes[index] > other._bytes[index]) return false;
            }

            return false;
        }

    };


    static_assert(
        sizeof(TypeIdentifier) == TypeIdentifier::Size,
        "TypeIdentifier must remain an exact 8-byte semantic value"
    );

} // ESPressio::System
