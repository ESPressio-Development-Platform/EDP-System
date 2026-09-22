#include "ESPressio_System.hpp"

namespace Framework = ESPressio::System::CompositionFramework;

struct TestDomain final : Framework::Domain {};
struct SharedService final : Framework::SharedCapability<TestDomain> {};
struct Consumer final : Framework::ExclusiveCapability<TestDomain> {};

using TwoServices = Framework::Requirement<
    SharedService,
    Framework::RequirementScope::SameDomain,
    Framework::ExactlyProviders<2U>
>;

struct OnlyService final : Framework::Provider<
    TestDomain,
    Framework::Offers<
        Framework::Offer<SharedService>
    >
> {};

struct ConsumerProvider final : Framework::Provider<
    TestDomain,
    Framework::Offers<
        Framework::Offer<Consumer>
    >,
    Framework::Contract<TwoServices>
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
