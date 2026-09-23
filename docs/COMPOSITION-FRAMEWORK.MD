# Composition Framework

The EDP-System Composition Framework is a **compile-time architecture description and validation facility**.

It lets an application describe:

- which capabilities exist;
- which concrete provider Types offer those capabilities;
- what each provider requires from the rest of the architecture;
- which provider relationships must hold;
- how qualified providers are matched and selected;
- and which lifecycle ordering constraints exist between providers.

Composition does **not** own provider objects, perform runtime service lookup, maintain a service locator, or introduce runtime type erasure. Bootstrap owns concrete provider instances and passes references explicitly to the runtime objects that consume them.

# Core Model

## Domain

A Domain groups related capabilities.

```cpp
struct PlatformDomain final : Framework::Domain {};
struct ServiceDomain final : Framework::Domain {};
```

A capability belongs to exactly one Domain.

## Capability

Use `ExclusiveCapability<TDomain>` when at most one provider may offer the capability within a Composition:

```cpp
struct Clock final :
    Framework::ExclusiveCapability<PlatformDomain> {};
```

Use `SharedCapability<TDomain>` when multiple providers are valid:

```cpp
struct Radio final :
    Framework::SharedCapability<PlatformDomain> {};
```

Exclusive capability conflicts are rejected at compile time.

## Offer and Offers

A provider advertises each capability through `Offer<TCapability, ...>`.

An Offer may also contain compile-time Property values and open-ended Attributes:

```cpp
struct LatencyMicroseconds final :
    Framework::Property<Radio, std::uint32_t> {};

struct WifiRadio final : Framework::Provider<
    PlatformDomain,
    Framework::Offers<
        Framework::Offer<
            Radio,
            Framework::PropertyValue<
                LatencyMicroseconds,
                8U
            >,
            Framework::TextAttribute<
                "Transport",
                "WiFi"
            >
        >
    >
> {};
```

`Offers<...>` is the canonical provider offer-set declaration.

## Requirement

A `Requirement` describes provider qualification and architectural necessity:

```cpp
using LocalClock = Framework::Requirement<
    Clock,
    Framework::RequirementScope::SameDomain,
    Framework::ExactlyProviders<1U>
>;
```

The template arguments are:

```cpp
Requirement<
    Capability,
    Scope,
    Cardinality,
    Constraints...
>
```

A Requirement does **not** silently choose one provider when several match. Matching and selection are separate operations.

### Requirement Scope

`RequirementScope::SameDomain`

The Requirement belongs inside a provider Contract only when the required capability belongs to the provider's own Domain.

`RequirementScope::ExternalDomain`

The Requirement belongs inside a provider Contract only when the required capability belongs to another Domain.

`RequirementScope::AnyDomain`

Useful for reusable consumer-side queries and Contracts where the consumer is not declaring a provider-owned same/external-domain relationship.

Provider-owned Contracts deliberately distinguish `SameDomain` from `ExternalDomain`; invalid scope/domain combinations are rejected at compile time.

### Cardinality

The Framework provides:

```cpp
Framework::ExactlyProviders<N>
Framework::AtLeastProviders<N>
Framework::AtMostProviders<N>
Framework::ProviderCountBetween<Minimum, Maximum>
Framework::AnyProviderCount
```

For example:

```cpp
using Radios = Framework::Requirement<
    Radio,
    Framework::RequirementScope::SameDomain,
    Framework::AtLeastProviders<2U>
>;
```

Cardinality is part of architectural validity. It is not a provider-selection policy.

### Qualification Constraints

Requirements may constrain Properties, FlagProperties and Attributes.

Representative Property constraints include:

```cpp
Framework::Equals<Property, Value>
Framework::NotEquals<Property, Value>
Framework::AtLeast<Property, Value>
Framework::AtMost<Property, Value>
Framework::Between<Property, Minimum, Maximum>
Framework::OneOf<Property, Values...>
Framework::NoneOf<Property, Values...>
Framework::HasProperty<Property>
Framework::DoesNotHaveProperty<Property>
```

Representative Attribute constraints include:

```cpp
Framework::HasAttribute<"Name">
Framework::DoesNotHaveAttribute<"Name">
Framework::AttributeEquals<"Name", Value>
Framework::TextAttributeEquals<"Name", "Value">
Framework::AttributeBetween<"Name", Minimum, Maximum>
```

Logical constraint composition is available through:

```cpp
Framework::AllOf<...>
Framework::AnyOf<...>
Framework::Not<...>
```

Typed flag constraints are documented separately in
[Composition Flag Properties](./COMPOSITION-FLAG-PROPERTIES.MD).

## Contract

A `Contract<...>` is the complete compile-time consumer contract attached to a provider, or a reusable standalone consumer contract.

It may contain direct Requirements and relationship clauses:

```cpp
Framework::Contract<
    LocalClock,
    LocalRadios,
    Framework::SameProvider<
        WifiRequirement,
        DiagnosticsRequirement
    >,
    Framework::DistinctProviders<
        WifiRequirement,
        BleRequirement
    >,
    Framework::InitializesAfter<LocalClock>,
    Framework::ShutsDownBefore<LocalClock>
>
```

### SameProvider

`SameProvider<R1, R2, ...>` requires one concrete provider Type to satisfy every contained Requirement.

This is useful when separate capabilities or qualifications must come from one implementation rather than merely existing somewhere in the architecture.

### DistinctProviders

`DistinctProviders<R1, R2, ...>` requires pairwise-distinct provider Types to satisfy the contained Requirements.

The Framework performs compile-time assignment validation rather than merely comparing match counts.

### Lifecycle Clauses

`InitializesAfter<R>` requires the Requirement to resolve unambiguously to a provider other than the owning provider and creates an initialization-order edge.

`ShutsDownBefore<R>` similarly creates a shutdown-order edge.

Lifecycle self-dependencies and cycles are compile-time errors.

## Provider

The canonical provider declaration is:

```cpp
Framework::Provider<
    Domain,
    Framework::Offers<...>,
    Framework::Contract<...>
>
```

The Contract defaults to `Contract<>`, so a provider with no requirements may simply write:

```cpp
struct ClockProvider final : Framework::Provider<
    PlatformDomain,
    Framework::Offers<
        Framework::Offer<Clock>
    >
> {};
```

Providers expose canonical compile-time metadata:

```cpp
ProviderType::CompositionDomain
ProviderType::CompositionOffers
ProviderType::CompositionContract
```

There is no compatibility metadata or parallel requirement/dependency representation.

# Composition

A `Composition<TDomain, Providers...>` validates one Domain's provider population.

```cpp
using PlatformComposition = Framework::Composition<
    PlatformDomain,
    ClockProvider,
    WifiRadioProvider,
    BleRadioProvider
>;
```

A Composition validates:

- every provider belongs to the declared Domain;
- each provider's Offers belong to that Domain;
- ExclusiveCapability conflicts do not exist;
- each provider Contract is structurally valid;
- same-domain direct Requirement cardinality is satisfied;
- same-domain `SameProvider` and `DistinctProviders` clauses are satisfied;
- same-domain lifecycle targets are unambiguous.

External-domain Contract clauses are completed at Architecture level.

`Composition::IsValid` is true only after compile-time validation succeeds.

# Architecture

An `Architecture<Compositions...>` combines multiple Domain Compositions:

```cpp
using ApplicationArchitecture = Framework::Architecture<
    PlatformComposition,
    ServiceComposition
>;
```

Architecture requires each Domain to appear at most once and validates every provider Contract against the flattened provider population.

This is where external-domain Requirement cardinality, cross-domain provider relationships and complete lifecycle relationships are validated.

```cpp
static_assert(ApplicationArchitecture::IsValid);
```

Architecture also exposes deterministic compile-time lifecycle orders:

```cpp
ApplicationArchitecture::InitializationOrder
ApplicationArchitecture::ShutdownOrder
```

These are `ProviderList` Types derived from explicit lifecycle clauses. The Framework does not invoke provider lifecycle methods itself.

# Matching and Selection

Matching answers **which providers satisfy a Requirement**.

Selection answers **which result shape the caller explicitly wants**.

## Matches

Both Composition and Architecture expose:

```cpp
using Candidates = ApplicationArchitecture::Matches<MyRequirement>;
```

`Matches<R>` returns a `ProviderList<...>` in deterministic declaration order.

Additional query metadata:

```cpp
ApplicationArchitecture::MatchCount<MyRequirement>
ApplicationArchitecture::HasMatch<MyRequirement>
ApplicationArchitecture::SatisfiesRequirement<MyRequirement>
```

`SatisfiesRequirement<R>` checks the Requirement's cardinality. `MatchCount<R>` and `Matches<R>` report qualification results independently of whether the cardinality is acceptable.

## Select

Provider choice is explicit:

```cpp
using Provider = ApplicationArchitecture::Select<
    MyRequirement,
    Framework::SelectUnique
>;
```

Available policies include:

```cpp
Framework::SelectAll
Framework::SelectUnique
Framework::SelectOptionalUnique
Framework::SelectMinimum<Property>
Framework::SelectMaximum<Property>
Framework::SelectAllMinimum<Property>
Framework::SelectAllMaximum<Property>
```

`SelectUnique` rejects zero or ambiguous matches.

`SelectOptionalUnique` returns `void` for zero matches and rejects ambiguity.

Minimum/maximum policies require the selected Property to be advertised by the matching providers. Singular minimum/maximum selection rejects ties; plural variants preserve every tied best match.

## Joint Matching

When one concrete provider must satisfy several Requirements in a query:

```cpp
using Candidates = ApplicationArchitecture::JointMatches<
    RadioRequirement,
    DiagnosticsRequirement
>;

using Provider = ApplicationArchitecture::SelectJoint<
    Framework::SelectUnique,
    RadioRequirement,
    DiagnosticsRequirement
>;
```

Also available:

```cpp
ApplicationArchitecture::JointMatchCount<...>
```

# Reusable Consumer Contracts

A Contract does not have to belong to a provider.

A reusable library can declare:

```cpp
using ConsumerContract = Framework::Contract<
    RadioRequirement,
    Framework::SameProvider<
        WifiRequirement,
        DiagnosticsRequirement
    >
>;
```

Then application Bootstrap can validate it against the selected Architecture:

```cpp
static_assert(
    ApplicationArchitecture::SatisfiesContract<ConsumerContract>
);

using Validation =
    ApplicationArchitecture::ValidateContract<ConsumerContract>;
```

`ValidateContract` provides strict compile-time diagnostics when the Contract is unsatisfied.

# ProviderMatch Diagnostics

`ProviderMatch<Provider, Requirement>` exposes qualification diagnostics for one provider:

```cpp
Framework::ProviderMatch<P, R>::OffersCapability
Framework::ProviderMatch<P, R>::ConstraintsSatisfied
Framework::ProviderMatch<P, R>::IsSatisfied
```

This is compile-time metadata only.

# Property and Attribute Inspection

A Composition may inspect a known provider's Offer metadata:

```cpp
using Properties =
    PlatformComposition::PropertiesForProvider<
        WifiRadioProvider,
        Radio
    >;

using Attributes =
    PlatformComposition::AttributesForProvider<
        WifiRadioProvider,
        Radio
    >;
```

For an ExclusiveCapability or another capability with exactly one provider, convenience queries are available:

```cpp
PlatformComposition::PropertiesFor<Clock>
PlatformComposition::HasProperty<Clock, ResolutionNanoseconds>
PlatformComposition::PropertyValue<Clock, ResolutionNanoseconds>
PlatformComposition::AttributesFor<Clock>
```

These convenience queries internally require unique provider selection; they do not silently choose among SharedCapability providers.

# ProviderList

Plural matching produces `ProviderList<...>`.

Useful compile-time operations include:

```cpp
ProviderListType::Count
ProviderListType::Contains<T>
ProviderListType::At<N>
ProviderListType::Front
ProviderListType::Back
ProviderListType::IndexOf<T>
```

Framework algorithms include:

```cpp
Framework::ProviderListConcat<A, B>
Framework::ProviderListUnion<A, B>
Framework::ProviderListIntersection<A, B>
Framework::ProviderListDifference<A, B>
Framework::ProviderListFilter<List, Predicate>
```

# Runtime Binding

Compile-time selection does not instantiate or locate providers.

Bootstrap owns the actual objects:

```cpp
ClockProvider clock;
WifiRadioProvider wifi;
```

A consumer that needs one provider receives an ordinary constructor/reference dependency.

When a compile-time query returns multiple provider Types, `ProviderReferences<ProviderList<...>>` can bind non-owning references to Bootstrap-owned instances while preserving the compile-time provider order.

See [Composition Provider References](./COMPOSITION-PROVIDER-REFERENCES.MD).

# Complete Example

```cpp
namespace Framework = ESPressio::System::CompositionFramework;

struct PlatformDomain final : Framework::Domain {};
struct ServiceDomain final : Framework::Domain {};

struct Radio final :
    Framework::SharedCapability<PlatformDomain> {};

struct Telemetry final :
    Framework::ExclusiveCapability<ServiceDomain> {};

struct Latency final :
    Framework::Property<Radio, std::uint32_t> {};

struct Wifi final : Framework::Provider<
    PlatformDomain,
    Framework::Offers<
        Framework::Offer<
            Radio,
            Framework::PropertyValue<Latency, 8U>,
            Framework::TextAttribute<"Transport", "WiFi">
        >
    >
> {};

struct Ble final : Framework::Provider<
    PlatformDomain,
    Framework::Offers<
        Framework::Offer<
            Radio,
            Framework::PropertyValue<Latency, 4U>,
            Framework::TextAttribute<"Transport", "BLE">
        >
    >
> {};

using ExternalWifi = Framework::Requirement<
    Radio,
    Framework::RequirementScope::ExternalDomain,
    Framework::ExactlyProviders<1U>,
    Framework::TextAttributeEquals<"Transport", "WiFi">
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
    Wifi,
    Ble
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

using ConsumerRadio = Framework::Requirement<
    Radio,
    Framework::RequirementScope::AnyDomain,
    Framework::AtLeastProviders<1U>,
    Framework::HasProperty<Latency>
>;

using LowestLatencyRadio = ApplicationArchitecture::Select<
    ConsumerRadio,
    Framework::SelectMinimum<Latency>
>;
```

All of the architecture and selection work above is compile-time work. Runtime ownership remains ordinary explicit C++ ownership.

# Compile-Time Failure Categories

The Framework deliberately rejects invalid architecture rather than silently weakening requirements. Examples include:

- duplicate providers for an ExclusiveCapability;
- provider Contracts using invalid Requirement scope/domain combinations;
- unsatisfied Requirement cardinality;
- a `SameProvider` relationship with no jointly satisfying provider;
- a `DistinctProviders` relationship for which no distinct assignment exists;
- ambiguous singular selection;
- tied singular best-match selection;
- lifecycle self-dependency;
- lifecycle dependency cycles;
- invalid Property, FlagProperty or Attribute constraints;
- provider queries against an unrelated Domain.

These checks are part of the compile-time architecture contract.

# Design Boundary

Composition is appropriate for stable architectural facts:

- capability availability;
- provider qualification;
- compile-time properties and Attributes;
- dependency relationships;
- deterministic provider selection;
- lifecycle ordering constraints.

Composition is not appropriate for runtime state:

- connection status;
- current queue depth;
- sensor values;
- dynamically discovered peers;
- mutable configuration;
- transient health.

Those belong in the runtime facilities that own them.

The central rule is:

> Composition describes **what concrete Types form the application architecture**. Bootstrap owns **the concrete objects**.
