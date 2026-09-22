#include <cstddef>
#include <cstdint>

#include <ESPressio_System.hpp>

namespace Demo {

    namespace Framework = ESPressio::System::CompositionFramework;

    /// Finite Radio feature vocabulary advertised by the demonstration providers.
    enum class RadioFeature : std::uint8_t {
        Broadcast = 0,
        ReceiveTimestamp = 1,
        LowEnergy = 2
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


    /// Broadcast-capable Radio provider which also supplies receive timestamps.
    class TimestampedRadio final : public Framework::Provider<
        RadioDomain,
        Framework::Provides<
            Framework::Offer<
                Radio,
                Framework::FlagPropertyValue<
                    SupportedFeatures,
                    RadioFeature::Broadcast,
                    RadioFeature::ReceiveTimestamp
                >
            >
        >
    > {

        private:

            // Demonstration runtime state.

            /// Number of Broadcast operations received by this provider.
            std::size_t _broadcastCount = 0U;


        public:

            // Demonstration Radio operations.

            /// Records one Broadcast operation.
            void Broadcast() noexcept {
                ++_broadcastCount;
            }

            /// Returns the number of Broadcast operations received by this provider.
            std::size_t BroadcastCount() const noexcept {
                return _broadcastCount;
            }

    };


    /// Second Broadcast-capable Radio provider.
    class BasicBroadcastRadio final : public Framework::Provider<
        RadioDomain,
        Framework::Provides<
            Framework::Offer<
                Radio,
                Framework::FlagPropertyValue<
                    SupportedFeatures,
                    RadioFeature::Broadcast
                >
            >
        >
    > {

        private:

            // Demonstration runtime state.

            /// Number of Broadcast operations received by this provider.
            std::size_t _broadcastCount = 0U;


        public:

            // Demonstration Radio operations.

            /// Records one Broadcast operation.
            void Broadcast() noexcept {
                ++_broadcastCount;
            }

            /// Returns the number of Broadcast operations received by this provider.
            std::size_t BroadcastCount() const noexcept {
                return _broadcastCount;
            }

    };


    /// Radio provider which does not advertise Broadcast support.
    class LowEnergyOnlyRadio final : public Framework::Provider<
        RadioDomain,
        Framework::Provides<
            Framework::Offer<
                Radio,
                Framework::FlagPropertyValue<
                    SupportedFeatures,
                    RadioFeature::LowEnergy
                >
            >
        >
    > {

        private:

            // Demonstration runtime state.

            /// Number of operations received by this intentionally excluded provider.
            std::size_t _operationCount = 0U;


        public:

            // Demonstration inspection.

            /// Returns the number of operations received by this provider.
            std::size_t OperationCount() const noexcept {
                return _operationCount;
            }

    };


    /// Requirement selecting every Radio provider which advertises Broadcast support.
    using BroadcastRequirement = Framework::Need<
        Radio,
        Framework::HasAllFlags<
            SupportedFeatures,
            RadioFeature::Broadcast
        >
    >;


    /// Complete Radio-domain Composition selected by the application.
    using RadioComposition = Framework::Composition<
        RadioDomain,
        TimestampedRadio,
        BasicBroadcastRadio,
        LowEnergyOnlyRadio
    >;


    /// Every provider type satisfying the Broadcast requirement.
    using BroadcastProviderTypes = RadioComposition::ProvidersSatisfying<
        BroadcastRequirement
    >;

    /// Non-owning runtime bindings for every provider selected by BroadcastProviderTypes.
    using BroadcastProviderReferences = Framework::ProviderReferences<
        BroadcastProviderTypes
    >;


    /// Consumer which knows only the plural Broadcast provider contract, not concrete provider names.
    class BroadcastConsumer final {

        private:

            // Injected provider bindings.

            /// Non-owning references to every Broadcast-capable Radio selected by Composition.
            BroadcastProviderReferences _radios;


        public:

            // Construction.

            /// Creates the consumer from already-bound Bootstrap-owned Broadcast providers.
            ///
            /// @param radios Non-owning references to every selected Broadcast provider.
            explicit BroadcastConsumer(
                BroadcastProviderReferences radios
            ) noexcept :
                _radios(radios) {}


            // Broadcast operation.

            /// Performs one Broadcast operation across every injected Radio provider.
            void BroadcastAll() noexcept {
                _radios.ForEach(
                    [](auto& radio) {
                        radio.Broadcast();
                    }
                );
            }

    };


    /// Application Bootstrap owning every concrete Radio and wiring the plural Broadcast subset.
    class ApplicationRuntime final {

        private:

            // Bootstrap-owned providers.

            /// First Broadcast-capable Radio instance.
            TimestampedRadio _timestampedRadio;

            /// Second Broadcast-capable Radio instance.
            BasicBroadcastRadio _basicBroadcastRadio;

            /// Non-Broadcast Radio instance retained by the application but excluded from Broadcast injection.
            LowEnergyOnlyRadio _lowEnergyOnlyRadio;

            // Runtime consumer.

            /// Consumer receiving only the Radio instances selected by BroadcastRequirement.
            BroadcastConsumer _broadcastConsumer;


        public:

            // Construction.

            /// Binds the compile-time Broadcast provider list to the corresponding Bootstrap-owned instances.
            ApplicationRuntime() noexcept :
                _broadcastConsumer(
                    BroadcastProviderReferences(
                        _timestampedRadio,
                        _basicBroadcastRadio
                    )
                ) {}


            // Demonstration execution.

            /// Broadcasts once and verifies that only the two selected providers receive the operation.
            int Run() noexcept {
                _broadcastConsumer.BroadcastAll();

                return _timestampedRadio.BroadcastCount() == 1U &&
                    _basicBroadcastRadio.BroadcastCount() == 1U &&
                    _lowEnergyOnlyRadio.OperationCount() == 0U
                        ? 0
                        : 1;
            }

    };


    static_assert(
        BroadcastProviderTypes::Count == 2U,
        "Exactly two Radio providers must satisfy the Broadcast requirement"
    );


    /// Executes the plural provider-reference demonstration.
    int Run() noexcept {
        ApplicationRuntime application;

        return application.Run();
    }

} // Demo


/// Runs the demonstration from the ESP-IDF application entrypoint.
extern "C" void app_main() {
    static_cast<void>(
        Demo::Run()
    );
}
