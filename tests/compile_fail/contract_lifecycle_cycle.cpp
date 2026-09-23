#include "ESPressio_System.hpp"

namespace Framework = ESPressio::System::CompositionFramework;

struct TestDomain final : Framework::Domain {};
struct FirstCapability final : Framework::ExclusiveCapability<TestDomain> {};
struct SecondCapability final : Framework::ExclusiveCapability<TestDomain> {};

using FirstRequirement = Framework::Requirement<
    FirstCapability,
    Framework::RequirementScope::SameDomain,
    Framework::ExactlyProviders<1U>
>;

using SecondRequirement = Framework::Requirement<
    SecondCapability,
    Framework::RequirementScope::SameDomain,
    Framework::ExactlyProviders<1U>
>;

struct FirstProvider final : Framework::Provider<
    TestDomain,
    Framework::Offers<
        Framework::Offer<FirstCapability>
    >,
    Framework::Contract<
        Framework::InitializesAfter<SecondRequirement>
    >
> {};

struct SecondProvider final : Framework::Provider<
    TestDomain,
    Framework::Offers<
        Framework::Offer<SecondCapability>
    >,
    Framework::Contract<
        Framework::InitializesAfter<FirstRequirement>
    >
> {};

using TestComposition = Framework::Composition<
    TestDomain,
    FirstProvider,
    SecondProvider
>;

using TestArchitecture = Framework::Architecture<TestComposition>;

using InvalidOrder = TestArchitecture::InitializationOrder;

InvalidOrder value;

int main() {
    return 0;
}
