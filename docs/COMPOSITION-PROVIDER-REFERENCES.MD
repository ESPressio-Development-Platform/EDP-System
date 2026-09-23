# Composition Provider References

## Purpose

`ProviderReferences<ProviderList<...>>` is a small runtime binding helper for plural compile-time provider selections.

It exists for one specific situation:

1. Composition determines a set of provider Types at compile time.
2. Bootstrap owns concrete instances of those provider Types.
3. A consumer needs non-owning access to all selected instances.

`ProviderReferences` bridges those two layers without introducing a registry, service locator, heap allocation, runtime type erasure, or provider ownership.

## Architectural Flow

The intended flow is:

```text
Requirement
    -> Composition::Matches<Requirement>
    -> ProviderList<ConcreteProviderTypes...>
    -> Bootstrap-owned concrete provider instances
    -> ProviderReferences<ProviderList<...>>
    -> consumer
```

Provider selection remains compile time. Object ownership remains Bootstrap.

## Declaring a Plural Requirement

For example:

```cpp
using BroadcastRequirement = Framework::Requirement<
    Radio,
    Framework::RequirementScope::SameDomain,
    Framework::AtLeastProviders<1U>,
    Framework::HasAllFlags<
        SupportedFeatures,
        RadioFeature::Broadcast
    >
>;
```

Resolve every matching provider Type:

```cpp
using BroadcastProviderTypes =
    RadioComposition::Matches<BroadcastRequirement>;
```

`BroadcastProviderTypes` is a `ProviderList<...>` preserving deterministic provider declaration order.

## Binding Bootstrap-Owned Provider Instances

Define the runtime reference collection:

```cpp
using BroadcastProviderReferences =
    Framework::ProviderReferences<
        BroadcastProviderTypes
    >;
```

Bootstrap owns the providers:

```cpp
TimestampedRadio timestamped;
BasicBroadcastRadio basic;
LowEnergyOnlyRadio lowEnergy;
```

Bind exactly the provider Types represented by the compile-time list, in list order:

```cpp
BroadcastProviderReferences radios(
    timestamped,
    basic
);
```

Construction is compile-time constrained. Reversing the providers or supplying an unrelated provider does not satisfy the binding constructor.

The non-matching `lowEnergy` instance remains owned by Bootstrap but is not present in `radios`.

## Operating Across Every Provider

Use `ForEach`:

```cpp
radios.ForEach(
    [](auto& radio) {
        radio.Broadcast();
    }
);
```

The callable is instantiated against every provider Type represented by the ProviderList.

This retains static dispatch and concrete provider Types. There is no virtual base class and no runtime capability lookup.

## Const Traversal

A const reference collection supplies const provider references to the callable:

```cpp
const auto& constRadios = radios;

constRadios.ForEach(
    [](const auto& radio) {
        inspect(radio);
    }
);
```

## Empty Plural Results

An empty list is a valid `ProviderReferences` specialization:

```cpp
Framework::ProviderReferences<
    Framework::ProviderList<>
> none;
```

Calling `ForEach` on it performs no operation.

Whether an empty match set is architecturally acceptable is controlled by the Requirement's cardinality. For example, `AnyProviderCount` can intentionally allow zero.

## Ownership and Lifetime

`ProviderReferences` is non-owning.

The referenced provider objects must outlive the `ProviderReferences` object and every consumer that stores a copy of it.

A typical application therefore uses this ownership order:

```text
Application Bootstrap/runtime owner
    owns concrete provider objects
    owns consumers
        consumers store ProviderReferences
            ProviderReferences borrows providers
```

The Framework never extends provider lifetimes.

## Result Handling

`ProviderReferences` does not aggregate operation result values.

The callable supplied to `ForEach` owns any aggregation policy required by the application:

```cpp
bool allSucceeded = true;

radios.ForEach(
    [&allSucceeded](auto& radio) {
        allSucceeded =
            radio.TryBroadcast() &&
            allSucceeded;
    }
);
```

This avoids imposing one runtime error model on unrelated capabilities.

## Resource Behavior

The helper stores only the references needed to reach the bound concrete provider instances.

It performs no dynamic allocation and creates no registry.

All provider-set discovery has already happened at compile time in `Matches<Requirement>`.

## ProviderReferences Is Not a Service Locator

The following model is deliberately **not** provided:

```text
runtime global registry
    -> ask for capability
    -> search providers dynamically
    -> return erased interface/object
```

Instead:

```text
compile-time Requirement
    -> concrete ProviderList
    -> explicit Bootstrap binding
    -> typed non-owning references
```

That distinction preserves determinism, explicit ownership and compile-time architecture validation.

## Relationship to Singular Injection

When exactly one provider is required, prefer direct explicit injection of the concrete Type selected at compile time:

```cpp
using ClockProvider = ApplicationArchitecture::Select<
    ClockRequirement,
    Framework::SelectUnique
>;

class Consumer final {
public:
    explicit Consumer(ClockProvider& clock) noexcept :
        _clock(&clock) {}

private:
    ClockProvider* _clock;
};
```

Use `ProviderReferences` when the compile-time result is genuinely plural.

## Complete Example

```cpp
namespace Framework = ESPressio::System::CompositionFramework;

enum class RadioFeature : std::uint8_t {
    Broadcast = 0,
    LowEnergy = 1
};

struct RadioDomain final : Framework::Domain {};
struct Radio final : Framework::SharedCapability<RadioDomain> {};

struct SupportedFeatures final : Framework::FlagProperty<
    Radio,
    RadioFeature,
    std::uint8_t
> {};

class FirstRadio final : public Framework::Provider<
    RadioDomain,
    Framework::Offers<
        Framework::Offer<
            Radio,
            Framework::FlagPropertyValue<
                SupportedFeatures,
                RadioFeature::Broadcast
            >
        >
    >
> {
public:
    void Broadcast() noexcept {}
};

class SecondRadio final : public Framework::Provider<
    RadioDomain,
    Framework::Offers<
        Framework::Offer<
            Radio,
            Framework::FlagPropertyValue<
                SupportedFeatures,
                RadioFeature::Broadcast
            >
        >
    >
> {
public:
    void Broadcast() noexcept {}
};

using BroadcastRequirement = Framework::Requirement<
    Radio,
    Framework::RequirementScope::SameDomain,
    Framework::AtLeastProviders<1U>,
    Framework::HasAllFlags<
        SupportedFeatures,
        RadioFeature::Broadcast
    >
>;

using RadioComposition = Framework::Composition<
    RadioDomain,
    FirstRadio,
    SecondRadio
>;

using BroadcastProviderTypes =
    RadioComposition::Matches<BroadcastRequirement>;

using BroadcastReferences =
    Framework::ProviderReferences<
        BroadcastProviderTypes
    >;

static_assert(BroadcastProviderTypes::Count == 2U);

FirstRadio first;
SecondRadio second;

BroadcastReferences references(
    first,
    second
);
```

The buildable demonstration is under `demos/composition-provider-references`.
