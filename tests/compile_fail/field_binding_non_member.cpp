#include "ESPressio_System.hpp"

/// Invalid Field binding which does not reference a non-static data member.
using InvalidField = ESPressio::System::FieldBinding<
    42,
    0U
>;

/// Forces instantiation of the invalid Field binding.
int main() {
    return static_cast<int>(sizeof(InvalidField));
}
