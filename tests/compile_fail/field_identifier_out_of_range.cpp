#include "ESPressio_System.hpp"

/// Verifies that values outside the complete one-byte Field identity range are rejected.
int main() {
    constexpr ESPressio::System::FieldIdentifier InvalidIdentifier{256U};
    static_cast<void>(InvalidIdentifier);
    return 0;
}
