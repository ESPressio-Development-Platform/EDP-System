#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "ESPressio_System.hpp"

namespace ESPressio::System::Tests::ProviderReferences {

    namespace Framework = ESPressio::System::CompositionFramework;

    /// Finite Radio feature vocabulary used by the provider-reference tests.
    enum class RadioFeature : std::uint8_t {
        Broadcast = 0,
        ReceiveTimestamp = 1,
        LowEnergy = 2
    };


    /// Composition domain used by the provider-reference tests.
    struct RadioDomain final : Framework::Domain {};


    /// Shared Radio capability used to validate plural provider binding.
    struct Radio final : Framework::SharedCapability<RadioDomain> {};


    /// Typed feature set advertised by each test Radio provider.
    struct SupportedFeatures final : Framework::FlagProperty<
        Radio,
        RadioFeature,
        std::uint8_t
    > {};


    /// First Radio provider satisfying the Broadcast requirement.
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

            // Test runtime state.

            /// Number of Broadcast operations observed by this provider.
            std::size_t _broadcastCount = 0U;


        public:

            // Test provider operations.

            /// Records one Broadcast operation.
            void Broadcast() noexcept {
                ++_broadcastCount;
            }

            /// Returns the number of Broadcast operations observed by this provider.
            std::size_t BroadcastCount() const noexcept {
                return _broadcastCount;
            }

    };


    /// Second Radio provider satisfying the Broadcast requirement.
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

            // Test runtime state.

            /// Number of Broadcast operations observed by this provider.
            std::size_t _broadcastCount = 0U;


        public:

            // Test provider operations.

            /// Records one Broadcast operation.
            void Broadcast() noexcept {
                ++_broadcastCount;
            }

            /// Returns the number of Broadcast operations observed by this provider.
            std::size_t BroadcastCount() const noexcept {
                return _broadcastCount;
            }

    };


    /// Radio provider intentionally excluded from the Broadcast requirement.
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

            // Test runtime state.

            /// Number of Broadcast operations observed by this provider.
            std::size_t _broadcastCount = 0U;


        public:

            // Test provider operations.

            /// Records one Broadcast operation if this provider is ever invoked directly by the test.
            void Broadcast() noexcept {
                ++_broadcastCount;
            }

            /// Returns the number of Broadcast operations observed by this provider.
            std::size_t BroadcastCount() const noexcept {
                return _broadcastCount;
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


    /// Complete Radio Composition used to derive the plural Broadcast provider list.
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

    /// Non-owning runtime reference collection for every Broadcast provider.
    using BroadcastProviderReferences = Framework::ProviderReferences<
        BroadcastProviderTypes
    >;


    static_assert(
        BroadcastProviderTypes::Count == 2U,
        "Expected exactly two providers to satisfy the Broadcast requirement"
    );

    static_assert(
        BroadcastProviderReferences::Count == 2U,
        "Expected ProviderReferences Count to match its ProviderList"
    );

    static_assert(
        std::is_same_v<
            BroadcastProviderReferences::ProviderTypes,
            BroadcastProviderTypes
        >,
        "Expected ProviderReferences to preserve its exact ProviderList type"
    );

    static_assert(
        std::is_constructible_v<
            BroadcastProviderReferences,
            TimestampedRadio&,
            BasicBroadcastRadio&
        >,
        "Expected exact ProviderList-ordered provider references to be constructible"
    );

    static_assert(
        !std::is_constructible_v<
            BroadcastProviderReferences,
            BasicBroadcastRadio&,
            TimestampedRadio&
        >,
        "Expected reversed provider order not to satisfy the exact binding constructor"
    );

    static_assert(
        !std::is_constructible_v<
            BroadcastProviderReferences,
            TimestampedRadio&,
            LowEnergyOnlyRadio&
        >,
        "Expected a provider outside the resolved ProviderList not to satisfy the binding constructor"
    );

} // ESPressio::System::Tests::ProviderReferences


/// Executes the host-side ProviderReferences tests.
int main() {
    using namespace ESPressio::System::Tests::ProviderReferences;

    TimestampedRadio timestampedRadio;
    BasicBroadcastRadio basicBroadcastRadio;
    LowEnergyOnlyRadio lowEnergyOnlyRadio;

    BroadcastProviderReferences broadcastRadios(
        timestampedRadio,
        basicBroadcastRadio
    );

    std::size_t traversalIndex = 0U;
    bool traversalOrderIsCorrect = true;

    broadcastRadios.ForEach(
        [&traversalIndex, &traversalOrderIsCorrect](auto& radio) {
            using ProviderType = std::remove_cvref_t<decltype(radio)>;

            if constexpr (std::is_same_v<ProviderType, TimestampedRadio>) {
                traversalOrderIsCorrect =
                    traversalOrderIsCorrect &&
                    traversalIndex == 0U;
            } else if constexpr (std::is_same_v<ProviderType, BasicBroadcastRadio>) {
                traversalOrderIsCorrect =
                    traversalOrderIsCorrect &&
                    traversalIndex == 1U;
            }

            ++traversalIndex;
            radio.Broadcast();
        }
    );

    const auto& constBroadcastRadios = broadcastRadios;
    std::size_t observedBroadcasts = 0U;

    constBroadcastRadios.ForEach(
        [&observedBroadcasts](const auto& radio) {
            observedBroadcasts += radio.BroadcastCount();
        }
    );

    Framework::ProviderReferences<
        Framework::ProviderList<>
    > emptyProviders;

    std::size_t emptyTraversalCount = 0U;

    emptyProviders.ForEach(
        [&emptyTraversalCount](auto&) {
            ++emptyTraversalCount;
        }
    );

    const auto& constEmptyProviders = emptyProviders;

    constEmptyProviders.ForEach(
        [&emptyTraversalCount](const auto&) {
            ++emptyTraversalCount;
        }
    );

    return traversalOrderIsCorrect &&
        traversalIndex == 2U &&
        timestampedRadio.BroadcastCount() == 1U &&
        basicBroadcastRadio.BroadcastCount() == 1U &&
        lowEnergyOnlyRadio.BroadcastCount() == 0U &&
        observedBroadcasts == 2U &&
        emptyTraversalCount == 0U
            ? 0
            : 1;
}
