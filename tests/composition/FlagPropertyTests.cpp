#include <cstdint>
#include <type_traits>

#include "ESPressio_System.hpp"

namespace ESPressio::System::Tests::CompositionFlagProperty {

    namespace Framework = ESPressio::System::CompositionFramework;

    /// Composition domain used by the flag-property tests.
    struct RadioDomain final : Framework::Domain {};


    /// Shared Radio capability used to validate typed feature-set provider matching.
    struct Radio final : Framework::SharedCapability<RadioDomain> {};


    /// Compile-time feature vocabulary supported by one Radio provider.
    enum class RadioFeature : std::uint8_t {
        Broadcast = 0,
        ReliableDelivery = 1,
        ReceiveTimestamp = 2,
        LowEnergy = 3
    };


    /// Unrelated enum domain used to verify type-domain discrimination.
    enum class ForeignFeature : std::uint8_t {
        SomethingElse = 0
    };


    /// Typed finite feature set advertised for the Radio capability.
    struct SupportedFeatures final : Framework::FlagProperty<
        Radio,
        RadioFeature,
        std::uint8_t
    > {};


    /// Ordinary Property used to confirm flag-property inspection remains selective.
    struct MaximumPayloadBytes final : Framework::Property<
        Radio,
        std::uint16_t
    > {};


    /// Radio provider supporting broadcast, reliable delivery, and receive timestamps.
    struct TimestampedRadioProvider final : Framework::Provider<
        RadioDomain,
        Framework::Provides<
            Framework::Offer<
                Radio,
                Framework::FlagPropertyValue<
                    SupportedFeatures,
                    RadioFeature::Broadcast,
                    RadioFeature::ReliableDelivery,
                    RadioFeature::ReceiveTimestamp
                >,
                Framework::PropertyValue<
                    MaximumPayloadBytes,
                    512U
                >
            >
        >
    > {};


    /// Radio provider supporting broadcast and low-energy operation.
    struct LowEnergyRadioProvider final : Framework::Provider<
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


    /// Radio provider intentionally omitting the optional SupportedFeatures Property.
    struct UnqualifiedRadioProvider final : Framework::Provider<
        RadioDomain,
        Framework::Provides<
            Framework::Offer<Radio>
        >
    > {};


    /// Requirement demanding both reliable delivery and receive timestamp support.
    using ReliableTimestampRequirement = Framework::Need<
        Radio,
        Framework::HasAllFlags<
            SupportedFeatures,
            RadioFeature::ReliableDelivery,
            RadioFeature::ReceiveTimestamp
        >
    >;


    /// Requirement accepting either receive timestamp or low-energy support.
    using TimestampOrLowEnergyRequirement = Framework::Need<
        Radio,
        Framework::HasAnyFlags<
            SupportedFeatures,
            RadioFeature::ReceiveTimestamp,
            RadioFeature::LowEnergy
        >
    >;


    /// Requirement excluding providers which advertise low-energy operation.
    using NoLowEnergyRequirement = Framework::Need<
        Radio,
        Framework::HasNoFlags<
            SupportedFeatures,
            RadioFeature::LowEnergy
        >
    >;


    /// Complete Radio Composition used by the typed flag-property tests.
    using RadioComposition = Framework::Composition<
        RadioDomain,
        TimestampedRadioProvider,
        LowEnergyRadioProvider,
        UnqualifiedRadioProvider
    >;


    static_assert(
        Framework::IsFlagPropertyV<SupportedFeatures>,
        "Expected SupportedFeatures to be recognised as a concrete FlagProperty"
    );

    static_assert(
        !Framework::IsFlagPropertyV<MaximumPayloadBytes>,
        "Expected an ordinary Property not to be classified as a FlagProperty"
    );

    static_assert(
        Framework::Detail::FlagArgumentsUsePropertyDomain<
            SupportedFeatures,
            RadioFeature::Broadcast,
            RadioFeature::ReceiveTimestamp
        >(),
        "Expected RadioFeature values to belong to the SupportedFeatures flag domain"
    );

    static_assert(
        !Framework::Detail::FlagArgumentsUsePropertyDomain<
            SupportedFeatures,
            ForeignFeature::SomethingElse
        >(),
        "Expected an unrelated enum domain to be rejected by flag-property validation"
    );

    static_assert(
        Framework::Detail::FlagArgumentsAreRepresentable<
            SupportedFeatures,
            RadioFeature::LowEnergy
        >(),
        "Expected every defined RadioFeature to fit the selected one-byte storage"
    );

    static_assert(
        Framework::FlagPropertyValue<
            SupportedFeatures,
            RadioFeature::Broadcast,
            RadioFeature::ReliableDelivery,
            RadioFeature::ReceiveTimestamp
        >::Value == 0x07U,
        "Expected typed flag advertisement to preserve the exact one-byte representation"
    );

    static_assert(
        Framework::FlagPropertyValue<SupportedFeatures>::Value == 0U,
        "Expected an empty typed flag advertisement to produce a zero representation"
    );

    static_assert(
        RadioComposition::PropertiesForProvider<
            TimestampedRadioProvider,
            Radio
        >::template Value<SupportedFeatures> == 0x07U,
        "Expected existing PropertySet queries to preserve flag-valued Property storage"
    );

    static_assert(
        RadioComposition::ProviderCountSatisfying<ReliableTimestampRequirement> == 1U,
        "Expected HasAllFlags to retain only the provider advertising every requested feature"
    );

    static_assert(
        std::is_same_v<
            RadioComposition::ProviderSatisfying<ReliableTimestampRequirement>,
            TimestampedRadioProvider
        >,
        "Expected HasAllFlags provider resolution to select TimestampedRadioProvider"
    );

    static_assert(
        RadioComposition::ProviderCountSatisfying<TimestampOrLowEnergyRequirement> == 2U,
        "Expected HasAnyFlags to retain providers advertising either requested feature"
    );

    static_assert(
        RadioComposition::ProvidersSatisfying<TimestampOrLowEnergyRequirement>::Count == 2U,
        "Expected plural provider resolution to preserve both HasAnyFlags matches"
    );

    static_assert(
        RadioComposition::ProviderCountSatisfying<NoLowEnergyRequirement> == 1U,
        "Expected HasNoFlags to reject LowEnergy and missing-Property providers"
    );

    static_assert(
        std::is_same_v<
            RadioComposition::ProviderSatisfying<NoLowEnergyRequirement>,
            TimestampedRadioProvider
        >,
        "Expected HasNoFlags provider resolution to select the non-low-energy qualified provider"
    );

} // ESPressio::System::Tests::CompositionFlagProperty


/// Executes the host-side compile-time Composition flag-property tests.
int main() {
    return 0;
}
