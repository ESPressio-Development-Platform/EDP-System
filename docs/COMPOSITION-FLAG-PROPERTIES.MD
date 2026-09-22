# Composition Flag Properties

## Purpose

Composition flag properties describe a finite set of orthogonal compile-time features belonging to one capability.

They are appropriate when a provider can support several independent characteristics simultaneously and a consumer needs to qualify providers by asking whether all, any, or none of a named set of features are present.

The facility is deliberately built on the existing Composition Property model. It does **not** replace Capabilities, Providers, Offers, Needs, Attributes, or provider resolution.

## Architectural boundary

Use a flag property when the question is:

> Which members of this finite feature vocabulary does this one capability support?

Do not use a flag property to collapse unrelated architectural services into one mask.

For example, this is appropriate:

~~~text
Radio
    Broadcast
    ReliableDelivery
    ReceiveTimestamp
    LowEnergy
~~~

These are facets of one Radio capability.

This is not appropriate:

~~~text
SystemFeatures
    Clock
    Persistence
    Radio
    Encryption
~~~

Clock, Persistence, Radio, and Encryption are separate architectural services and should remain separate Composition Capabilities.

The same distinction applies to runtime state. A flag property describes what the compiled provider is capable of. It must not be used for values such as current connection state, present battery condition, or live queue occupancy.

## Declaring a flag vocabulary

Flag enums use the same contract as `ESPressio::System::FlagSet`: each enum value is a **zero-based bit position**, not a pre-shifted mask.

~~~cpp
enum class RadioFeature : std::uint8_t {
    Broadcast = 0,
    ReliableDelivery = 1,
    ReceiveTimestamp = 2,
    LowEnergy = 3
};
~~~

A concrete flag-valued Property derives from `FlagProperty`:

~~~cpp
struct SupportedFeatures final : Composition::FlagProperty<
    Radio,
    RadioFeature,
    std::uint8_t
> {};
~~~

The template parameters are the capability being described, the enum defining the finite flag vocabulary, and the unsigned storage type used as the complete compile-time representation.

The storage type defaults to `std::uint32_t`, but constrained embedded contracts should deliberately select the smallest width which accommodates the stable vocabulary.

## Advertising provider features

A provider advertises features using `FlagPropertyValue`:

~~~cpp
class TimestampedRadio final : public Composition::Provider<
    RadioDomain,
    Composition::Provides<
        Composition::Offer<
            Radio,
            Composition::FlagPropertyValue<
                SupportedFeatures,
                RadioFeature::Broadcast,
                RadioFeature::ReliableDelivery,
                RadioFeature::ReceiveTimestamp
            >
        >
    >
> {};
~~~

No raw mask needs to be written or maintained.

An empty value is valid when a provider deliberately advertises the Property but supports none of its flags:

~~~cpp
Composition::FlagPropertyValue<SupportedFeatures>
~~~

That is different from omitting the Property entirely. Omitting a Property means that a constraint requiring that Property cannot be satisfied.

## Requiring all flags

`HasAllFlags` requires every named flag to be present:

~~~cpp
using ReliableTimestampRadio = Composition::Need<
    Radio,
    Composition::HasAllFlags<
        SupportedFeatures,
        RadioFeature::ReliableDelivery,
        RadioFeature::ReceiveTimestamp
    >
>;
~~~

## Requiring any flag

`HasAnyFlags` requires at least one named flag to be present:

~~~cpp
using TimestampOrLowEnergyRadio = Composition::Need<
    Radio,
    Composition::HasAnyFlags<
        SupportedFeatures,
        RadioFeature::ReceiveTimestamp,
        RadioFeature::LowEnergy
    >
>;
~~~

## Requiring flags to be absent

`HasNoFlags` requires every named flag to be clear:

~~~cpp
using NonLowEnergyRadio = Composition::Need<
    Radio,
    Composition::HasNoFlags<
        SupportedFeatures,
        RadioFeature::LowEnergy
    >
>;
~~~

A provider which does not advertise `SupportedFeatures` does **not** satisfy this constraint. Absence of the Property is not interpreted as authoritative evidence that every flag is clear.

## Provider resolution

Flag constraints participate in the existing Need/provider matching machinery. No special resolver exists for them.

Existing APIs continue to apply:

~~~text
ProviderCountSatisfying<TNeed>
HasProviderSatisfying<TNeed>
ProvidersSatisfying<TNeed>
ProviderSatisfying<TNeed>
~~~

This is intentional: flag-valued Properties extend qualification semantics without creating a parallel Composition model.

## Compatibility with ordinary Property queries

A `FlagProperty` stores its compile-time value as the selected unsigned integral storage type.

Consequently existing `PropertySet::Value<>` queries remain valid, and existing generic constraints such as `Equals<SupportedFeatures, ...>` remain available when raw representation equality is genuinely required.

For normal feature advertisement and matching, prefer `FlagPropertyValue` and the typed flag constraints because they preserve enum-domain checking and avoid manually maintained masks.

## Compile-time diagnostics

The typed helper surface rejects:

- a non-`FlagProperty` used with flag-specific helpers;
- flags from a different enum domain;
- flag positions which do not fit the Property storage type;
- empty `HasAllFlags`, `HasAnyFlags`, or `HasNoFlags` constraints.

These are architecture-definition errors and therefore intentionally fail compilation.

## Runtime and memory cost

The feature introduces no Composition-owned runtime object and performs no runtime allocation.

`FlagPropertyValue` reduces its typed flag list to the selected unsigned storage representation during compilation. The all/any/none constraints are evaluated during template instantiation as part of normal Composition provider matching.

Runtime cost is therefore zero.

## Relationship to runtime FlagSet

`FlagProperty` and `ESPressio::System::FlagSet` deliberately share the same flag vocabulary contract:

~~~text
enum value = zero-based bit position
~~~

The two uses remain distinct:

~~~text
Composition FlagProperty
    compile-time provider characteristic

FlagSet
    compact runtime value representation
~~~

A runtime state variable must not be moved into Composition merely because it uses the same enum.

## Complete demonstration

A buildable example is provided under:

~~~text
demos/composition-flag-properties/
~~~

with Arduino IDE, PIOArduino/PlatformIO Arduino, and PIOArduino/PlatformIO ESP-IDF targets.
