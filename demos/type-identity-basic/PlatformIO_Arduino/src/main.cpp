#include <Arduino.h>

#include <cstdint>

#include <ESPressio_System.hpp>

namespace Demo {

    /// Result of executing the universal Type identity demonstration.
    enum class DemoResult : std::uint8_t {
        Succeeded = 0,
        IdentifierInvalid = 1,
        AuthorityMismatch = 2,
        LocalIdentifierMismatch = 3,
        AuthorityValueMismatch = 4,
        LocalValueMismatch = 5,
        CanonicalBytesMismatch = 6
    };


    /// Demonstration Type authority owned by one logical schema/governance authority.
    inline constexpr ESPressio::System::TypeAuthorityIdentifier ExampleAuthority{
        ESPressio::System::TypeAuthorityIdentifier::Storage{
            0x01U,
            0x23U,
            0x45U
        }
    };

    /// Demonstration authority-local Type value allocated by ExampleAuthority.
    inline constexpr ESPressio::System::TypeLocalIdentifier ExampleLocalIdentifier{
        ESPressio::System::TypeLocalIdentifier::Storage{
            0x67U,
            0x89U,
            0xABU,
            0xCDU,
            0xEFU
        }
    };


    /// Demonstration semantic Type carrying its universal identity directly.
    struct ExampleType final {

        /// Stable universal Type identity used by generic EDP facilities.
        static constexpr ESPressio::System::TypeIdentifier Identifier{
            ExampleAuthority,
            ExampleLocalIdentifier
        };

    };


    static_assert(
        ESPressio::System::IdentifiedType<ExampleType>,
        "ExampleType must satisfy the universal identification contract"
    );


    /// Runs the universal Type identity demonstration.
    DemoResult Run() noexcept {
        const auto identifier = ESPressio::System::TypeIdentifierOf<ExampleType>;

        if (!identifier.IsValid()) return DemoResult::IdentifierInvalid;
        if (identifier.Authority() != ExampleAuthority) return DemoResult::AuthorityMismatch;
        if (identifier.LocalIdentifier() != ExampleLocalIdentifier) return DemoResult::LocalIdentifierMismatch;
        if (identifier.Authority().Value() != 0x012345U) return DemoResult::AuthorityValueMismatch;
        if (identifier.LocalIdentifier().Value() != 0x6789ABCDEFULL) return DemoResult::LocalValueMismatch;

        const ESPressio::System::TypeIdentifier::Storage expectedBytes{
            0x01U,
            0x23U,
            0x45U,
            0x67U,
            0x89U,
            0xABU,
            0xCDU,
            0xEFU
        };

        if (identifier.Bytes() != expectedBytes) return DemoResult::CanonicalBytesMismatch;

        return DemoResult::Succeeded;
    }

} // Demo

/// Runs the demonstration once during Arduino initialization.
void setup() {
    static_cast<void>(Demo::Run());
}

/// Leaves the demonstration idle after the one-time run.
void loop() {}
