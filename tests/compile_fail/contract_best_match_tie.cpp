#include "ESPressio_System.hpp"

namespace Framework = ESPressio::System::CompositionFramework;

struct TestDomain final : Framework::Domain {};
struct Service final : Framework::SharedCapability<TestDomain> {};

struct Cost final : Framework::Property<
    Service,
    int
> {};

struct FirstService final : Framework::Provider<
    TestDomain,
    Framework::Offers<
        Framework::Offer<
            Service,
            Framework::PropertyValue<
                Cost,
                1
            >
        >
    >
> {};

struct SecondService final : Framework::Provider<
    TestDomain,
    Framework::Offers<
        Framework::Offer<
            Service,
            Framework::PropertyValue<
                Cost,
                1
            >
        >
    >
> {};

using Services = Framework::Composition<
    TestDomain,
    FirstService,
    SecondService
>;

using ServiceRequirement = Framework::Requirement<
    Service,
    Framework::RequirementScope::SameDomain
>;

using AmbiguousBest = Services::Select<
    ServiceRequirement,
    Framework::SelectMinimum<Cost>
>;

AmbiguousBest value;

int main() {
    return 0;
}
