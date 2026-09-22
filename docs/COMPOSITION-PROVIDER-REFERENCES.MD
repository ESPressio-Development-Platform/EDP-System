# Composition Provider References

## Purpose

`ProviderReferences` bridges one deliberate gap between compile-time Composition and ordinary runtime Bootstrap wiring.

The Composition Framework already answers:

> Which provider types satisfy this capability or Need?

For plural queries it returns a compile-time `ProviderList<TProviders...>` through:

~~~cpp
Composition::ProvidersFor<TCapability>
Composition::ProvidersSatisfying<TNeed>
Architecture::ProvidersSatisfying<TNeed>
~~~

`ProviderReferences` lets Bootstrap code bind the corresponding **already-owned runtime provider instances** to that compile-time list and pass the resulting non-owning view to a consumer.

It does not construct, own, discover, register, or dynamically select providers.

## Architectural flow

The intended flow is:

~~~text
Need
  |
  v
ProvidersSatisfying<Need>
  |
  v
ProviderList<A, B, C>
  |
  v
Bootstrap owns A, B and C instances
  |
  v
ProviderReferences<ProviderList<A, B, C>>
  |
  v
consumer.ForEach(...)
~~~

The capability/Need filtering happens entirely at compile time. Runtime traversal therefore performs no capability test and no descriptor lookup.

## Declaring a plural requirement

For example, a Radio domain can advertise a finite feature set:

~~~cpp
enum class RadioFeature : std::uint8_t {
    Broadcast = 0,
    ReceiveTimestamp = 1,
    LowEnergy = 2
};

struct SupportedFeatures final : Composition::FlagProperty<
    Radio,
    RadioFeature,
    std::uint8_t
> {};
~~~

A consumer can describe every acceptable Broadcast provider:

~~~cpp
using BroadcastRequirement = Composition::Need<
    Radio,
    Composition::HasAllFlags<
        SupportedFeatures,
        RadioFeature::Broadcast
    >
>;
~~~

The existing plural query resolves all matching provider **types**:

~~~cpp
using BroadcastProviderTypes =
    RadioComposition::ProvidersSatisfying<
        BroadcastRequirement
    >;
~~~

If two providers match, the result is conceptually:

~~~cpp
Composition::ProviderList<
    TimestampedRadio,
    BasicBroadcastRadio
>
~~~

No provider instance exists merely because this type list exists.

## Binding Bootstrap-owned provider instances

Bootstrap owns the actual objects:

~~~cpp
TimestampedRadio timestampedRadio;
BasicBroadcastRadio basicBroadcastRadio;
LowEnergyOnlyRadio lowEnergyOnlyRadio;
~~~

The exact plural result can then be bound:

~~~cpp
using BroadcastProviderReferences =
    Composition::ProviderReferences<
        BroadcastProviderTypes
    >;

BroadcastProviderReferences broadcastRadios(
    timestampedRadio,
    basicBroadcastRadio
);
~~~

The constructor requires provider references in exactly the type/order represented by `BroadcastProviderTypes`.

Passing the wrong provider, omitting one, adding one, or changing their order does not satisfy that constructor and therefore fails compilation.

The excluded `LowEnergyOnlyRadio` object remains owned by Bootstrap but is not part of this injected view because its provider type was not selected by the `BroadcastRequirement`.

## Operating across every provider

A consumer can apply the same operation to every bound provider:

~~~cpp
broadcastRadios.ForEach(
    [](auto& radio) {
        radio.Broadcast();
    }
);
~~~

The callable must be valid for every represented provider type.

This is intentionally compile-time checked. A callback which only accepts one concrete provider type cannot be used with a collection containing several different provider types.

The operation runs once for each provider in `ProviderList` order.

## Const traversal

A const `ProviderReferences` view exposes each provider as const:

~~~cpp
const auto& radios = broadcastRadios;

radios.ForEach(
    [](const auto& radio) {
        static_cast<void>(
            radio.BroadcastCount()
        );
    }
);
~~~

This allows read-only inspection without weakening constness of the provider instances.

## Empty plural results

`ProviderReferences<ProviderList<>>` is valid.

Calling `ForEach` on an empty collection performs no invocation.

This allows callers which deliberately treat a plural query as optional to represent a zero-match result without a special runtime registry or sentinel object.

A required capability/Need remains subject to the existing Composition/Architecture validation rules. `ProviderReferences` does not weaken those contracts.

## Ownership and lifetime

`ProviderReferences` is strictly non-owning.

It stores one non-owning pointer value for each represented provider instance. It performs no heap allocation.

Every provider instance must outlive:

- the `ProviderReferences` object;
- every copy of that object;
- every consumer which stores such a copy.

Bootstrap remains responsible for provider object construction, destruction, initialization, shutdown, and dependency wiring.

The most important rule is:

> Composition selects provider types. Bootstrap owns provider instances. ProviderReferences only binds references between those two layers.

## Result handling

`ForEach` itself has no operational success/failure state. It invokes the caller-supplied operation for each provider.

Return values from the callback are intentionally not aggregated by `ProviderReferences`, because different domains have different result semantics.

When a provider operation is fallible, the callback must handle or deliberately aggregate its strongly typed result itself.

For example:

~~~cpp
broadcastRadios.ForEach(
    [&overallResult](auto& radio) {
        const auto result = radio.Broadcast();

        // Domain-specific result handling belongs here.
        overallResult.Observe(
            result
        );
    }
);
~~~

This prevents the Composition utility from inventing one generic success/failure policy for unrelated provider domains.

## Resource behavior

For `N` bound providers, the persistent collection stores `N` non-owning provider pointer values in a fixed `std::tuple`.

There is:

- no heap allocation;
- no runtime list growth;
- no provider registry;
- no virtual dispatch required by the collection;
- no runtime capability matching;
- no runtime Property or Attribute inspection.

The set of provider types and traversal calls are visible to the compiler and remain eligible for normal optimization/inlining.

## ProviderReferences is not a service locator

This extension does **not** add:

~~~text
composition.Get<T>()
architecture.ResolveAtRuntime(...)
runtime capability names
runtime provider registration
runtime provider discovery
runtime type erasure
~~~

Those mechanisms would materially change the Composition architecture.

`ProviderReferences` is instead a typed Bootstrap wiring utility over a provider list which was already resolved at compile time.

## Relationship to singular injection

Singular resolution remains unchanged:

~~~cpp
using ClockProvider =
    ApplicationComposition::ProviderFor<SystemClock>;
~~~

Bootstrap can continue to own/inject that one provider using ordinary C++ references.

Plural runtime binding is only needed when a legitimate plural query returns several provider types and one consumer should operate across the matching instances.

## Complete demonstration

A buildable demonstration is provided under:

~~~text
demos/composition-provider-references/
~~~

with Arduino IDE, PIOArduino/PlatformIO Arduino, and PIOArduino/PlatformIO ESP-IDF targets.
