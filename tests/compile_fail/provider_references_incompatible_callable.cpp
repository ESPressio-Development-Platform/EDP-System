#include <cstdint>

#include "ESPressio_System.hpp"

namespace Test {

    namespace Framework = ESPressio::System::CompositionFramework;

    /// Radio feature vocabulary used by this intentional compile-failure test.
    enum class RadioFeature : std::uint8_t {
        Broadcast = 0
    };


    /// Composition domain used by this intentional compile-failure test.
    struct RadioDomain final : Framework::Domain {};


    /// Shared Radio capability used by this intentional compile-failure test.
    struct Radio final : Framework::SharedCapability<RadioDomain> {};


    /// First Radio provider type represented by the target ProviderList.
    struct FirstRadio final : Framework::Provider<
        RadioDomain,
        Framework::Offers<
            Framework::Offer<Radio>
        >
    > {};


    /// Second Radio provider type represented by the target ProviderList.
    struct SecondRadio final : Framework::Provider<
        RadioDomain,
        Framework::Offers<
            Framework::Offer<Radio>
        >
    > {};


    /// Non-owning reference collection containing two different provider types.
    using References = Framework::ProviderReferences<
        Framework::ProviderList<
            FirstRadio,
            SecondRadio
        >
    >;

} // Test


/// Intentionally supplies a callable which cannot accept every represented provider type.
int main() {
    Test::FirstRadio firstRadio;
    Test::SecondRadio secondRadio;

    Test::References references(
        firstRadio,
        secondRadio
    );

    references.ForEach(
        [](Test::FirstRadio&) {}
    );

    return 0;
}
