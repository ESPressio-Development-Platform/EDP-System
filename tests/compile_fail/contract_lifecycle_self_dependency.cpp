#include "ESPressio_System.hpp"

namespace Framework = ESPressio::System::CompositionFramework;

struct TestDomain final : Framework::Domain {};
struct Service final : Framework::ExclusiveCapability<TestDomain> {};

using SelfRequirement = Framework::Requirement<
    Service,
    Framework::RequirementScope::SameDomain,
    Framework::ExactlyProviders<1U>
>;

struct SelfDependentProvider final : Framework::Provider<
    TestDomain,
    Framework::Offers<
        Framework::Offer<Service>
    >,
    Framework::Contract<
        Framework::InitializesAfter<SelfRequirement>
    >
> {};

using TestComposition = Framework::Composition<
    TestDomain,
    SelfDependentProvider
>;

using InvalidArchitecture = Framework::Architecture<TestComposition>;

static_assert(InvalidArchitecture::IsValid);

int main() {
    return 0;
}
