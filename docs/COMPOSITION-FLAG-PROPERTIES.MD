# Composition Flag Properties

## Purpose

`FlagProperty` represents a finite, strongly typed compile-time feature set advertised by a capability Offer.

It is useful when a provider supports several independent features and a Requirement needs to qualify providers by combinations of those features.

Flag Properties are compile-time metadata. They do not store runtime feature state.

## Declaring a Flag Vocabulary

Define an enum whose values identify the available features:

```cpp
enum class RadioFeature : std::uint8_t {
    Broadcast = 0,
    Acknowledgements = 1,
    ReceiveTimestamp = 2,
    LowEnergy = 3
};
```

Then declare the Property:

```cpp
struct SupportedFeatures final : Framework::FlagProperty<
    Radio,
    RadioFeature,
    std::uint8_t
> {};
```

The first template argument is the owning Capability, the second is the typed flag domain, and the third is the compact integral storage Type.

The Framework validates that advertised flags belong to the declared enum domain and fit the selected storage representation.

## Advertising Provider Features

A provider advertises a typed feature set through `FlagPropertyValue` inside an `Offer`:

```cpp
struct TimestampedRadio final : Framework::Provider<
    RadioDomain,
    Framework::Offers<
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
```

Another provider can advertise a different set:

```cpp
struct LowEnergyRadio final : Framework::Provider<
    RadioDomain,
    Framework::Offers<
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
```

An empty `FlagPropertyValue<SupportedFeatures>` has a zero representation.

## Requiring All Flags

`HasAllFlags` requires every supplied flag to be present:

```cpp
using ReliableTimestampRequirement = Framework::Requirement<
    Radio,
    Framework::RequirementScope::SameDomain,
    Framework::ExactlyProviders<1U>,
    Framework::HasAllFlags<
        SupportedFeatures,
        RadioFeature::Acknowledgements,
        RadioFeature::ReceiveTimestamp
    >
>;
```

A provider missing either requested feature does not match.

## Requiring Any Flag

`HasAnyFlags` requires at least one supplied flag:

```cpp
using SpecializedRadioRequirement = Framework::Requirement<
    Radio,
    Framework::RequirementScope::SameDomain,
    Framework::AtLeastProviders<1U>,
    Framework::HasAnyFlags<
        SupportedFeatures,
        RadioFeature::LowEnergy,
        RadioFeature::ReceiveTimestamp
    >
>;
```

Multiple providers may match.

## Requiring Flags to Be Absent

`HasNoFlags` requires all supplied flags to be absent from the advertised FlagProperty value:

```cpp
using NonLowEnergyRequirement = Framework::Requirement<
    Radio,
    Framework::RequirementScope::SameDomain,
    Framework::ExactlyProviders<1U>,
    Framework::HasNoFlags<
        SupportedFeatures,
        RadioFeature::LowEnergy
    >
>;
```

A provider which does not advertise `SupportedFeatures` at all does not satisfy this constraint. Absence of the Property is not treated as a zero-valued feature set.

Use ordinary Property presence/absence constraints when Property existence itself is part of the qualification.

## Provider Matching and Selection

Use canonical Requirement queries:

```cpp
using MatchingRadios =
    RadioComposition::Matches<SpecializedRadioRequirement>;

static_assert(
    RadioComposition::MatchCount<SpecializedRadioRequirement> == 2U
);
```

When one provider is required, make the selection policy explicit:

```cpp
using RadioProvider = RadioComposition::Select<
    ReliableTimestampRequirement,
    Framework::SelectUnique
>;
```

The Requirement cardinality describes architectural acceptability. `SelectUnique` describes the result shape requested by the caller.

## Compatibility with Ordinary Property Queries

A `FlagProperty` remains a Property and its compact storage value is available through the normal Property metadata surface.

For example:

```cpp
static_assert(
    RadioComposition::PropertiesForProvider<
        TimestampedRadio,
        Radio
    >::template Value<SupportedFeatures> == 0x07U
);
```

Ordinary numeric constraints such as `Equals` may also inspect the underlying representation when that is intentionally part of the contract:

```cpp
using ExactFeatureStorage = Framework::Requirement<
    Radio,
    Framework::RequirementScope::SameDomain,
    Framework::ExactlyProviders<1U>,
    Framework::Equals<
        SupportedFeatures,
        0x07U
    >
>;
```

For feature semantics, prefer `HasAllFlags`, `HasAnyFlags` and `HasNoFlags` because they retain the typed flag vocabulary.

## Logical Composition

Flag constraints can participate in the same logical constraint expressions as other Requirement constraints:

```cpp
using Requirement = Framework::Requirement<
    Radio,
    Framework::RequirementScope::AnyDomain,
    Framework::AtLeastProviders<1U>,
    Framework::AnyOf<
        Framework::HasAllFlags<
            SupportedFeatures,
            RadioFeature::ReceiveTimestamp
        >,
        Framework::HasAllFlags<
            SupportedFeatures,
            RadioFeature::LowEnergy
        >
    >
>;
```

## Compile-Time Diagnostics

The Framework rejects invalid declarations including:

- flags from an unrelated enum Type;
- values that cannot be represented by the selected storage Type;
- a FlagProperty associated with an invalid Capability;
- constraints applied to the wrong Property/Capability domain;
- empty flag-constraint argument lists where the operation requires at least one flag.

These failures occur at compile time.

## Runtime and Memory Cost

`FlagProperty`, `FlagPropertyValue`, and the associated Requirement constraints are compile-time Types.

They do not allocate memory, create provider instances, or add runtime lookup structures.

The runtime provider object is exactly the provider object Bootstrap chooses to instantiate.

## Relationship to Runtime FlagSet

Composition Flag Properties and `FlagSet` solve different problems.

- `FlagProperty` describes immutable compile-time provider characteristics.
- `FlagSet` stores mutable runtime flags.

Do not use Composition metadata for mutable connection state, current hardware status, peer state, or other runtime facts.

## Complete Example

```cpp
namespace Framework = ESPressio::System::CompositionFramework;

enum class RadioFeature : std::uint8_t {
    Broadcast = 0,
    ReceiveTimestamp = 1,
    LowEnergy = 2
};

struct RadioDomain final : Framework::Domain {};
struct Radio final : Framework::SharedCapability<RadioDomain> {};

struct SupportedFeatures final : Framework::FlagProperty<
    Radio,
    RadioFeature,
    std::uint8_t
> {};

struct TimestampedRadio final : Framework::Provider<
    RadioDomain,
    Framework::Offers<
        Framework::Offer<
            Radio,
            Framework::FlagPropertyValue<
                SupportedFeatures,
                RadioFeature::Broadcast,
                RadioFeature::ReceiveTimestamp
            >
        >
    >
> {};

struct LowEnergyRadio final : Framework::Provider<
    RadioDomain,
    Framework::Offers<
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

using TimestampRequirement = Framework::Requirement<
    Radio,
    Framework::RequirementScope::SameDomain,
    Framework::ExactlyProviders<1U>,
    Framework::HasAllFlags<
        SupportedFeatures,
        RadioFeature::ReceiveTimestamp
    >
>;

using RadioComposition = Framework::Composition<
    RadioDomain,
    TimestampedRadio,
    LowEnergyRadio
>;

using Selected = RadioComposition::Select<
    TimestampRequirement,
    Framework::SelectUnique
>;

static_assert(std::is_same_v<Selected, TimestampedRadio>);
```

The buildable demonstration is under `demos/composition-flag-properties`.
