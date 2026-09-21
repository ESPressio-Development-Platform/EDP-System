# Qualified Composition Resolution

## Purpose

The Composition Framework supports applications in which more than one concrete provider legitimately supplies the same `SharedCapability`.

Capability identity alone is not always sufficient to choose the provider required by one consumer. Two Radio providers may both satisfy the same Radio capability while representing different physical modules, frequencies, roles, buses, positions, or other application-relevant qualities.

Qualified Composition resolution allows providers to advertise open-ended compile-time **Attributes** and allows a `Need` to filter providers by those Attributes without naming concrete implementation types.

All selection remains in the C++ type system. The framework does not introduce a runtime provider registry, runtime descriptor lookup, allocation, string comparison, or object ownership.

---

## Capability, Property, and Attribute

The framework deliberately distinguishes three concepts.

### Capability

A Capability describes **what a provider fundamentally does**.

Examples:

- System Clock;
- Radio;
- File Storage;
- Key/Value Storage.

Capabilities remain the fundamental substitution boundary.

### Property

A Property is a formal characteristic owned by a Capability contract.

Properties are appropriate when a characteristic forms part of the capability's substitutability contract and should have one shared semantic definition.

Examples include:

- maximum packet size;
- clock resolution;
- maximum supported path length.

Properties continue to use the existing `Property<>`, `PropertyValue<>`, and ordered constraint vocabulary.

### Attribute

An Attribute is an open-ended compile-time descriptor attached to one specific capability `Offer`.

Attributes are appropriate when a provider or application needs to describe **which otherwise-compatible provider is meant**, without forcing every possible differentiator into the abstraction's closed vocabulary.

Examples include:

- `Frequency = 24917`;
- `StorageType = "SD"`;
- `Role = "Telemetry"`;
- `Position = "RearLeft"`.

The capability-owning library does not need to pre-register Attribute names or enumerate every possible Attribute value.

A future provider can introduce a new descriptor without modifying the capability abstraction.

---

## Advertising Attributes

Attributes are declared within an `Offer` so that they qualify only the capability represented by that Offer.

A provider offering two capabilities can therefore advertise completely different Attributes for each capability.

### Native compile-time values

Scalar values, enum values, Boolean values, and other valid C++20 non-type template values can be advertised directly:

```cpp
Framework::Offer<
    Radio,
    Framework::Attribute<
        "Frequency",
        24917U
    >
>
```

### Compile-time text

Text Attributes use the C++20 structural-string convenience alias:

```cpp
Framework::Offer<
    FileStorage,
    Framework::TextAttribute<
        "StorageType",
        "SD"
    >
>
```

The equivalent generic form is:

```cpp
Framework::Attribute<
    "StorageType",
    Framework::Text<"SD">
>
```

The text is architectural compile-time metadata. The Composition Framework provides no runtime descriptor registry or runtime lookup API.

If consuming application code deliberately materializes compile-time metadata at runtime, that application code becomes responsible for the resulting runtime cost. Runtime materialization is not part of the Composition contract.

### Attribute names are open

An Attribute name is a C++20 structural compile-time string.

The following are all legal without prior registration in the capability-owning library:

```cpp
Framework::Attribute<"Frequency", 24917U>
Framework::TextAttribute<"Role", "Telemetry">
Framework::TextAttribute<"Bus", "SPI2">
Framework::TextAttribute<"StorageType", "FutureStorageTechnology">
```

Attribute names are case-sensitive and exact.

Within one Offer, each Attribute name may appear only once.

---

## Requiring Attributes

A consumer qualifies a `Need` with `AttributeEquals` or `TextAttributeEquals`.

For example:

```cpp
using TelemetryRadioRequirement = Framework::Need<
    Radio,
    Framework::AttributeEquals<
        "Frequency",
        24917U
    >,
    Framework::TextAttributeEquals<
        "Role",
        "Telemetry"
    >
>;
```

Properties and Attributes may be constrained by the same Need:

```cpp
using Requirement = Framework::Need<
    Radio,
    Framework::AtLeast<
        MaximumPayloadBytes,
        128U
    >,
    Framework::AttributeEquals<
        "Frequency",
        24917U
    >
>;
```

The provider must satisfy every constraint in the Need.

Attribute matching is exact in the current contract. Ordered/range Attribute constraints are not currently part of the public API.

---

## Qualified Provider Queries

A Domain `Composition` exposes four qualified queries.

### Count

```cpp
ApplicationComposition::ProviderCountSatisfying<
    TelemetryRadioRequirement
>
```

Returns the number of providers satisfying the complete Need.

### Predicate

```cpp
ApplicationComposition::HasProviderSatisfying<
    TelemetryRadioRequirement
>
```

Indicates whether at least one matching provider exists.

### Plural resolution

```cpp
using Providers = ApplicationComposition::ProvidersSatisfying<
    TelemetryRadioRequirement
>;
```

Returns a `ProviderList` containing every matching provider.

Plural resolution is the correct operation when more than one provider may legitimately match.

### Singular resolution

```cpp
using Provider = ApplicationComposition::ProviderSatisfying<
    TelemetryRadioRequirement
>;
```

Requires exactly one matching provider.

Compilation fails when:

- no provider satisfies the Need; or
- more than one provider satisfies the Need.

This deliberately prevents silent first-match selection.

If several providers match, the requirement should be made more specific or plural resolution should be used.

---

## Existing Capability Queries Remain Unchanged

Qualified resolution is additive.

The existing APIs retain their established meaning:

```cpp
ApplicationComposition::ProviderFor<SystemClock>
ApplicationComposition::ProvidersFor<CommunicationInterface>
```

`ProviderFor<TCapability>` still requires exactly one provider of that capability.

`ProvidersFor<TCapability>` still returns all providers of that capability without applying qualified constraints.

Existing Property APIs also retain their previous semantics.

---

## Same-Domain Requirements

The existing `Requires<>` declaration remains a same-Domain contract.

For example:

```cpp
Framework::Requires<
    Framework::Need<SystemClock>
>
```

The provider and the required capability must belong to the same Composition Domain.

A Domain `Composition` continues to validate these requirements directly.

This existing behavior has not been weakened or reinterpreted.

---

## Cross-Domain Dependencies

A provider may also require capabilities belonging to another Domain.

These requirements use the separate additive `DependsOn<>` declaration:

```cpp
class TelemetryProvider final : public Framework::Provider<
    ServiceDomain,
    Framework::Provides<
        Framework::Offer<TelemetryService>
    >,
    Framework::Requires<>,
    Framework::DependsOn<
        TelemetryRadioRequirement
    >
> {};
```

A `DependsOn<>` entry must belong to a Domain other than the provider's own Domain.

Same-Domain dependencies continue to use `Requires<>`.

Unlike `Requires<>`, `DependsOn<>` may contain more than one differently-qualified Need for the same SharedCapability:

```cpp
Framework::DependsOn<
    TelemetryRadioRequirement,
    ControlRadioRequirement
>
```

Each Need is validated independently.

Repeated Needs do **not** implicitly assert provider cardinality or object distinctness. If an architecture requires proof that two dependencies must resolve to different concrete providers, that is a separate architectural constraint and must be represented explicitly rather than inferred from repetition.

---

## Architecture

A Domain `Composition` validates providers and same-Domain requirements.

Cross-Domain dependencies are validated by an `Architecture` containing the relevant Domain Compositions:

```cpp
using ApplicationArchitecture = Framework::Architecture<
    RadioComposition,
    ServiceComposition
>;
```

`Architecture<>` verifies:

- each entry is a Composition;
- each Domain appears at most once;
- every provider's `DependsOn<>` requirements can be satisfied by the participating Compositions.

It also exposes Architecture-level qualified provider queries:

```cpp
using RadioProvider = ApplicationArchitecture::ProviderSatisfying<
    TelemetryRadioRequirement
>;
```

### Architecture is not runtime dependency injection

`Architecture<>` does not:

- construct providers;
- own providers;
- allocate storage;
- pass references;
- establish initialization order;
- start or stop services.

Bootstrap code remains responsible for ordinary C++ object ownership and for wiring the selected provider instances together.

Composition and Architecture answer compile-time structural questions only.

### Architecture validation must be instantiated

A provider may declare `DependsOn<>` while its own Domain Composition remains independently valid.

The complete cross-Domain contract is validated when the application or higher-level Bootstrap instantiates the corresponding `Architecture<...>`.

Applications using providers with cross-Domain dependencies must therefore include the relevant Domain Compositions in their Architecture.

---

## Dependency-Graph Boundaries Still Apply

`DependsOn<>` describes a compile-time capability dependency.

It does not override ESPressio's library dependency policies.

In particular:

- it does not authorize circular ESPressio library dependencies;
- it does not permit a higher-level abstraction library to depend on a concrete lower-level implementation repository when the architecture prohibits that direction;
- it does not replace the repository dependency graph.

For example, a Persistence-domain concrete provider may declare a need for a Platform-domain storage mechanism while normal library dependencies still flow in the permitted direction.

---

## Runtime Cost Contract

Attributes are intended solely for compile-time architectural selection.

The Composition Framework introduces no deliberate runtime:

- heap allocation;
- RAM registry;
- provider lookup;
- descriptor lookup;
- string comparison;
- dynamic dispatch.

Compile-time text is represented using C++20 structural values and participates in template resolution.

Longer or more numerous descriptors can increase compiler work, template symbol length, and debug/build artefact size. Those are build-time costs rather than intentional runtime firmware costs.

The framework should not add runtime APIs which materialize Attributes merely for inspection.

---

## Complete Example

```cpp
namespace Framework = ESPressio::System::CompositionFramework;

struct RadioDomain final : Framework::Domain {};
struct ServiceDomain final : Framework::Domain {};

struct Radio final : Framework::SharedCapability<RadioDomain> {};
struct Service final : Framework::ExclusiveCapability<ServiceDomain> {};

class FrequencyXRadio final : public Framework::Provider<
    RadioDomain,
    Framework::Provides<
        Framework::Offer<
            Radio,
            Framework::Attribute<
                "Frequency",
                24917U
            >,
            Framework::TextAttribute<
                "Role",
                "Telemetry"
            >
        >
    >
> {};

class FrequencyYRadio final : public Framework::Provider<
    RadioDomain,
    Framework::Provides<
        Framework::Offer<
            Radio,
            Framework::Attribute<
                "Frequency",
                58124U
            >,
            Framework::TextAttribute<
                "Role",
                "Control"
            >
        >
    >
> {};

using TelemetryRadioRequirement = Framework::Need<
    Radio,
    Framework::AttributeEquals<
        "Frequency",
        24917U
    >,
    Framework::TextAttributeEquals<
        "Role",
        "Telemetry"
    >
>;

using ControlRadioRequirement = Framework::Need<
    Radio,
    Framework::AttributeEquals<
        "Frequency",
        58124U
    >
>;

class ServiceProvider final : public Framework::Provider<
    ServiceDomain,
    Framework::Provides<
        Framework::Offer<Service>
    >,
    Framework::Requires<>,
    Framework::DependsOn<
        TelemetryRadioRequirement,
        ControlRadioRequirement
    >
> {};

using RadioComposition = Framework::Composition<
    RadioDomain,
    FrequencyXRadio,
    FrequencyYRadio
>;

using ServiceComposition = Framework::Composition<
    ServiceDomain,
    ServiceProvider
>;

using ApplicationArchitecture = Framework::Architecture<
    RadioComposition,
    ServiceComposition
>;

using TelemetryRadioProvider = ApplicationArchitecture::ProviderSatisfying<
    TelemetryRadioRequirement
>;
```

`TelemetryRadioProvider` resolves to `FrequencyXRadio` at compile time without the service naming that concrete implementation in its requirement.
