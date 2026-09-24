#include <cstdint>

#include "ESPressio_System.hpp"

namespace {

    /// Type intentionally declaring Identifier in the wrong semantic domain.
    struct WrongIdentifierType final {

        /// Deliberately incorrect scalar declaration used to exercise exact-Type validation.
        static constexpr std::uint64_t Identifier = 1U;

    };

    /// Forces evaluation of the canonical reader and its focused exact-Type diagnostic.
    [[maybe_unused]] constexpr auto Identifier =
        ESPressio::System::TypeIdentifierOf<WrongIdentifierType>;

} // anonymous namespace

/// This translation unit is expected to fail compilation before main is emitted.
int main() {
    return 0;
}
