#include "ESPressio_System.hpp"

/// Foreign Type which owns the incorrectly reused member binding.
struct ForeignOwner final {
    int Value = 0;
};

/// Identified Type whose declared FieldSet does not bind its own members.
struct InvalidSchema final {

    /// Valid universal Type identity used to isolate the owner mismatch.
    static constexpr ESPressio::System::TypeIdentifier Identifier{
        ESPressio::System::TypeIdentifier::Storage{
            0x01U,
            0x23U,
            0x45U,
            0x00U,
            0x00U,
            0x00U,
            0x00U,
            0x01U
        }
    };

    /// Invalid schema owned by ForeignOwner rather than InvalidSchema.
    using Fields = ESPressio::System::FieldSet<
        ESPressio::System::FieldBinding<
            &ForeignOwner::Value,
            0U
        >
    >;

};


static_assert(
    ESPressio::System::SchemaType<InvalidSchema>,
    "InvalidSchema must be rejected because its Field owner is different"
);


/// Compile-fail test entry point.
int main() {
    return 0;
}
