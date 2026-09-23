#include <cstdint>
#include <type_traits>

#include "ESPressio_System.hpp"

namespace ESPressio::System::Tests::Composition {

    namespace Framework = ESPressio::System::CompositionFramework;

    /// Composition domain used by the host-side framework tests.
    struct PlatformDomain final : Framework::Domain {};


    /// Secondary domain used to validate cross-domain dependencies.
    struct ServiceDomain final : Framework::Domain {};


    /// Exclusive clock capability used by the host-side framework tests.
    struct SystemClock final : Framework::ExclusiveCapability<PlatformDomain> {};


    /// Exclusive scheduler capability used by the host-side framework tests.
    struct TaskScheduler final : Framework::ExclusiveCapability<PlatformDomain> {};


    /// Shared communication capability used by the host-side framework tests.
    struct CommunicationInterface final : Framework::SharedCapability<PlatformDomain> {};


    /// Service capability whose provider depends on qualified communication providers.
    struct TelemetryService final : Framework::ExclusiveCapability<ServiceDomain> {};


    /// Resolution of the SystemClock capability in nanoseconds.
    struct ClockResolutionNanoseconds final : Framework::Property<SystemClock, std::uint32_t> {};


    /// Maximum payload supported by one CommunicationInterface provider.
    struct MaximumPayloadBytes final : Framework::Property<CommunicationInterface, std::uint16_t> {};


    /// Optional preferred-interface marker intentionally advertised by only one provider.
    struct PreferredInterface final : Framework::Property<CommunicationInterface, bool> {};


    /// Test provider supplying the SystemClock capability.
    struct ClockProvider final : Framework::Provider<
        PlatformDomain,
        Framework::Offers<
            Framework::Offer<
                SystemClock,
                Framework::PropertyValue<
                    ClockResolutionNanoseconds,
                    1000U
                >
            >
        >
    > {};


    /// Test provider supplying TaskScheduler while requiring a sufficiently precise SystemClock.
    struct SchedulerProvider final : Framework::Provider<
        PlatformDomain,
        Framework::Offers<
            Framework::Offer<TaskScheduler>
        >,
        Framework::Contract<
            Framework::Requirement<
                SystemClock,
                Framework::RequirementScope::SameDomain,
                Framework::ExactlyProviders<1U>,
                Framework::AtMost<
                    ClockResolutionNanoseconds,
                    1000U
                >
            >
        >
    > {};


    /// First test provider supplying the shared CommunicationInterface capability.
    struct WifiProvider final : Framework::Provider<
        PlatformDomain,
        Framework::Offers<
            Framework::Offer<
                CommunicationInterface,
                Framework::PropertyValue<
                    MaximumPayloadBytes,
                    1500U
                >,
                Framework::PropertyValue<
                    PreferredInterface,
                    true
                >,
                Framework::Attribute<
                    "Frequency",
                    24917
                >,
                Framework::TextAttribute<
                    "Transport",
                    "WiFi"
                >,
                Framework::TextAttribute<
                    "Band",
                    "2.4GHz"
                >,
                Framework::TextAttribute<
                    "Purpose",
                    "Primary"
                >
            >
        >
    > {};


    /// Second test provider supplying the shared CommunicationInterface capability.
    struct BluetoothProvider final : Framework::Provider<
        PlatformDomain,
        Framework::Offers<
            Framework::Offer<
                CommunicationInterface,
                Framework::PropertyValue<
                    MaximumPayloadBytes,
                    512U
                >,
                Framework::Attribute<
                    "Frequency",
                    58124
                >,
                Framework::TextAttribute<
                    "Transport",
                    "Bluetooth"
                >,
                Framework::TextAttribute<
                    "Band",
                    "2.4GHz"
                >
            >
        >
    > {};


    /// Qualified requirement selecting the Wi-Fi communication provider by numeric Attribute.
    using WifiFrequencyRequirement = Framework::Requirement<
        CommunicationInterface,
        Framework::RequirementScope::ExternalDomain,
        Framework::ExactlyProviders<1U>,
        Framework::AttributeEquals<
            "Frequency",
            24917
        >
    >;


    /// Qualified requirement selecting the Bluetooth provider by compile-time text Attribute.
    using BluetoothTransportRequirement = Framework::Requirement<
        CommunicationInterface,
        Framework::RequirementScope::ExternalDomain,
        Framework::ExactlyProviders<1U>,
        Framework::TextAttributeEquals<
            "Transport",
            "Bluetooth"
        >
    >;


    /// Broad qualified requirement intentionally matching both communication providers.
    using SharedBandRequirement = Framework::Requirement<
        CommunicationInterface,
        Framework::RequirementScope::SameDomain,
        Framework::AtLeastProviders<1U>,
        Framework::TextAttributeEquals<
            "Band",
            "2.4GHz"
        >
    >;


    /// Qualified requirement intentionally matching no communication provider.
    using MissingFrequencyRequirement = Framework::Requirement<
        CommunicationInterface,
        Framework::RequirementScope::SameDomain,
        Framework::AnyProviderCount,
        Framework::AttributeEquals<
            "Frequency",
            12345
        >
    >;


    /// Requirement whose Attribute is intentionally absent from one otherwise-compatible provider.
    using PrimaryPurposeRequirement = Framework::Requirement<
        CommunicationInterface,
        Framework::RequirementScope::SameDomain,
        Framework::ExactlyProviders<1U>,
        Framework::TextAttributeEquals<
            "Purpose",
            "Primary"
        >
    >;


    /// Requirement whose Property is intentionally absent from one otherwise-compatible provider.
    using PreferredInterfaceRequirement = Framework::Requirement<
        CommunicationInterface,
        Framework::RequirementScope::SameDomain,
        Framework::ExactlyProviders<1U>,
        Framework::Equals<
            PreferredInterface,
            true
        >
    >;


    /// Service provider requiring two differently-qualified communication providers from another Domain.
    struct TelemetryProvider final : Framework::Provider<
        ServiceDomain,
        Framework::Offers<
            Framework::Offer<TelemetryService>
        >,
        Framework::Contract<
            WifiFrequencyRequirement,
            BluetoothTransportRequirement
        >
    > {};


    /// Same-domain query for the unique SystemClock provider.
    using SystemClockRequirement = Framework::Requirement<
        SystemClock,
        Framework::RequirementScope::SameDomain,
        Framework::ExactlyProviders<1U>
    >;


    /// Same-domain query for the unique TaskScheduler provider.
    using TaskSchedulerRequirement = Framework::Requirement<
        TaskScheduler,
        Framework::RequirementScope::SameDomain,
        Framework::ExactlyProviders<1U>
    >;


    /// Same-domain query matching every CommunicationInterface provider.
    using CommunicationRequirement = Framework::Requirement<
        CommunicationInterface,
        Framework::RequirementScope::SameDomain,
        Framework::AnyProviderCount
    >;


    /// Valid platform Composition used to verify canonical and qualified provider resolution.
    using PlatformComposition = Framework::Composition<
        PlatformDomain,
        ClockProvider,
        SchedulerProvider,
        WifiProvider,
        BluetoothProvider
    >;


    /// Valid service Composition used to verify architecture-level cross-domain dependencies.
    using ServiceComposition = Framework::Composition<
        ServiceDomain,
        TelemetryProvider
    >;


    /// Complete multi-domain Architecture used by the host-side tests.
    using TestArchitecture = Framework::Architecture<
        PlatformComposition,
        ServiceComposition
    >;


    static_assert(
        PlatformComposition::IsValid,
        "Expected the platform Composition to be valid"
    );

    static_assert(
        PlatformComposition::ProviderCount == 4U,
        "Expected every platform provider to be represented in the Composition"
    );

    static_assert(
        PlatformComposition::HasMatch<SystemClockRequirement>,
        "Expected the Composition to provide SystemClock"
    );

    static_assert(
        PlatformComposition::HasMatch<TaskSchedulerRequirement>,
        "Expected the Composition to provide TaskScheduler"
    );

    static_assert(
        PlatformComposition::MatchCount<CommunicationRequirement> == 2U,
        "Expected two shared communication providers"
    );

    static_assert(
        std::is_same_v<PlatformComposition::Select<SystemClockRequirement, Framework::SelectUnique>, ClockProvider>,
        "Expected ClockProvider to resolve as the SystemClock provider"
    );

    static_assert(
        PlatformComposition::Matches<CommunicationRequirement>::Count == 2U,
        "Expected both shared communication providers to be returned"
    );

    static_assert(
        PlatformComposition::Matches<CommunicationRequirement>::Contains<WifiProvider>,
        "Expected WifiProvider in the shared communication provider list"
    );

    static_assert(
        PlatformComposition::Matches<CommunicationRequirement>::Contains<BluetoothProvider>,
        "Expected BluetoothProvider in the shared communication provider list"
    );

    static_assert(
        PlatformComposition::HasProperty<SystemClock, ClockResolutionNanoseconds>,
        "Expected the SystemClock provider to advertise its resolution"
    );

    static_assert(
        PlatformComposition::PropertyValue<SystemClock, ClockResolutionNanoseconds> == 1000U,
        "Expected the SystemClock resolution to be preserved"
    );

    static_assert(
        PlatformComposition::PropertiesForProvider<WifiProvider, CommunicationInterface>::template Value<MaximumPayloadBytes> == 1500U,
        "Expected provider-specific property lookup to preserve the Wi-Fi payload value"
    );

    static_assert(
        PlatformComposition::MatchCount<WifiFrequencyRequirement> == 1U,
        "Expected exactly one communication provider at the requested frequency"
    );

    static_assert(
        std::is_same_v<
            PlatformComposition::Select<WifiFrequencyRequirement, Framework::SelectUnique>,
            WifiProvider
        >,
        "Expected qualified numeric Attribute resolution to select WifiProvider"
    );

    static_assert(
        std::is_same_v<
            PlatformComposition::Select<BluetoothTransportRequirement, Framework::SelectUnique>,
            BluetoothProvider
        >,
        "Expected qualified text Attribute resolution to select BluetoothProvider"
    );

    static_assert(
        PlatformComposition::MatchCount<SharedBandRequirement> == 2U,
        "Expected the shared-band requirement to remain intentionally ambiguous"
    );

    static_assert(
        PlatformComposition::Matches<SharedBandRequirement>::Count == 2U,
        "Expected plural qualified resolution to return both providers"
    );

    static_assert(
        PlatformComposition::MatchCount<MissingFrequencyRequirement> == 0U,
        "Expected an unmatched qualified requirement to report zero providers"
    );

    static_assert(
        !PlatformComposition::HasMatch<MissingFrequencyRequirement>,
        "Expected an unmatched qualified requirement to report no satisfying provider"
    );

    static_assert(
        PlatformComposition::MatchCount<PrimaryPurposeRequirement> == 1U,
        "Expected a provider missing the requested Attribute to be filtered out rather than rejected"
    );

    static_assert(
        std::is_same_v<
            PlatformComposition::Select<PrimaryPurposeRequirement, Framework::SelectUnique>,
            WifiProvider
        >,
        "Expected the provider advertising the requested optional Attribute to resolve"
    );

    static_assert(
        PlatformComposition::MatchCount<PreferredInterfaceRequirement> == 1U,
        "Expected a provider missing the requested Property to be filtered out rather than rejected"
    );

    static_assert(
        std::is_same_v<
            PlatformComposition::Select<PreferredInterfaceRequirement, Framework::SelectUnique>,
            WifiProvider
        >,
        "Expected the provider advertising the requested optional Property to resolve"
    );

    static_assert(
        PlatformComposition::AttributesForProvider<WifiProvider, CommunicationInterface>::template Matches<"Frequency", 24917>,
        "Expected provider-specific Attribute lookup to preserve the numeric descriptor"
    );

    static_assert(
        PlatformComposition::AttributesForProvider<BluetoothProvider, CommunicationInterface>::template Matches<
            "Transport",
            Framework::Text<"Bluetooth">
        >,
        "Expected provider-specific Attribute lookup to preserve the text descriptor"
    );

    static_assert(
        TestArchitecture::IsValid,
        "Expected cross-domain dependencies to be satisfied"
    );

    static_assert(
        TestArchitecture::MatchCount<WifiFrequencyRequirement> == 1U,
        "Expected architecture-level qualified provider counting to find the Wi-Fi provider"
    );

    static_assert(
        std::is_same_v<
            TestArchitecture::Select<WifiFrequencyRequirement, Framework::SelectUnique>,
            WifiProvider
        >,
        "Expected architecture-level qualified resolution to return WifiProvider"
    );

} // ESPressio::System::Tests::Composition


/// Executes the host-side compile-time Composition Framework tests.
int main() {
    return 0;
}
