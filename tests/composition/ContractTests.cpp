#include <cstdint>
#include <type_traits>

#include "ESPressio_System.hpp"

namespace ESPressio::System::Tests::CompositionContracts {

    namespace Framework = ESPressio::System::CompositionFramework;


    // Test Domains.

    /// Platform-like Domain used by consolidated Contract tests.
    struct PlatformDomain final : Framework::Domain {};

    /// Service-like Domain used by cross-domain Contract tests.
    struct ServiceDomain final : Framework::Domain {};


    // Test Capabilities.

    /// Exclusive clock Capability.
    struct Clock final : Framework::ExclusiveCapability<PlatformDomain> {};

    /// Shared radio Capability.
    struct Radio final : Framework::SharedCapability<PlatformDomain> {};

    /// Shared diagnostics Capability which may be co-offered with Radio.
    struct Diagnostics final : Framework::SharedCapability<PlatformDomain> {};

    /// Exclusive coordinator Capability consuming several Platform facilities.
    struct Coordinator final : Framework::ExclusiveCapability<PlatformDomain> {};

    /// Exclusive service Capability consuming Platform providers.
    struct Telemetry final : Framework::ExclusiveCapability<ServiceDomain> {};


    // Test Properties.

    /// Clock resolution in nanoseconds.
    struct ResolutionNanoseconds final : Framework::Property<
        Clock,
        std::uint32_t
    > {};

    /// Radio operation latency used for deterministic best-match selection.
    struct LatencyMicroseconds final : Framework::Property<
        Radio,
        std::uint32_t
    > {};

    /// Radio maximum payload bytes.
    struct MaximumPayloadBytes final : Framework::Property<
        Radio,
        std::uint16_t
    > {};

    /// Optional preferred-provider marker.
    struct Preferred final : Framework::Property<
        Radio,
        bool
    > {};


    // Reusable Requirements.

    /// Unique same-domain Clock requirement used by the coordinator.
    using LocalClock = Framework::Requirement<
        Clock,
        Framework::RequirementScope::SameDomain,
        Framework::ExactlyProviders<1U>,
        Framework::AtMost<
            ResolutionNanoseconds,
            1000U
        >
    >;

    /// Broad same-domain Radio requirement requiring at least two providers.
    using LocalRadios = Framework::Requirement<
        Radio,
        Framework::RequirementScope::SameDomain,
        Framework::AtLeastProviders<2U>,
        Framework::HasProperty<LatencyMicroseconds>
    >;

    /// Same-domain Wi-Fi Radio qualification.
    using LocalWifiRadio = Framework::Requirement<
        Radio,
        Framework::RequirementScope::SameDomain,
        Framework::AtLeastProviders<1U>,
        Framework::TextAttributeEquals<
            "Transport",
            "WiFi"
        >
    >;

    /// Same-domain BLE Radio qualification.
    using LocalBleRadio = Framework::Requirement<
        Radio,
        Framework::RequirementScope::SameDomain,
        Framework::AtLeastProviders<1U>,
        Framework::TextAttributeEquals<
            "Transport",
            "BLE"
        >
    >;

    /// Same-domain Diagnostics qualification.
    using LocalDiagnostics = Framework::Requirement<
        Diagnostics,
        Framework::RequirementScope::SameDomain
    >;

    /// Cross-domain Radio population required by the Telemetry provider.
    using ExternalRadios = Framework::Requirement<
        Radio,
        Framework::RequirementScope::ExternalDomain,
        Framework::AtLeastProviders<3U>,
        Framework::HasProperty<MaximumPayloadBytes>
    >;

    /// Cross-domain Wi-Fi Radio qualification.
    using ExternalWifiRadio = Framework::Requirement<
        Radio,
        Framework::RequirementScope::ExternalDomain,
        Framework::AtLeastProviders<1U>,
        Framework::TextAttributeEquals<
            "Transport",
            "WiFi"
        >
    >;

    /// Cross-domain BLE Radio qualification.
    using ExternalBleRadio = Framework::Requirement<
        Radio,
        Framework::RequirementScope::ExternalDomain,
        Framework::AtLeastProviders<1U>,
        Framework::TextAttributeEquals<
            "Transport",
            "BLE"
        >
    >;


    // Test Providers.

    /// Unique Clock provider.
    struct ClockProvider final : Framework::Provider<
        PlatformDomain,
        Framework::Offers<
            Framework::Offer<
                Clock,
                Framework::PropertyValue<
                    ResolutionNanoseconds,
                    500U
                >
            >
        >
    > {};


    /// Radio provider which also supplies Diagnostics.
    struct WifiRadioProvider final : Framework::Provider<
        PlatformDomain,
        Framework::Offers<
            Framework::Offer<
                Radio,
                Framework::PropertyValue<
                    LatencyMicroseconds,
                    8U
                >,
                Framework::PropertyValue<
                    MaximumPayloadBytes,
                    512U
                >,
                Framework::PropertyValue<
                    Preferred,
                    true
                >,
                Framework::TextAttribute<
                    "Transport",
                    "WiFi"
                >
            >,
            Framework::Offer<Diagnostics>
        >
    > {};


    /// BLE Radio provider with the largest payload and tied-lowest latency.
    struct BleRadioProvider final : Framework::Provider<
        PlatformDomain,
        Framework::Offers<
            Framework::Offer<
                Radio,
                Framework::PropertyValue<
                    LatencyMicroseconds,
                    4U
                >,
                Framework::PropertyValue<
                    MaximumPayloadBytes,
                    1024U
                >,
                Framework::TextAttribute<
                    "Transport",
                    "BLE"
                >
            >
        >
    > {};


    /// ESP-NOW-like Radio provider tied for the lowest latency.
    struct EspNowRadioProvider final : Framework::Provider<
        PlatformDomain,
        Framework::Offers<
            Framework::Offer<
                Radio,
                Framework::PropertyValue<
                    LatencyMicroseconds,
                    4U
                >,
                Framework::PropertyValue<
                    MaximumPayloadBytes,
                    256U
                >,
                Framework::TextAttribute<
                    "Transport",
                    "ESPNow"
                >
            >
        >
    > {};


    /// Provider exercising local cardinality, same-provider, distinct-provider and lifecycle Contracts.
    struct CoordinatorProvider final : Framework::Provider<
        PlatformDomain,
        Framework::Offers<
            Framework::Offer<Coordinator>
        >,
        Framework::Contract<
            LocalClock,
            LocalRadios,
            Framework::SameProvider<
                LocalWifiRadio,
                LocalDiagnostics
            >,
            Framework::DistinctProviders<
                LocalWifiRadio,
                LocalBleRadio
            >,
            Framework::InitializesAfter<LocalClock>,
            Framework::ShutsDownBefore<LocalClock>
        >
    > {};


    /// Cross-domain consumer provider exercising external cardinality and distinct assignment.
    struct TelemetryProvider final : Framework::Provider<
        ServiceDomain,
        Framework::Offers<
            Framework::Offer<Telemetry>
        >,
        Framework::Contract<
            ExternalRadios,
            Framework::DistinctProviders<
                ExternalWifiRadio,
                ExternalBleRadio
            >
        >
    > {};


    // Compositions and Architecture.

    /// Complete Platform-domain Composition.
    using PlatformComposition = Framework::Composition<
        PlatformDomain,
        ClockProvider,
        WifiRadioProvider,
        BleRadioProvider,
        EspNowRadioProvider,
        CoordinatorProvider
    >;

    /// Complete Service-domain Composition.
    using ServiceComposition = Framework::Composition<
        ServiceDomain,
        TelemetryProvider
    >;

    /// Complete multi-domain test Architecture.
    using TestArchitecture = Framework::Architecture<
        PlatformComposition,
        ServiceComposition
    >;


    // Consumer Requirements.

    /// Architecture-level Radio qualification exercising logical composition and presence checks.
    using ConsumerRadioRequirement = Framework::Requirement<
        Radio,
        Framework::RequirementScope::AnyDomain,
        Framework::AtLeastProviders<2U>,
        Framework::HasProperty<LatencyMicroseconds>,
        Framework::AnyOf<
            Framework::AtMost<
                LatencyMicroseconds,
                4U
            >,
            Framework::Equals<
                Preferred,
                true
            >
        >,
        Framework::Not<
            Framework::TextAttributeEquals<
                "Transport",
                "Unavailable"
            >
        >
    >;

    /// Requirement which matches no provider and is intentionally optional.
    using MissingRadioRequirement = Framework::Requirement<
        Radio,
        Framework::RequirementScope::AnyDomain,
        Framework::AnyProviderCount,
        Framework::TextAttributeEquals<
            "Transport",
            "Missing"
        >
    >;

    /// Consumer-level Wi-Fi qualification.
    using ConsumerWifiRadio = Framework::Requirement<
        Radio,
        Framework::RequirementScope::AnyDomain,
        Framework::AtLeastProviders<1U>,
        Framework::TextAttributeEquals<
            "Transport",
            "WiFi"
        >
    >;

    /// Consumer-level BLE qualification.
    using ConsumerBleRadio = Framework::Requirement<
        Radio,
        Framework::RequirementScope::AnyDomain,
        Framework::AtLeastProviders<1U>,
        Framework::TextAttributeEquals<
            "Transport",
            "BLE"
        >
    >;

    /// Consumer-level Diagnostics qualification.
    using ConsumerDiagnostics = Framework::Requirement<
        Diagnostics,
        Framework::RequirementScope::AnyDomain
    >;

    /// Reusable standalone consumer Contract.
    using ConsumerContract = Framework::Contract<
        ConsumerRadioRequirement,
        Framework::SameProvider<
            ConsumerWifiRadio,
            ConsumerDiagnostics
        >,
        Framework::DistinctProviders<
            ConsumerWifiRadio,
            ConsumerBleRadio
        >
    >;


    // Provider/Architecture validation.

    static_assert(
        PlatformComposition::IsValid,
        "Consolidated same-domain Contracts must validate"
    );

    static_assert(
        TestArchitecture::IsValid,
        "Consolidated cross-domain Contracts must validate"
    );

    static_assert(
        TestArchitecture::SatisfiesContract<ConsumerContract>,
        "Architecture must satisfy the reusable consumer Contract"
    );

    static_assert(
        TestArchitecture::ValidateContract<ConsumerContract>::IsValid,
        "Strict consumer Contract validation must succeed"
    );


    // Lifecycle ordering.

    static_assert(
        TestArchitecture::InitializationOrder::template IndexOf<ClockProvider> <
        TestArchitecture::InitializationOrder::template IndexOf<CoordinatorProvider>,
        "InitializesAfter must place the selected predecessor before the owning provider"
    );

    static_assert(
        TestArchitecture::ShutdownOrder::template IndexOf<CoordinatorProvider> <
        TestArchitecture::ShutdownOrder::template IndexOf<ClockProvider>,
        "ShutsDownBefore must place the owning provider before the selected successor"
    );


    // Qualification and selection.

    static_assert(
        TestArchitecture::MatchCount<ConsumerRadioRequirement> == 3U,
        "All three Radios satisfy the consumer qualification"
    );

    static_assert(
        TestArchitecture::SatisfiesRequirement<ConsumerRadioRequirement>,
        "Consumer Radio cardinality must be satisfied"
    );

    using AllConsumerRadios = TestArchitecture::Select<
        ConsumerRadioRequirement,
        Framework::SelectAll
    >;

    static_assert(
        AllConsumerRadios::Count == 3U,
        "SelectAll must retain every qualifying provider"
    );

    using LowestLatencyRadios = TestArchitecture::Select<
        ConsumerRadioRequirement,
        Framework::SelectAllMinimum<
            LatencyMicroseconds
        >
    >;

    static_assert(
        LowestLatencyRadios::Count == 2U,
        "Plural best-match selection must retain tied minimum providers"
    );

    static_assert(
        LowestLatencyRadios::template Contains<BleRadioProvider> &&
        LowestLatencyRadios::template Contains<EspNowRadioProvider>,
        "Plural minimum selection must retain both tied Radios"
    );

    using LargestPayloadRadio = TestArchitecture::Select<
        ConsumerRadioRequirement,
        Framework::SelectMaximum<
            MaximumPayloadBytes
        >
    >;

    static_assert(
        std::is_same_v<
            LargestPayloadRadio,
            BleRadioProvider
        >,
        "Maximum Property selection must resolve the unique largest payload"
    );

    using MissingOptionalRadio = TestArchitecture::Select<
        MissingRadioRequirement,
        Framework::SelectOptionalUnique
    >;

    static_assert(
        std::is_void_v<MissingOptionalRadio>,
        "Optional unique selection must resolve void when nothing matches"
    );


    // Match diagnostics.

    static_assert(
        Framework::ProviderMatch<
            WifiRadioProvider,
            ConsumerWifiRadio
        >::ProvidesCapability,
        "Wi-Fi provider must advertise Radio"
    );

    static_assert(
        Framework::ProviderMatch<
            WifiRadioProvider,
            ConsumerWifiRadio
        >::ConstraintsSatisfied,
        "Wi-Fi provider must satisfy the Wi-Fi qualification"
    );

    static_assert(
        !Framework::ProviderMatch<
            BleRadioProvider,
            ConsumerWifiRadio
        >::IsSatisfied,
        "BLE provider must not satisfy the Wi-Fi qualification"
    );


    // ProviderList algorithms.

    using FirstTwoRadios = Framework::ProviderList<
        WifiRadioProvider,
        BleRadioProvider
    >;

    using LastTwoRadios = Framework::ProviderList<
        BleRadioProvider,
        EspNowRadioProvider
    >;

    using ConcatenatedRadios = Framework::ProviderListConcat<
        FirstTwoRadios,
        Framework::ProviderList<EspNowRadioProvider>
    >;

    using CommonRadios = Framework::ProviderListIntersection<
        FirstTwoRadios,
        LastTwoRadios
    >;

    using WifiOnly = Framework::ProviderListDifference<
        FirstTwoRadios,
        LastTwoRadios
    >;

    static_assert(
        ConcatenatedRadios::Count == 3U &&
        std::is_same_v<
            ConcatenatedRadios::template At<0U>,
            WifiRadioProvider
        > &&
        std::is_same_v<
            ConcatenatedRadios::Back,
            EspNowRadioProvider
        >,
        "ProviderList concatenation/indexing must preserve declaration order"
    );

    static_assert(
        CommonRadios::Count == 1U &&
        CommonRadios::template Contains<BleRadioProvider>,
        "ProviderList intersection must preserve only common providers"
    );

    static_assert(
        WifiOnly::Count == 1U &&
        WifiOnly::template Contains<WifiRadioProvider>,
        "ProviderList difference must remove providers represented by the right list"
    );

} // ESPressio::System::Tests::CompositionContracts


/// Executes the consolidated compile-time Composition Contract tests.
int main() {
    return 0;
}
