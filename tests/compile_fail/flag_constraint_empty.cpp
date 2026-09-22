#include <cstdint>

#include "ESPressio_System.hpp"

namespace Test {

    namespace Framework = ESPressio::System::CompositionFramework;

    /// Flag enum used by this intentional compile-failure test.
    enum class Feature : std::uint8_t {
        Supported = 0
    };

    /// Composition domain used by this intentional compile-failure test.
    struct Domain final : Framework::Domain {};

    /// Capability used by this intentional compile-failure test.
    struct Capability final : Framework::ExclusiveCapability<Domain> {};

    /// Typed flag Property used to verify that a constraint must request at least one flag.
    struct Features final : Framework::FlagProperty<
        Capability,
        Feature,
        std::uint8_t
    > {};

    /// Intentionally invalid empty all-flags requirement.
    using InvalidConstraint = Framework::HasAllFlags<Features>;

    static_assert(
        sizeof(InvalidConstraint) > 0U,
        "This source must fail before an empty flag constraint can be formed"
    );

} // Test
