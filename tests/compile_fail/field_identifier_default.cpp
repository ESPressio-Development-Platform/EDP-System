#include "ESPressio_System.hpp"

/// Verifies that FieldIdentifier cannot silently default to Field zero.
int main() {
    ESPressio::System::FieldIdentifier identifier;
    static_cast<void>(identifier);
    return 0;
}
