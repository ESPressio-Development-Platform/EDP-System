#include "ESPressio_System.hpp"

namespace {

    /// Type intentionally exposing a mutable runtime Identifier rather than compile-time metadata.
    struct RuntimeIdentifier final {

        /// Deliberately non-constexpr identifier used to exercise constant-expression validation.
        inline static ESPressio::System::TypeIdentifier Identifier{
            ESPressio::System::TypeIdentifier::Storage{
                0x01U,
                0x23U,
                0x45U,
                0x67U,
                0x89U,
                0xABU,
                0xCDU,
                0xEFU
            }
        };

    };

    /// Forces evaluation of the canonical reader and its constant-expression diagnostic.
    [[maybe_unused]] constexpr auto Identifier =
        ESPressio::System::TypeIdentifierOf<RuntimeIdentifier>;

} // anonymous namespace

/// This translation unit is expected to fail compilation before main is emitted.
int main() {
    return 0;
}
