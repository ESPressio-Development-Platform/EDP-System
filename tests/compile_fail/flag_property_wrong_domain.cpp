#include <cstdint>

#include "ESPressio_System.hpp"

namespace Test {

    namespace Framework = ESPressio::System::CompositionFramework;

    /// Composition domain used by this intentional compile-failure test.
    struct Domain final : Framework::Domain {};

    /// Capability used by this intentional compile-failure test.
    struct Capability final : Framework::ExclusiveCapability<Domain> {};

    /// Correct flag enum for the tested Property.
    enum class Feature : std::uint8_t {
        Supported = 0
    };

    /// Unrelated flag enum which must not be accepted by the tested Property.
    enum class ForeignFeature : std::uint8_t {
        Unsupported = 0
    };

    /// Typed flag Property accepting only Feature values.
    struct Features final : Framework::FlagProperty<
        Capability,
        Feature,
        std::uint8_t
    > {};

    /// Intentionally invalid value using a different enum domain.
    using InvalidValue = Framework::FlagPropertyValue<
        Features,
        ForeignFeature::Unsupported
    >;

    static_assert(
        InvalidValue::Value == 0U,
        "This source must fail before a value can be observed"
    );

} // Test
