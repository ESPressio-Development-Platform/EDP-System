# Flag Set

`ESPressio::System::FlagSet<TFlag, TStorage>` is the System-level compact runtime representation for a set of strongly typed flags.

The public abstraction deliberately talks in terms of **flags** rather than numbered bits. Internally, each flag maps to one bit in a caller-selected unsigned integral storage type.

## Purpose

A `FlagSet` is appropriate where a runtime value needs to represent several independent Boolean states without paying for one Boolean object per state.

Typical examples include:

- runtime feature state;
- provider state;
- transport state;
- protocol feature state;
- compact policy state;
- application-local status flags.

`FlagSet` is a runtime value abstraction. It is not a Composition capability or provider.

Compile-time architectural capability selection remains the responsibility of the [Composition Framework](./COMPOSITION-FRAMEWORK.MD).

## Flag Domain

The first template parameter is an enum type whose values define zero-based bit positions.

For example:

```cpp
enum class Feature : std::uint8_t {
    Clock = 0,
    Commands = 1,
    Events = 2,
    State = 3
};

using FeatureFlags = ESPressio::System::FlagSet<
    Feature,
    std::uint8_t
>;
```

The enum domain is part of the `FlagSet` type.

A flag from another enum cannot be passed to the public flag operations.

This prevents unrelated domains from being accidentally mixed merely because their numeric positions happen to match.

## Storage

The second template parameter selects the complete persistent representation.

It must be an unsigned integral type other than `bool`.

The default storage type is `std::uint32_t`.

Examples:

```cpp
using SmallFlags = ESPressio::System::FlagSet<
    Feature,
    std::uint8_t
>;

using MediumFlags = ESPressio::System::FlagSet<
    Feature,
    std::uint16_t
>;

using DefaultFlags = ESPressio::System::FlagSet<Feature>;
```

The number of representable positions is exposed through:

```cpp
FeatureFlags::Capacity
```

For `std::uint8_t`, the capacity is 8 positions.

## Memory Contract

A `FlagSet` instance contains exactly one persistent data member: the selected storage integer.

It performs no dynamic allocation, owns no pointer, has no vtable, and contains no runtime registry.

The host-side tests explicitly require:

```text
sizeof(FlagSet<..., uint8_t>)  == sizeof(uint8_t)
sizeof(FlagSet<..., uint16_t>) == sizeof(uint16_t)
sizeof(FlagSet<..., uint32_t>) == sizeof(uint32_t)
sizeof(FlagSet<..., uint64_t>) == sizeof(uint64_t)
```

The tests also require the value type to remain trivially copyable, trivially destructible, and standard-layout.

The helper `FlagState<TFlag>` used by state-aware constructors is a temporary construction value. Its size is not part of the persistent `FlagSet` representation.

## Construction

### Empty

Default construction produces an empty set:

```cpp
FeatureFlags flags;
```

Its raw value is zero.

### Raw storage

An exact raw representation can be supplied explicitly:

```cpp
FeatureFlags flags(
    static_cast<std::uint8_t>(0x05U)
);
```

The constructor is `explicit` so arbitrary integers do not silently become flag sets.

Raw construction is useful at persistence, serialization, and wire boundaries where the representation is already defined.

### Set flags

One or more typed flags can be supplied directly:

```cpp
FeatureFlags flags(
    Feature::Clock,
    Feature::Events
);
```

Both supplied flags start set.

### Explicit per-flag state

The nested `State` alias exposes the state-assignment type for the flag domain:

```cpp
FeatureFlags flags(
    FeatureFlags::State(
        Feature::Clock,
        true
    ),
    FeatureFlags::State(
        Feature::Commands,
        false
    ),
    FeatureFlags::State(
        Feature::Events,
        true
    )
);
```

This form makes the intended state of every listed flag explicit.

State assignments are applied from left to right. If the same flag is listed more than once, the later assignment wins.

### Raw storage plus state overrides

A raw representation can be followed by explicit state overrides:

```cpp
FeatureFlags flags(
    static_cast<std::uint8_t>(0xFFU),
    FeatureFlags::State(
        Feature::Clock,
        false
    ),
    FeatureFlags::State(
        Feature::Commands,
        true
    )
);
```

This is useful when a compact representation is loaded and selected flags must then be deliberately overridden.

## Mutation

### Set

```cpp
flags.Set(Feature::Clock);

flags.Set(
    Feature::Commands,
    Feature::Events
);
```

### Clear

```cpp
flags.Clear(Feature::Clock);

flags.Clear(
    Feature::Commands,
    Feature::Events
);
```

Calling `Clear()` with no flag arguments clears the complete representation.

### Flip

```cpp
flags.Flip(Feature::Clock);
```

Multiple flags can be flipped in one call.

### Assign one common state

```cpp
flags.Assign(
    clockAvailable,
    Feature::Clock
);

flags.Assign(
    subsystemEnabled,
    Feature::Commands,
    Feature::Events
);
```

A true state sets the supplied flags. A false state clears them.

This is the direct replacement for a separate set-or-clear helper.

### Assign selected raw bits

```cpp
flags.AssignMask(
    mask,
    state
);
```

Only bits selected by `mask` are replaced. Bits outside the mask are preserved.

The operation is equivalent to:

```cpp
bits = (bits & ~mask) | (state & mask);
```

while remaining encapsulated by the typed value.

## Predicates

### Is one flag set?

```cpp
if (flags.IsSet(Feature::Clock)) {
    // ...
}
```

### Is any requested flag set?

```cpp
if (flags.HasAny(
    Feature::Clock,
    Feature::Commands
)) {
    // ...
}
```

### Are all requested flags set?

```cpp
if (flags.HasAll(
    Feature::Clock,
    Feature::Commands
)) {
    // ...
}
```

### Is the set empty?

```cpp
if (flags.IsEmpty()) {
    // ...
}
```

These are genuine predicates and therefore return `bool`.

## Raw value access

The complete underlying representation is available through:

```cpp
const auto value = flags.Value();
```

No implicit integer conversion is provided.

This keeps representation-boundary access deliberate.

## Representability and Defensive Range Handling

A flag value is representable when its zero-based bit position is less than `Capacity`.

Known enum constants should therefore be designed so that every intended flag position fits the selected storage type.

Representability can be queried explicitly:

```cpp
if (flags.IsRepresentable(flag)) {
    // ...
}
```

This is particularly useful when a flag value originated outside the normal typed source path, for example after an explicit cast from externally sourced data.

The implementation defensively maps an unrepresentable position to no storage bit. This prevents undefined shifts:

- `Set`, `Clear`, `Flip`, and `Assign` ignore unrepresentable flag values;
- `IsSet` reports false for an unrepresentable flag;
- `HasAny` only considers represented bits;
- `HasAll` reports false if any requested flag is unrepresentable.

Application-defined enum constants should nevertheless remain within the selected capacity. Defensive handling is not a substitute for a correct flag-domain definition.

## Serialization and Wire Stability

The selected storage width is explicit and therefore suitable for stable compact representations.

For example:

```cpp
using ProtocolFeatures = ESPressio::System::FlagSet<
    ProtocolFeature,
    std::uint32_t
>;
```

continues to occupy 32 bits even if only a small number of flags are currently defined.

Adding another enum value therefore does not silently change the storage width.

When a raw representation crosses a persistence or wire boundary, normal format/version ownership remains with the domain defining that boundary.

`FlagSet` itself does not impose byte order, serialization format, or protocol evolution rules.

## Runtime Cost

Operations are `constexpr` and consist of fixed-width integer validation and bitwise operations.

Calls using known enum constants can normally be folded or inlined by an optimizing compiler.

For runtime flag values, representability checking introduces a small bounds check before the bit operation. This is intentional so invalid bit shifts are never executed.

The persistent RAM cost remains exactly the selected storage width.

## Concurrency

`FlagSet` contains no synchronization.

Independent instances can be used independently. Concurrent mutation or read/write access to the same instance requires synchronization appropriate to the owning application/domain.

## ISR Policy

No ESPressio component carries a general guarantee of ISR safety.

Although `FlagSet` is allocation-free and performs only local value operations, its presence does not override the ESPressio ISR policy. ISR design should remain limited to the minimal work permitted by that policy, and shared-state synchronization remains the responsibility of the owning design.

## Demo

A buildable demonstration is provided under:

```text
demos/flag-set-basic
```

with Arduino IDE, PlatformIO Arduino, and PlatformIO ESP-IDF targets.
