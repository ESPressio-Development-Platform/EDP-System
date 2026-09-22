#include <cstdint>
#include <type_traits>

#include <ESPressio_System.hpp>

namespace Demo {

    namespace Framework = ESPressio::System::CompositionFramework;

    /// Finite feature vocabulary advertised by the demonstration Radio providers.
    enum class RadioFeature : std::uint8_t {
        Broadcast = 0,
        Acknowledgements = 1,
        ReceiveTimestamp = 2,
        LowEnergy = 3
    };


    /// Composition domain containing the demonstration Radio capability.
    struct RadioDomain final : Framework::Domain {};


    /// Shared Radio capability with multiple concrete providers.
    struct Radio final : Framework::SharedCapability<RadioDomain> {};


    /// Typed compile-time feature set belonging to the Radio capability.
    struct SupportedFeatures final : Framework::FlagProperty<
        Radio,
        RadioFeature,
        std::uint8_t
    > {};


    /// Radio provider advertising broadcast and low-energy support.
    struct LowEnergyRadio final : Framework::Provider<
        RadioDomain,
        Framework::Provides<
            Framework::Offer<
                Radio,
                Framework::FlagPropertyValue<
                    SupportedFeatures,
                    RadioFeature::Broadcast,
                    RadioFeature::LowEnergy
                >
            >
        >
    > {};


    /// Radio provider advertising broadcast, acknowledgement, and receive-timestamp support.
    struct TimestampedRadio final : Framework::Provider<
        RadioDomain,
        Framework::Provides<
            Framework::Offer<
                Radio,
                Framework::FlagPropertyValue<
                    SupportedFeatures,
                    RadioFeature::Broadcast,
                    RadioFeature::Acknowledgements,
                    RadioFeature::ReceiveTimestamp
                >
            >
        >
    > {};


    /// Requirement selecting a Radio which supports both acknowledgements and receive timestamps.
    using ReliableTimestampRequirement = Framework::Need<
        Radio,
        Framework::HasAllFlags<
            SupportedFeatures,
            RadioFeature::Acknowledgements,
            RadioFeature::ReceiveTimestamp
        >
    >;


    /// Requirement accepting either low-energy or receive-timestamp support.
    using SpecializedRadioRequirement = Framework::Need<
        Radio,
        Framework::HasAnyFlags<
            SupportedFeatures,
            RadioFeature::LowEnergy,
            RadioFeature::ReceiveTimestamp
        >
    >;


    /// Requirement selecting providers which do not advertise low-energy operation.
    using NonLowEnergyRequirement = Framework::Need<
        Radio,
        Framework::HasNoFlags<
            SupportedFeatures,
            RadioFeature::LowEnergy
        >
    >;


    /// Complete Radio-domain Composition used by the demonstration.
    using RadioComposition = Framework::Composition<
        RadioDomain,
        LowEnergyRadio,
        TimestampedRadio
    >;


    /// Provider selected entirely from typed compile-time flag requirements.
    using ReliableTimestampRadio = RadioComposition::ProviderSatisfying<
        ReliableTimestampRequirement
    >;


    static_assert(
        RadioComposition::IsValid,
        "The demonstration Composition must be structurally valid"
    );

    static_assert(
        std::is_same_v<
            ReliableTimestampRadio,
            TimestampedRadio
        >,
        "The reliable timestamp requirement must select TimestampedRadio"
    );

    static_assert(
        RadioComposition::ProviderCountSatisfying<SpecializedRadioRequirement> == 2U,
        "HasAnyFlags must retain both specialised Radio providers"
    );

    static_assert(
        RadioComposition::ProviderCountSatisfying<NonLowEnergyRequirement> == 1U,
        "HasNoFlags must retain only the non-low-energy provider"
    );


    /// Completes the demonstration after all meaningful work has been validated at compile time.
    int Run() noexcept {
        return 0;
    }

} // Demo


/// Runs the demonstration from the ESP-IDF application entrypoint.
extern "C" void app_main() {
    static_cast<void>(
        Demo::Run()
    );
}
