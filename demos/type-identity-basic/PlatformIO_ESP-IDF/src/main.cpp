#include <ESPressio_System.hpp>

namespace Demo {

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
    int Run() noexcept {
        const auto identifier = ESPressio::System::TypeIdentifierOf<ExampleType>;

        if (!identifier.IsValid()) return 1;
        if (identifier.Authority() != ExampleAuthority) return 2;
        if (identifier.LocalIdentifier() != ExampleLocalIdentifier) return 3;
        if (identifier.Authority().Value() != 0x012345U) return 4;
        if (identifier.LocalIdentifier().Value() != 0x6789ABCDEFULL) return 5;

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

        if (identifier.Bytes() != expectedBytes) return 6;

        return 0;
    }

} // Demo

/// Runs the demonstration from the ESP-IDF application entry point.
extern "C" void app_main() {
    static_cast<void>(Demo::Run());
}
