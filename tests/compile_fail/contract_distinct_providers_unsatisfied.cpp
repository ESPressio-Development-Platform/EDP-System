#include "ESPressio_System.hpp"

namespace Framework = ESPressio::System::CompositionFramework;

struct TestDomain final : Framework::Domain {};
struct Service final : Framework::SharedCapability<TestDomain> {};
struct Consumer final : Framework::ExclusiveCapability<TestDomain> {};

struct Role final : Framework::Property<
    Service,
    int
> {};

using FirstRole = Framework::Requirement<
    Service,
    Framework::RequirementScope::SameDomain,
    Framework::AtLeastProviders<1U>,
    Framework::Equals<
        Role,
        1
    >
>;

using SecondRole = Framework::Requirement<
    Service,
    Framework::RequirementScope::SameDomain,
    Framework::AtLeastProviders<1U>,
    Framework::Equals<
        Role,
        1
    >
>;

struct OnlyService final : Framework::Provider<
    TestDomain,
    Framework::Offers<
        Framework::Offer<
            Service,
            Framework::PropertyValue<
                Role,
                1
            >
        >
    >
> {};

struct ConsumerProvider final : Framework::Provider<
    TestDomain,
    Framework::Offers<
        Framework::Offer<Consumer>
    >,
    Framework::Contract<
        Framework::DistinctProviders<
            FirstRole,
            SecondRole
        >
    >
> {};

using InvalidComposition = Framework::Composition<
    TestDomain,
    OnlyService,
    ConsumerProvider
>;

static_assert(InvalidComposition::IsValid);

int main() {
    return 0;
}
