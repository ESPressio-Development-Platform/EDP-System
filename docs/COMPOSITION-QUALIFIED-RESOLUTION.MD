# Qualified Composition Resolution

## Purpose

Qualified Composition Resolution selects provider Types by **capability plus compile-time characteristics**, rather than by concrete implementation name.

The canonical flow is:

```text
Capability
    + Requirement scope
    + cardinality
    + Property / FlagProperty / Attribute constraints
    -> Matches
    -> explicit Select policy
```

This supports interchangeable concrete providers without introducing runtime lookup or interface-based indirection.

## Capability, Property and Attribute

### Capability

A Capability answers:

> What architectural service does this provider supply?

For example:

```cpp
struct Radio final :
    Framework::SharedCapability<PlatformDomain> {};
```

### Property

A Property is a named compile-time characteristic with a declared value Type:

```cpp
struct MaximumPayloadBytes final :
    Framework::Property<Radio, std::uint16_t> {};
```

Providers advertise values through `PropertyValue`:

```cpp
Framework::PropertyValue<
    MaximumPayloadBytes,
    1024U
>
```

Properties are appropriate when the characteristic is part of the stable capability vocabulary.

### Attribute

An Attribute is an open compile-time descriptor attached to an Offer:

```cpp
Framework::Attribute<
    "Channel",
    6
>

Framework::TextAttribute<
    "Transport",
    "WiFi"
>
```

Attributes are appropriate for open-ended provider descriptors where defining a dedicated Property Type would add little value.

## Advertising Qualified Offers

A provider may advertise Properties and Attributes together:

```cpp
struct WifiRadio final : Framework::Provider<
    PlatformDomain,
    Framework::Offers<
        Framework::Offer<
            Radio,
            Framework::PropertyValue<
                MaximumPayloadBytes,
                512U
            >,
            Framework::TextAttribute<
                "Transport",
                "WiFi"
            >,
            Framework::Attribute<
                "Channel",
                6
            >
        >
    >
> {};
```

All metadata remains compile time.

## Requirement Qualification

A Requirement combines:

- a Capability;
- a scope;
- a provider cardinality;
- zero or more qualification constraints.

Example:

```cpp
using WifiRequirement = Framework::Requirement<
    Radio,
    Framework::RequirementScope::AnyDomain,
    Framework::AtLeastProviders<1U>,
    Framework::TextAttributeEquals<
        "Transport",
        "WiFi"
    >,
    Framework::AtLeast<
        MaximumPayloadBytes,
        256U
    >
>;
```

Qualification constraints filter matching providers. Cardinality determines whether the resulting population is architecturally acceptable.

## Attribute Constraints

Common Attribute constraints include:

```cpp
Framework::HasAttribute<"Name">
Framework::DoesNotHaveAttribute<"Name">
Framework::AttributeEquals<"Name", Value>
Framework::TextAttributeEquals<"Name", "Text">
Framework::AttributeBetween<"Name", Minimum, Maximum>
```

A provider that does not advertise an Attribute required by a positive constraint simply does not match.

Attribute names are intentionally open-ended compile-time strings.

## Property Constraints

Qualified resolution also supports Property constraints such as:

```cpp
Framework::HasProperty<Property>
Framework::DoesNotHaveProperty<Property>
Framework::Equals<Property, Value>
Framework::NotEquals<Property, Value>
Framework::AtLeast<Property, Value>
Framework::AtMost<Property, Value>
Framework::Between<Property, Minimum, Maximum>
Framework::OneOf<Property, Values...>
Framework::NoneOf<Property, Values...>
```

Typed FlagProperty constraints are described in
[Composition Flag Properties](./COMPOSITION-FLAG-PROPERTIES.MD).

## Logical Constraints

Constraints may be composed:

```cpp
Framework::AllOf<...>
Framework::AnyOf<...>
Framework::Not<...>
```

For example:

```cpp
using PreferredRadio = Framework::Requirement<
    Radio,
    Framework::RequirementScope::AnyDomain,
    Framework::AtLeastProviders<1U>,
    Framework::AnyOf<
        Framework::AtMost<
            LatencyMicroseconds,
            4U
        >,
        Framework::Equals<
            Preferred,
            true
        >
    >
>;
```

## Requirement Scope

`RequirementScope::SameDomain` is used by a provider Contract when the required capability belongs to the provider's own Domain.

`RequirementScope::ExternalDomain` is used by a provider Contract when the required capability belongs to another Domain.

`RequirementScope::AnyDomain` is useful for consumer-side queries and reusable consumer Contracts.

Scope is validated as part of provider Contract declaration. It is not inferred from repository layout or include relationships.

## Cardinality

The supported cardinality vocabulary includes:

```cpp
Framework::ExactlyProviders<N>
Framework::AtLeastProviders<N>
Framework::AtMostProviders<N>
Framework::ProviderCountBetween<Minimum, Maximum>
Framework::AnyProviderCount
```

Cardinality belongs to the Requirement. Selection policy belongs to the query.

This distinction is deliberate. For example, a Requirement may accept at least two Radios while a caller separately asks to select every match or the unique lowest-latency match.

## Matching Queries

Both `Composition` and `Architecture` expose the canonical query surface.

### Match count

```cpp
ApplicationArchitecture::MatchCount<WifiRequirement>
```

### Presence

```cpp
ApplicationArchitecture::HasMatch<WifiRequirement>
```

### Plural matches

```cpp
using Providers =
    ApplicationArchitecture::Matches<WifiRequirement>;
```

The result is a deterministic `ProviderList<...>`.

### Cardinality validation

```cpp
ApplicationArchitecture::SatisfiesRequirement<
    WifiRequirement
>
```

This evaluates the Requirement's provider-count policy.

## Explicit Selection

Use `Select` when a specific result shape is required:

```cpp
using WifiProvider = ApplicationArchitecture::Select<
    WifiRequirement,
    Framework::SelectUnique
>;
```

Available selection policies:

```cpp
Framework::SelectAll
Framework::SelectUnique
Framework::SelectOptionalUnique
Framework::SelectMinimum<Property>
Framework::SelectMaximum<Property>
Framework::SelectAllMinimum<Property>
Framework::SelectAllMaximum<Property>
```

No singular provider is chosen implicitly.

## Best-Match Selection

Property-based policies rank the already-qualified provider set.

For example:

```cpp
using FastestRadio = ApplicationArchitecture::Select<
    RadioRequirement,
    Framework::SelectMinimum<
        LatencyMicroseconds
    >
>;
```

If more than one provider ties for the minimum, singular `SelectMinimum` is a compile-time error.

Use `SelectAllMinimum` to preserve all tied providers:

```cpp
using FastestRadios = ApplicationArchitecture::Select<
    RadioRequirement,
    Framework::SelectAllMinimum<
        LatencyMicroseconds
    >
>;
```

The same distinction applies to maximum-value selection.

## Same-Domain Provider Contracts

A provider declares same-domain requirements directly in its consolidated Contract:

```cpp
using LocalClock = Framework::Requirement<
    Clock,
    Framework::RequirementScope::SameDomain,
    Framework::ExactlyProviders<1U>
>;

struct SchedulerProvider final : Framework::Provider<
    PlatformDomain,
    Framework::Offers<
        Framework::Offer<Scheduler>
    >,
    Framework::Contract<
        LocalClock
    >
> {};
```

The Domain Composition validates the same-domain cardinality.

## Cross-Domain Provider Contracts

Cross-domain requirements use `ExternalDomain`:

```cpp
using ExternalWifi = Framework::Requirement<
    Radio,
    Framework::RequirementScope::ExternalDomain,
    Framework::ExactlyProviders<1U>,
    Framework::TextAttributeEquals<
        "Transport",
        "WiFi"
    >
>;

struct TelemetryProvider final : Framework::Provider<
    ServiceDomain,
    Framework::Offers<
        Framework::Offer<Telemetry>
    >,
    Framework::Contract<
        ExternalWifi
    >
> {};
```

The complete `Architecture` validates the external Requirement.

There is no separate cross-domain dependency container. The provider's `Contract` is the single source of truth.

## Provider Relationships

Qualified Requirements may be combined in Contract relationship clauses.

### Same provider

```cpp
Framework::SameProvider<
    WifiRequirement,
    DiagnosticsRequirement
>
```

One concrete provider Type must satisfy every Requirement.

### Distinct providers

```cpp
Framework::DistinctProviders<
    WifiRequirement,
    BleRequirement
>
```

The Framework must be able to assign pairwise-distinct provider Types.

These are structural constraints, not runtime object searches.

## Architecture

```cpp
using ApplicationArchitecture = Framework::Architecture<
    PlatformComposition,
    ServiceComposition
>;

static_assert(ApplicationArchitecture::IsValid);
```

Architecture flattens the participating provider Types for Contract validation while preserving Domain Composition boundaries.

It validates:

- external Requirement cardinality;
- cross-domain SameProvider/DistinctProviders relationships;
- lifecycle targets;
- lifecycle cycles;
- reusable consumer Contracts.

## Architecture Is Not Runtime Dependency Injection

Architecture does not create providers.

This:

```cpp
using Provider = ApplicationArchitecture::Select<
    WifiRequirement,
    Framework::SelectUnique
>;
```

produces a Type.

Bootstrap still owns the object:

```cpp
Provider wifi;
```

and passes it explicitly to consumers.

## Dependency-Graph Boundaries Still Apply

Composition does not change repository ownership or dependency direction.

A provider library may implement a capability defined in another library, but compile-time Architecture metadata must not be used to create circular library dependencies.

The repository dependency graph remains authoritative.

## Runtime Cost Contract

Qualified resolution is compile time:

- no heap allocation;
- no runtime search;
- no runtime strings;
- no provider registry;
- no virtual dispatch requirement;
- no retained query objects.

Only the concrete objects explicitly instantiated by Bootstrap exist at runtime.

## Complete Example

```cpp
namespace Framework = ESPressio::System::CompositionFramework;

struct PlatformDomain final : Framework::Domain {};
struct ServiceDomain final : Framework::Domain {};

struct Radio final :
    Framework::SharedCapability<PlatformDomain> {};

struct Telemetry final :
    Framework::ExclusiveCapability<ServiceDomain> {};

struct MaximumPayloadBytes final :
    Framework::Property<Radio, std::uint16_t> {};

struct WifiRadio final : Framework::Provider<
    PlatformDomain,
    Framework::Offers<
        Framework::Offer<
            Radio,
            Framework::PropertyValue<
                MaximumPayloadBytes,
                512U
            >,
            Framework::TextAttribute<
                "Transport",
                "WiFi"
            >
        >
    >
> {};

struct BleRadio final : Framework::Provider<
    PlatformDomain,
    Framework::Offers<
        Framework::Offer<
            Radio,
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

using ExternalWifi = Framework::Requirement<
    Radio,
    Framework::RequirementScope::ExternalDomain,
    Framework::ExactlyProviders<1U>,
    Framework::TextAttributeEquals<
        "Transport",
        "WiFi"
    >
>;

struct TelemetryProvider final : Framework::Provider<
    ServiceDomain,
    Framework::Offers<
        Framework::Offer<Telemetry>
    >,
    Framework::Contract<
        ExternalWifi
    >
> {};

using PlatformComposition = Framework::Composition<
    PlatformDomain,
    WifiRadio,
    BleRadio
>;

using ServiceComposition = Framework::Composition<
    ServiceDomain,
    TelemetryProvider
>;

using ApplicationArchitecture = Framework::Architecture<
    PlatformComposition,
    ServiceComposition
>;

static_assert(ApplicationArchitecture::IsValid);

using ConsumerRadios = Framework::Requirement<
    Radio,
    Framework::RequirementScope::AnyDomain,
    Framework::AtLeastProviders<1U>,
    Framework::AtLeast<
        MaximumPayloadBytes,
        512U
    >
>;

using Matches =
    ApplicationArchitecture::Matches<ConsumerRadios>;

static_assert(Matches::Count == 2U);

using LargestPayload = ApplicationArchitecture::Select<
    ConsumerRadios,
    Framework::SelectMaximum<
        MaximumPayloadBytes
    >
>;

static_assert(
    std::is_same_v<
        LargestPayload,
        BleRadio
    >
);
```

The buildable examples and demos under this repository use the same canonical query surface.
