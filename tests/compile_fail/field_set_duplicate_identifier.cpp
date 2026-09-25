#include "ESPressio_System.hpp"

/// Demonstration owner containing two distinct data members.
struct Example final {
    int First = 0;
    int Second = 0;
};

/// Invalid schema which assigns the same Field identity to two members.
using InvalidFields = ESPressio::System::FieldSet<
    ESPressio::System::FieldBinding<
        &Example::First,
        7U
    >,
    ESPressio::System::FieldBinding<
        &Example::Second,
        7U
    >
>;

/// Forces instantiation of the invalid FieldSet.
int main() {
    return static_cast<int>(InvalidFields::Count);
}
