#include <cstdint>
#include <type_traits>

#include "ESPressio_System.hpp"

namespace ESPressio::System::Tests::Composition {

    namespace Framework = ESPressio::System::CompositionFramework;

    /// Composition domain used by the host-side framework tests.
    struct PlatformDomain final : Framework::Domain {};


    /// Exclusive clock capability used by the host-side framework tests.
    struct SystemClock final : Framework::ExclusiveCapability<PlatformDomain> {};


    /// Exclusive scheduler capability used by the host-side framework tests.
    struct TaskScheduler final : Framework::ExclusiveCapability<PlatformDomain> {};


    /// Shared communication capability used by the host-side framework tests.
    struct CommunicationInterface final : Framework::SharedCapability<PlatformDomain> {};


    /// Resolution of the SystemClock capability in nanoseconds.
    struct ClockResolutionNanoseconds final : Framework::Property<SystemClock, std::uint32_t> {};


    /// Maximum payload supported by one CommunicationInterface provider.
    struct MaximumPayloadBytes final : Framework::Property<CommunicationInterface, std::uint16_t> {};


    /// Test provider supplying the SystemClock capability.
    struct ClockProvider final : Framework::Provider<
        PlatformDomain,
        Framework::Provides<
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
        Framework::Provides<
            Framework::Offer<TaskScheduler>
        >,
        Framework::Requires<
            Framework::Need<
                SystemClock,
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
        Framework::Provides<
            Framework::Offer<
                CommunicationInterface,
                Framework::PropertyValue<
                    MaximumPayloadBytes,
                    1500U
                >
            >
        >
    > {};


    /// Second test provider supplying the shared CommunicationInterface capability.
    struct BluetoothProvider final : Framework::Provider<
        PlatformDomain,
        Framework::Provides<
            Framework::Offer<
                CommunicationInterface,
                Framework::PropertyValue<
                    MaximumPayloadBytes,
                    512U
                >
            >
        >
    > {};


    /// Valid composition used to verify provider resolution and property inspection.
    using TestComposition = Framework::Composition<
        PlatformDomain,
        ClockProvider,
        SchedulerProvider,
        WifiProvider,
        BluetoothProvider
    >;


    static_assert(
        TestComposition::IsValid,
        "Expected the test composition to be valid"
    );

    static_assert(
        TestComposition::ProviderCount == 4U,
        "Expected every provider to be represented in the composition"
    );

    static_assert(
        TestComposition::Provides<SystemClock>,
        "Expected the composition to provide SystemClock"
    );

    static_assert(
        TestComposition::Provides<TaskScheduler>,
        "Expected the composition to provide TaskScheduler"
    );

    static_assert(
        TestComposition::ProviderCountFor<CommunicationInterface> == 2U,
        "Expected two shared communication providers"
    );

    static_assert(
        std::is_same_v<TestComposition::ProviderFor<SystemClock>, ClockProvider>,
        "Expected ClockProvider to resolve as the SystemClock provider"
    );

    static_assert(
        TestComposition::ProvidersFor<CommunicationInterface>::Count == 2U,
        "Expected both shared communication providers to be returned"
    );

    static_assert(
        TestComposition::ProvidersFor<CommunicationInterface>::Contains<WifiProvider>,
        "Expected WifiProvider in the shared communication provider list"
    );

    static_assert(
        TestComposition::ProvidersFor<CommunicationInterface>::Contains<BluetoothProvider>,
        "Expected BluetoothProvider in the shared communication provider list"
    );

    static_assert(
        TestComposition::HasProperty<SystemClock, ClockResolutionNanoseconds>,
        "Expected the SystemClock provider to advertise its resolution"
    );

    static_assert(
        TestComposition::PropertyValue<SystemClock, ClockResolutionNanoseconds> == 1000U,
        "Expected the SystemClock resolution to be preserved"
    );

    static_assert(
        TestComposition::PropertiesForProvider<WifiProvider, CommunicationInterface>::template Value<MaximumPayloadBytes> == 1500U,
        "Expected provider-specific property lookup to preserve the Wi-Fi payload value"
    );

} // ESPressio::System::Tests::Composition


/// Executes the host-side compile-time Composition Framework tests.
int main() {
    return 0;
}
