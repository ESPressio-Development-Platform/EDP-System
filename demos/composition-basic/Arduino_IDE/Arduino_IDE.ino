#include <Arduino.h>

#include <cstdint>
#include <type_traits>

#include <ESPressio_System.hpp>

namespace Demo {

    namespace Framework = ESPressio::System::CompositionFramework;

    /// Composition domain containing the demonstration Radio capability.
    struct RadioDomain final : Framework::Domain {};


    /// Composition domain containing a service that consumes qualified Radio providers.
    struct ServiceDomain final : Framework::Domain {};


    /// Shared Radio capability with multiple valid concrete providers.
    struct Radio final : Framework::SharedCapability<RadioDomain> {};


    /// Service capability whose provider requires two specifically-qualified Radios.
    struct TelemetryService final : Framework::ExclusiveCapability<ServiceDomain> {};


    /// First concrete Radio provider distinguished only by compile-time Attributes.
    class FrequencyXRadio final : public Framework::Provider<
        RadioDomain,
        Framework::Offers<
            Framework::Offer<
                Radio,
                Framework::Attribute<
                    "Frequency",
                    24917U
                >,
                Framework::TextAttribute<
                    "Role",
                    "Telemetry"
                >
            >
        >
    > {};


    /// Second concrete Radio provider distinguished only by compile-time Attributes.
    class FrequencyYRadio final : public Framework::Provider<
        RadioDomain,
        Framework::Offers<
            Framework::Offer<
                Radio,
                Framework::Attribute<
                    "Frequency",
                    58124U
                >,
                Framework::TextAttribute<
                    "Role",
                    "Control"
                >
            >
        >
    > {};


    /// Requirement selecting the Telemetry Radio without naming its concrete implementation.
    using TelemetryRadioRequirement = Framework::Requirement<
        Radio,
        Framework::RequirementScope::ExternalDomain,
        Framework::ExactlyProviders<1U>,
        Framework::AttributeEquals<
            "Frequency",
            24917U
        >,
        Framework::TextAttributeEquals<
            "Role",
            "Telemetry"
        >
    >;


    /// Requirement selecting the Control Radio without naming its concrete implementation.
    using ControlRadioRequirement = Framework::Requirement<
        Radio,
        Framework::RequirementScope::ExternalDomain,
        Framework::ExactlyProviders<1U>,
        Framework::AttributeEquals<
            "Frequency",
            58124U
        >,
        Framework::TextAttributeEquals<
            "Role",
            "Control"
        >
    >;


    /// Service provider declaring cross-domain dependencies on both qualified Radio providers.
    class TelemetryServiceProvider final : public Framework::Provider<
        ServiceDomain,
        Framework::Offers<
            Framework::Offer<TelemetryService>
        >,
        Framework::Contract<
            TelemetryRadioRequirement,
            ControlRadioRequirement
        >
    > {};


    /// Radio-domain Composition containing both otherwise-compatible Radio implementations.
    using RadioComposition = Framework::Composition<
        RadioDomain,
        FrequencyXRadio,
        FrequencyYRadio
    >;


    /// Service-domain Composition containing the cross-domain consumer.
    using ServiceComposition = Framework::Composition<
        ServiceDomain,
        TelemetryServiceProvider
    >;


    /// Complete architecture validating the service's cross-domain Radio dependencies.
    using ApplicationArchitecture = Framework::Architecture<
        RadioComposition,
        ServiceComposition
    >;


    /// Concrete Telemetry Radio selected entirely from compile-time capability and Attribute requirements.
    using TelemetryRadioProvider = ApplicationArchitecture::Select<
        TelemetryRadioRequirement,
        Framework::SelectUnique
    >;

    /// Concrete Control Radio selected entirely from compile-time capability and Attribute requirements.
    using ControlRadioProvider = ApplicationArchitecture::Select<
        ControlRadioRequirement,
        Framework::SelectUnique
    >;


    static_assert(
        ApplicationArchitecture::IsValid,
        "Demonstration Architecture must satisfy every cross-domain dependency"
    );

    static_assert(
        std::is_same_v<TelemetryRadioProvider, FrequencyXRadio>,
        "Telemetry requirement must resolve FrequencyXRadio"
    );

    static_assert(
        std::is_same_v<ControlRadioProvider, FrequencyYRadio>,
        "Control requirement must resolve FrequencyYRadio"
    );


    /// Completes the demonstration after all meaningful work has been validated at compile time.
    int Run() noexcept {
        return 0;
    }

} // Demo


/// Runs the demonstration once during Arduino initialization.
void setup() {
    static_cast<void>(Demo::Run());
}

/// Leaves the demonstration idle after the one-time run.
void loop() {}
