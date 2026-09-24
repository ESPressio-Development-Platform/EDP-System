#include "ESPressio_System.hpp"

namespace {

    /// Type intentionally declaring the reserved Invalid/Unspecified universal identity.
    struct InvalidIdentifier final {

        /// Reserved all-zero identifier used to exercise validity diagnostics.
        static constexpr ESPressio::System::TypeIdentifier Identifier{};

    };

    /// Forces evaluation of the canonical reader and its invalid-identity diagnostic.
    [[maybe_unused]] constexpr auto Identifier =
        ESPressio::System::TypeIdentifierOf<InvalidIdentifier>;

} // anonymous namespace

/// This translation unit is expected to fail compilation before main is emitted.
int main() {
    return 0;
}
