#include "ESPressio_System.hpp"

namespace Framework = ESPressio::System::CompositionFramework;

struct FirstDomain final : Framework::Domain {};
struct SecondDomain final : Framework::Domain {};

struct FirstCapability final : Framework::SharedCapability<FirstDomain> {};
struct SecondCapability final : Framework::SharedCapability<SecondDomain> {};
struct Consumer final : Framework::ExclusiveCapability<FirstDomain> {};

using LocalRequirement = Framework::Requirement<
    FirstCapability,
    Framework::RequirementScope::SameDomain
>;

using ExternalRequirement = Framework::Requirement<
    SecondCapability,
    Framework::RequirementScope::ExternalDomain
>;

struct InvalidProvider final : Framework::Provider<
    FirstDomain,
    Framework::Offers<
        Framework::Offer<Consumer>
    >,
    Framework::Contract<
        Framework::SameProvider<
            LocalRequirement,
            ExternalRequirement
        >
    >
> {};

int main() {
    return sizeof(InvalidProvider) == 0U;
}
