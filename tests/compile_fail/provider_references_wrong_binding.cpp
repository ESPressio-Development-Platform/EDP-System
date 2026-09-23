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


    /// Typed feature set advertised by the test providers.
    struct SupportedFeatures final : Framework::FlagProperty<
        Radio,
        RadioFeature,
        std::uint8_t
    > {};


    /// First Radio provider represented by the target ProviderList.
    struct FirstRadio final : Framework::Provider<
        RadioDomain,
        Framework::Offers<
            Framework::Offer<
                Radio,
                Framework::FlagPropertyValue<
                    SupportedFeatures,
                    RadioFeature::Broadcast
                >
            >
        >
    > {};


    /// Second Radio provider represented by the target ProviderList.
    struct SecondRadio final : Framework::Provider<
        RadioDomain,
        Framework::Offers<
            Framework::Offer<
                Radio,
                Framework::FlagPropertyValue<
                    SupportedFeatures,
                    RadioFeature::Broadcast
                >
            >
        >
    > {};


    /// Non-owning reference collection requiring FirstRadio followed by SecondRadio.
    using References = Framework::ProviderReferences<
        Framework::ProviderList<
            FirstRadio,
            SecondRadio
        >
    >;

} // Test


/// Intentionally attempts to bind the resolved providers in the wrong order.
int main() {
    Test::FirstRadio firstRadio;
    Test::SecondRadio secondRadio;

    Test::References references(
        secondRadio,
        firstRadio
    );

    static_cast<void>(references);

    return 0;
}
