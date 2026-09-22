#include <cstdint>

#include "ESPressio_System.hpp"

namespace Test {

    namespace Framework = ESPressio::System::CompositionFramework;

    /// Flag enum containing one position which cannot fit the selected one-byte Property storage.
    enum class Feature : std::uint8_t {
        Supported = 0,
        OutsideStorage = 8
    };

    /// Composition domain used by this intentional compile-failure test.
    struct Domain final : Framework::Domain {};

    /// Capability used by this intentional compile-failure test.
    struct Capability final : Framework::ExclusiveCapability<Domain> {};

    /// One-byte typed flag Property used to verify representability diagnostics.
    struct Features final : Framework::FlagProperty<
        Capability,
        Feature,
        std::uint8_t
    > {};

    /// Intentionally invalid value using an unrepresentable flag position.
    using InvalidValue = Framework::FlagPropertyValue<
        Features,
        Feature::OutsideStorage
    >;

    static_assert(
        InvalidValue::Value == 0U,
        "This source must fail before a value can be observed"
    );

} // Test
