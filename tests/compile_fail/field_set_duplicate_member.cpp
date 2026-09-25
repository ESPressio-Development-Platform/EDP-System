#include "ESPressio_System.hpp"

/// Demonstration owner containing one data member.
struct Example final {
    int Value = 0;
};

/// Invalid schema which assigns two identities to the same data member.
using InvalidFields = ESPressio::System::FieldSet<
    ESPressio::System::FieldBinding<
        &Example::Value,
        1U
    >,
    ESPressio::System::FieldBinding<
        &Example::Value,
        2U
    >
>;

/// Forces instantiation of the invalid FieldSet.
int main() {
    return static_cast<int>(InvalidFields::Count);
}
