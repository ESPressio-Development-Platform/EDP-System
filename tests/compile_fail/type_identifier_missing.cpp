#include "ESPressio_System.hpp"

namespace {

    /// Type intentionally missing the required direct Identifier declaration.
    struct MissingIdentifier final {};

    /// Forces evaluation of the canonical reader and its focused missing-member diagnostic.
    [[maybe_unused]] constexpr auto Identifier =
        ESPressio::System::TypeIdentifierOf<MissingIdentifier>;

} // anonymous namespace

/// This translation unit is expected to fail compilation before main is emitted.
int main() {
    return 0;
}
