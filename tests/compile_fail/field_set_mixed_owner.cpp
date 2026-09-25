#include "ESPressio_System.hpp"

/// First demonstration owner.
struct FirstOwner final {
    int Value = 0;
};

/// Second demonstration owner.
struct SecondOwner final {
    int Value = 0;
};

/// Invalid schema which mixes Fields from two owning Types.
using InvalidFields = ESPressio::System::FieldSet<
    ESPressio::System::FieldBinding<
        &FirstOwner::Value,
        1U
    >,
    ESPressio::System::FieldBinding<
        &SecondOwner::Value,
        2U
    >
>;

/// Forces instantiation of the invalid FieldSet.
int main() {
    return static_cast<int>(InvalidFields::Count);
}
