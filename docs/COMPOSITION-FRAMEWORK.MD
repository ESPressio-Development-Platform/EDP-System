# Composition Framework

The ESPressio Composition Framework describes the compile-time architecture of an application or library.

It allows code to describe, in a form the compiler can verify:

- what capabilities exist;
- which providers supply those capabilities;
- which capabilities each provider requires;
- which compile-time properties a capability provider advertises, including typed finite flag sets;
- which open-ended compile-time Attributes qualify one capability Offer;
- which property values and Attributes a consumer requires;
- which provider uniquely satisfies a qualified Need;
- whether same-domain requirements and cross-domain dependencies are structurally valid;
- whether the complete collection of providers is valid.

The framework is deliberately **not** a runtime dependency-injection container. It does not construct provider objects, own provider objects, allocate memory, start services, stop services, or decide runtime initialization order.

Its job is to answer one architectural question before the program runs:

> **Is this collection of components structurally compatible?**

Runtime ownership and lifecycle remain ordinary C++ responsibilities.

---

# Contents

1. [The Problem Composition Solves](#the-problem-composition-solves)
2. [Tight Coupling and Why It Becomes Technical Debt](#tight-coupling-and-why-it-becomes-technical-debt)
3. [Why ESPressio Uses Composition Instead of Interface-Based Decoupling](#why-espressio-uses-composition-instead-of-interface-based-decoupling)
4. [The Basic Concepts](#the-basic-concepts)
5. [The Bootstrap Boundary](#the-bootstrap-boundary)
6. [Example 1 - Your First Composition](#example-1---your-first-composition)
7. [Example 2 - Requirements and Properties](#example-2---requirements-and-properties)
8. [Example 3 - Shared Capabilities](#example-3---shared-capabilities)
9. [Example 4 - Consuming Composition from a Reusable Library](#example-4---consuming-composition-from-a-reusable-library)
10. [Qualified Provider Resolution](#qualified-provider-resolution)
11. [Cross-Domain Architecture Validation](#cross-domain-architecture-validation)
12. [Compile-Time Validation](#compile-time-validation)
13. [Common Mistakes](#common-mistakes)
14. [Choosing What Belongs in Composition](#choosing-what-belongs-in-composition)
15. [Public Vocabulary Reference](#public-vocabulary-reference)

---

# The Problem Composition Solves

Imagine a library that needs a system clock.

The library should not need to know whether that clock comes from:

- ESP32 hardware;
- an external RTC;
- a desktop operating system;
- a test implementation;
- or some future platform.

The library usually cares about the capability, not the concrete implementation.

Without Composition, it is very easy to write code such as:

```cpp
class TimingService {

    private:

        /// Concrete clock implementation used directly by this service.
        Esp32Clock& _clock;

    public:

        /// Creates a timing service coupled directly to Esp32Clock.
        explicit TimingService(Esp32Clock& clock) : _clock(clock) {}

};
```

This works, but `TimingService` now knows that its clock is specifically an `Esp32Clock`.

Composition allows the same architectural requirement to be expressed as:

> A `SystemClock` capability must be available.

A concrete platform can separately declare:

> `Esp32Clock` provides `SystemClock`.

The application Bootstrap then combines those concrete providers into a Composition. The compiler verifies whether all declared requirements are satisfied.

Composition can also express stronger requirements. For example:

> A `SystemClock` must exist, and its resolution must be no worse than 1000 nanoseconds.

That requirement can also be checked at compile time.

---

# Tight Coupling and Why It Becomes Technical Debt

A **hard-coded dependency** exists when reusable code names the concrete implementation that it depends upon.

For example:

```cpp
Esp32Clock _clock;
```

means:

> This code does not merely require a clock. It requires this exact clock implementation.

That creates **tight coupling**.

Tight coupling is not automatically wrong. At an application Bootstrap boundary, choosing concrete implementations is necessary and intentional. The problem begins when knowledge of those concrete types spreads throughout the codebase.

If `Esp32Clock` appears in ten different libraries, replacing it with `Ds3231Clock` may require changes in all ten libraries. If a desktop build needs `DesktopClock`, those same libraries may need conditional compilation or parallel implementations. Tests may need still more substitutions.

Over time this creates technical debt because:

- unrelated libraries become aware of platform-specific types;
- replacing one implementation requires changes across many consumers;
- platform-specific include dependencies spread through the dependency graph;
- testing alternative implementations becomes harder;
- reusable libraries become less reusable;
- build configuration and source code become increasingly entangled.

Composition moves the concrete selection to one intentional boundary.

Reusable code depends on a **capability contract**. The Bootstrap supplies the concrete type that satisfies that contract.

The concrete type can be defined:

- in the same library;
- in another ESPressio library;
- in an application-specific library;
- in a platform library;
- in a test library;
- in any folder structure suitable for that project.

Composition does not care where the C++ type physically lives. Normal C++ build rules still apply: the Bootstrap must be able to include the type definition and the build system must provide the appropriate library dependency. Once visible to the compiler, the concrete type can participate in the Composition regardless of its repository or folder location.

This means most of the codebase can remain unaware of the selected concrete implementation.

---

# Why ESPressio Uses Composition Instead of Interface-Based Decoupling

A traditional C++ solution to tight coupling is an abstract interface:

```cpp
class ISystemClock {

    public:

        /// Returns the current system time.
        virtual std::uint64_t Now() const = 0;

        /// Provides a polymorphic destruction boundary.
        virtual ~ISystemClock() = default;

};
```

A consumer can then depend on `ISystemClock&` instead of `Esp32Clock&`.

That is a valid design technique, and ESPressio does not prohibit interfaces where runtime polymorphism is genuinely required.

However, the majority of ESPressio architectural choices are known **when the firmware or application is built**. An ESP32 firmware image normally already knows which clock, scheduler, radio implementation, persistence backend, and other platform facilities were selected for that build.

Using runtime interfaces for a decision that is already fixed at compile time introduces machinery that ESPressio usually does not need:

- virtual dispatch and its associated indirection unless the compiler can devirtualize it;
- vtable metadata for polymorphic types;
- pointer or reference plumbing through consumers;
- runtime lifetime relationships that must be managed correctly;
- weaker opportunities for compile-time validation of provider properties and cross-provider requirements;
- a runtime abstraction boundary where a compile-time type relationship would have been sufficient.

Composition instead keeps the selection in the type system.

For example:

```cpp
ApplicationComposition::ProviderFor<SystemClock>
```

is a compile-time type lookup. If `ExampleClock` is the provider selected for `SystemClock`, the expression resolves to the type `ExampleClock` during compilation.

There is no Composition registry to query at runtime and no Composition-owned object to dereference.

This gives ESPressio several useful properties:

- invalid architectures can fail during compilation;
- capability properties can be checked during compilation;
- concrete provider types remain available to the compiler for normal optimization and inlining;
- reusable libraries do not need to know which concrete provider was selected;
- embedded applications do not pay for runtime substitution when runtime substitution is not required.

The distinction is therefore:

> **Interfaces are useful when implementations must be interchangeable at runtime.**

> **Composition is useful when implementations are selected at build time and the architecture should be validated at compile time.**

ESPressio uses Composition as its default architectural decoupling mechanism because build-time selection is the common case for the platform.

---

# The Basic Concepts

## Domain

A **Domain** groups related capabilities.

For example, a platform Domain might contain `SystemClock`, `TaskScheduler`, `EntropySource`, and `PersistentStorage`.

A concrete Domain derives from `Composition::Domain`.

## Capability

A **Capability** represents something that can be provided.

Capabilities are compile-time architectural identifiers. They do not contain runtime state and they do not implement behaviour.

There are two kinds.

### Exclusive Capability

An `ExclusiveCapability` may have no more than one provider inside a Composition.

A definitive system clock is a typical example.

If two providers both claim to supply the same exclusive capability, the Composition is rejected at compile time.

### Shared Capability

A `SharedCapability` may have multiple providers.

Communication interfaces are a typical example: a device may legitimately provide Wi-Fi, Bluetooth, and Ethernet implementations of the same shared capability.

## Provider

A **Provider** is a normal C++ type that declares what capabilities it supplies and what capabilities it requires.

The Composition Framework does not construct or own the provider object.

## Offer

An `Offer` says:

> This provider supplies this capability.

An Offer may also advertise compile-time properties of the supplied capability.

## Need

A `Need` says:

> This provider requires this capability.

A Need may also constrain properties of the required capability.

## Property

A **Property** describes a compile-time characteristic of one capability.

Examples include clock resolution, maximum packet size, or compile-time storage capacity.

Properties belong directly to capabilities. A clock property therefore cannot accidentally be attached to an unrelated radio capability.

### Flag-valued Property

A `FlagProperty` is a specialised Property for one finite set of orthogonal capability features.

Providers advertise flags through `FlagPropertyValue`, and consumers can constrain them with `HasAllFlags`, `HasAnyFlags`, or `HasNoFlags`.

Flag enum values are zero-based bit positions following the same contract as `ESPressio::System::FlagSet`; callers do not maintain raw masks for normal typed use.

Flag-valued Properties remain ordinary Composition Properties internally, so existing Property queries and provider-resolution semantics are preserved.

For the complete contract, examples, misuse boundaries, storage semantics, and diagnostics, see [Composition Flag Properties](./COMPOSITION-FLAG-PROPERTIES.MD).

## Attribute

An **Attribute** is open-ended compile-time metadata attached to one specific capability `Offer`.

Attributes are intended to qualify **which otherwise-compatible provider is meant** without forcing every possible discriminator into the capability abstraction.

Examples include:

- `Frequency = 24917`;
- `StorageType = "SD"`;
- `Role = "Telemetry"`;
- application-defined bus, position, channel, or identity descriptors.

Attributes are C++20 compile-time values and are not a runtime registry or runtime string lookup surface.

## Constraint

A **Constraint** describes an acceptable value for a required property or Attribute.

Formal capability Properties support:

- `Equals<TProperty, TValue>`;
- `AtLeast<TProperty, TValue>`;
- `AtMost<TProperty, TValue>`;
- `GreaterThan<TProperty, TValue>`;
- `LessThan<TProperty, TValue>`.

Flag-valued Properties additionally support:

- `HasAllFlags<TProperty, ...>`;
- `HasAnyFlags<TProperty, ...>`;
- `HasNoFlags<TProperty, ...>`.

Open Attributes currently support exact matching through `AttributeEquals<...>` and `TextAttributeEquals<...>`.

## Composition

A `Composition` combines providers belonging to one Domain, validates the complete provider set, and exposes compile-time queries for resolving providers and their properties.

---

# The Bootstrap Boundary

Composition does **not** make concrete provider types disappear.

Something must still decide that this build uses `Esp32Clock` rather than `DesktopClock`.

In ESPressio, that decision belongs at the application or library **Bootstrap boundary**.

A typical arrangement looks like this:

```text
Reusable libraries
    depend on capabilities
          |
          v
Composition contracts
          |
          v
Application Bootstrap
    selects concrete providers
          |
          +--> Esp32Clock
          +--> FreeRtosScheduler
          +--> EspNowRadio
```

Only the Bootstrap needs to assemble the one-or-more concrete type references required to satisfy the Composition contracts for that build.

For example:

```cpp
using ApplicationComposition = Composition::Composition<
    PlatformDomain,
    Esp32Clock,
    FreeRtosScheduler,
    EspNowRadio
>;
```

Those concrete types may come from completely different libraries and repository structures.

Changing the clock provider can therefore be localized to the Bootstrap:

```cpp
using ApplicationComposition = Composition::Composition<
    PlatformDomain,
    Ds3231Clock,
    FreeRtosScheduler,
    EspNowRadio
>;
```

Reusable libraries that ask the Composition for `SystemClock` do not need to change.

This is the main maintenance benefit: **concrete implementation knowledge is concentrated rather than distributed**.

---

# Example 1 - Your First Composition

This example contains one Domain, one exclusive capability, one provider, and one runtime owner.

It is a complete program.

## Composition version

```cpp
#include <cstdint>

#include "ESPressio_System.hpp"

namespace Example {

    namespace Composition = ESPressio::System::CompositionFramework;

    /// Platform capabilities used by this example application.
    struct PlatformDomain final : Composition::Domain {};


    /// Definitive system clock capability for this application.
    struct SystemClock final : Composition::ExclusiveCapability<PlatformDomain> {};


    /// Concrete clock provider used by this application.
    class ExampleClock final : public Composition::Provider<
        PlatformDomain,
        Composition::Provides<
            Composition::Offer<SystemClock>
        >
    > {

        private:

            // Runtime state.

            /// Indicates whether runtime initialization has completed.
            bool _initialized = false;

        public:

            // Public lifecycle.

            /// Initializes the runtime clock implementation.
            void Initialize() {
                _initialized = true;
            }

            // Public clock access.

            /// Returns a placeholder clock value for this example.
            std::uint64_t Now() const {
                if (!_initialized) { return 0U; }

                return 1000U;
            }

    };


    /// Complete compile-time architecture for the example application.
    using ApplicationComposition = Composition::Composition<
        PlatformDomain,
        ExampleClock
    >;


    /// Owns the runtime objects selected by the compile-time composition.
    class ApplicationRuntime final {

        private:

            // Runtime providers.

            /// Provider selected by the composition for the SystemClock capability.
            ApplicationComposition::ProviderFor<SystemClock> _clock;

        public:

            // Public lifecycle.

            /// Initializes the selected runtime providers.
            void Initialize() {
                _clock.Initialize();
            }

            /// Runs the example application.
            int Run() {
                return _clock.Now() > 0U ? 0 : 1;
            }

    };

} // Example


/// Executes the first Composition Framework example.
int main() {
    Example::ApplicationRuntime application;

    application.Initialize();

    return application.Run();
}
```

The lifecycle is intentionally explicit:

1. `PlatformDomain` groups the architecture.
2. `SystemClock` defines an exclusive capability.
3. `ExampleClock` declares that it provides that capability.
4. `ApplicationComposition` validates the provider set.
5. `ApplicationRuntime` uses `ProviderFor<SystemClock>` to resolve the concrete provider type.
6. `ApplicationRuntime` owns the provider instance and calls `Initialize()` itself.

For this Composition:

```cpp
ApplicationComposition::ProviderFor<SystemClock>
```

resolves at compile time to:

```cpp
ExampleClock
```

Therefore this declaration:

```cpp
ApplicationComposition::ProviderFor<SystemClock> _clock;
```

is effectively compiled as though the type were:

```cpp
ExampleClock _clock;
```

The difference is **where the dependency is selected**.

## Hard-coded equivalent

The same runtime behaviour can be written without Composition:

```cpp
#include <cstdint>

namespace Example {

    /// Concrete clock implementation used directly by the application.
    class ExampleClock final {

        private:

            // Runtime state.

            /// Indicates whether runtime initialization has completed.
            bool _initialized = false;

        public:

            // Public lifecycle.

            /// Initializes the runtime clock implementation.
            void Initialize() {
                _initialized = true;
            }

            // Public clock access.

            /// Returns a placeholder clock value for this example.
            std::uint64_t Now() const {
                if (!_initialized) { return 0U; }

                return 1000U;
            }

    };


    /// Owns the hard-coded runtime clock implementation.
    class ApplicationRuntime final {

        private:

            // Runtime providers.

            /// Clock implementation selected directly by this class.
            ExampleClock _clock;

        public:

            // Public lifecycle.

            /// Initializes the clock implementation.
            void Initialize() {
                _clock.Initialize();
            }

            /// Runs the example application.
            int Run() {
                return _clock.Now() > 0U ? 0 : 1;
            }

    };

} // Example


/// Executes the hard-coded equivalent.
int main() {
    Example::ApplicationRuntime application;

    application.Initialize();

    return application.Run();
}
```

Both programs own an `ExampleClock` object.

The architectural difference is that the hard-coded `ApplicationRuntime` explicitly names `ExampleClock`. If the application changes to `Ds3231Clock`, this class must also change.

With Composition, the concrete selection belongs to `ApplicationComposition`. Consumers ask for `SystemClock`, and the selected provider type is substituted by the compiler.

---

# Example 2 - Requirements and Properties

This example adds a scheduler that requires a System Clock with a sufficiently fine resolution.

It demonstrates the complete relationship between:

- a capability;
- a property belonging to that capability;
- a provider advertising a property value;
- another provider requiring the capability;
- a constraint placed upon the required property;
- runtime initialization after compile-time validation.

## Composition version

```cpp
#include <cstdint>

#include "ESPressio_System.hpp"

namespace Example {

    namespace Composition = ESPressio::System::CompositionFramework;

    /// Domain used by the scheduler example.
    struct PlatformDomain final : Composition::Domain {};


    /// Definitive system clock capability.
    struct SystemClock final : Composition::ExclusiveCapability<PlatformDomain> {};


    /// Definitive task scheduler capability.
    struct TaskScheduler final : Composition::ExclusiveCapability<PlatformDomain> {};


    /// Resolution of the SystemClock capability in nanoseconds.
    struct ClockResolutionNanoseconds final : Composition::Property<SystemClock, std::uint32_t> {};


    /// Clock provider advertising a 1000-nanosecond resolution.
    class ExampleClock final : public Composition::Provider<
        PlatformDomain,
        Composition::Provides<
            Composition::Offer<
                SystemClock,
                Composition::PropertyValue<
                    ClockResolutionNanoseconds,
                    1000U
                >
            >
        >
    > {

        private:

            // Runtime state.

            /// Indicates whether the clock provider has completed initialization.
            bool _initialized = false;

        public:

            // Public lifecycle.

            /// Initializes the clock provider.
            void Initialize() {
                _initialized = true;
            }

            // Public clock access.

            /// Returns a placeholder current-time value.
            std::uint64_t Now() const {
                if (!_initialized) { return 0U; }

                return 1000U;
            }

    };


    /// Scheduler requiring a SystemClock with at most 1000-nanosecond resolution.
    class ExampleScheduler final : public Composition::Provider<
        PlatformDomain,
        Composition::Provides<
            Composition::Offer<TaskScheduler>
        >,
        Composition::Requires<
            Composition::Need<
                SystemClock,
                Composition::AtMost<
                    ClockResolutionNanoseconds,
                    1000U
                >
            >
        >
    > {

        private:

            // Runtime state.

            /// Time observed during scheduler initialization.
            std::uint64_t _initializationTime = 0U;

            /// Indicates whether scheduler execution has started.
            bool _started = false;

        public:

            // Public lifecycle.

            /// Initializes the scheduler from whichever clock provider satisfies SystemClock.
            template<class TClockProvider>
            void Initialize(TClockProvider& clockProvider) {
                _initializationTime = clockProvider.Now();
            }

            /// Starts scheduler execution.
            void Start() {
                _started = true;
            }

            // Public status.

            /// Indicates whether scheduler execution has started.
            bool IsStarted() const {
                return _started && _initializationTime > 0U;
            }

    };


    /// Valid architecture because ExampleClock satisfies the scheduler requirement.
    using ApplicationComposition = Composition::Composition<
        PlatformDomain,
        ExampleClock,
        ExampleScheduler
    >;


    static_assert(
        ApplicationComposition::HasProperty<SystemClock, ClockResolutionNanoseconds>,
        "Expected the clock provider to advertise its resolution"
    );

    static_assert(
        ApplicationComposition::PropertyValue<SystemClock, ClockResolutionNanoseconds> == 1000U,
        "Expected a 1000-nanosecond clock resolution"
    );


    /// Owns and initializes the runtime providers.
    class ApplicationRuntime final {

        private:

            // Runtime providers.

            /// Provider selected for SystemClock.
            ApplicationComposition::ProviderFor<SystemClock> _clock;

            /// Provider selected for TaskScheduler.
            ApplicationComposition::ProviderFor<TaskScheduler> _scheduler;

        public:

            // Public lifecycle.

            /// Initializes providers in the dependency order selected by the application.
            void Initialize() {
                _clock.Initialize();
                _scheduler.Initialize(_clock);
            }

            /// Starts scheduler execution after initialization.
            void Start() {
                _scheduler.Start();
            }

            // Public status.

            /// Indicates whether the scheduler is running.
            bool IsRunning() const {
                return _scheduler.IsStarted();
            }

    };

} // Example


/// Executes the requirements-and-properties example.
int main() {
    Example::ApplicationRuntime application;

    application.Initialize();
    application.Start();

    return application.IsRunning() ? 0 : 1;
}
```

The declaration inside `ExampleScheduler` means:

> I require `SystemClock`, and the provider satisfying that requirement must advertise `ClockResolutionNanoseconds <= 1000`.

If the clock instead advertised `1000000U`, `ApplicationComposition` would fail to compile.

## Hard-coded equivalent

Without Composition, the dependency and its assumed suitability must be expressed directly:

```cpp
#include <cstdint>

namespace Example {

    /// Concrete clock implementation selected directly by the codebase.
    class ExampleClock final {

        private:

            // Runtime state.

            /// Indicates whether the clock provider has completed initialization.
            bool _initialized = false;

        public:

            // Public lifecycle.

            /// Initializes the clock provider.
            void Initialize() {
                _initialized = true;
            }

            // Public clock access.

            /// Returns a placeholder current-time value.
            std::uint64_t Now() const {
                if (!_initialized) { return 0U; }

                return 1000U;
            }

    };


    /// Scheduler coupled directly to ExampleClock.
    class ExampleScheduler final {

        private:

            // Runtime state.

            /// Time observed during scheduler initialization.
            std::uint64_t _initializationTime = 0U;

            /// Indicates whether scheduler execution has started.
            bool _started = false;

        public:

            // Public lifecycle.

            /// Initializes this scheduler from the specifically required ExampleClock implementation.
            void Initialize(ExampleClock& clockProvider) {
                _initializationTime = clockProvider.Now();
            }

            /// Starts scheduler execution.
            void Start() {
                _started = true;
            }

            // Public status.

            /// Indicates whether scheduler execution has started.
            bool IsStarted() const {
                return _started && _initializationTime > 0U;
            }

    };


    /// Owns the hard-coded concrete implementations.
    class ApplicationRuntime final {

        private:

            // Runtime providers.

            /// Clock implementation selected directly by this class.
            ExampleClock _clock;

            /// Scheduler implementation selected directly by this class.
            ExampleScheduler _scheduler;

        public:

            // Public lifecycle.

            /// Initializes the hard-coded providers.
            void Initialize() {
                _clock.Initialize();
                _scheduler.Initialize(_clock);
            }

            /// Starts scheduler execution.
            void Start() {
                _scheduler.Start();
            }

            // Public status.

            /// Indicates whether the scheduler is running.
            bool IsRunning() const {
                return _scheduler.IsStarted();
            }

    };

} // Example


/// Executes the hard-coded requirements example.
int main() {
    Example::ApplicationRuntime application;

    application.Initialize();
    application.Start();

    return application.IsRunning() ? 0 : 1;
}
```

This version contains two different forms of coupling:

- `ApplicationRuntime` explicitly owns `ExampleClock` and `ExampleScheduler`;
- `ExampleScheduler::Initialize()` explicitly requires `ExampleClock&`.

Changing the clock type can therefore force changes into both Bootstrap code **and the scheduler implementation itself**.

Composition removes the second dependency completely. The scheduler declares the capability and property contract it needs. The application Bootstrap selects a provider that satisfies it. If the selected provider does not satisfy the declared resolution requirement, the Composition fails at compile time.

That is substantially stronger than merely replacing one concrete class name with another.

---

# Example 3 - Shared Capabilities

An exclusive capability has one definitive provider. A shared capability is intentionally plural.

This example supplies the same communication capability through Wi-Fi and Bluetooth.

## Composition version

```cpp
#include "ESPressio_System.hpp"

namespace Example {

    namespace Composition = ESPressio::System::CompositionFramework;

    /// Domain used by the shared-capability example.
    struct PlatformDomain final : Composition::Domain {};


    /// Communication capability for which multiple providers are permitted.
    struct CommunicationInterface final : Composition::SharedCapability<PlatformDomain> {};


    /// Wi-Fi implementation of the shared communication capability.
    class WifiInterface final : public Composition::Provider<
        PlatformDomain,
        Composition::Provides<
            Composition::Offer<CommunicationInterface>
        >
    > {

        private:

            // Runtime state.

            /// Indicates whether this interface has started.
            bool _started = false;

        public:

            // Public lifecycle.

            /// Starts the Wi-Fi interface.
            void Start() {
                _started = true;
            }

            // Public status.

            /// Indicates whether the Wi-Fi interface has started.
            bool IsStarted() const {
                return _started;
            }

    };


    /// Bluetooth implementation of the shared communication capability.
    class BluetoothInterface final : public Composition::Provider<
        PlatformDomain,
        Composition::Provides<
            Composition::Offer<CommunicationInterface>
        >
    > {

        private:

            // Runtime state.

            /// Indicates whether this interface has started.
            bool _started = false;

        public:

            // Public lifecycle.

            /// Starts the Bluetooth interface.
            void Start() {
                _started = true;
            }

            // Public status.

            /// Indicates whether the Bluetooth interface has started.
            bool IsStarted() const {
                return _started;
            }

    };


    /// Composition containing both shared communication providers.
    using ApplicationComposition = Composition::Composition<
        PlatformDomain,
        WifiInterface,
        BluetoothInterface
    >;


    /// Compile-time list of providers supplying CommunicationInterface.
    using CommunicationProviders = ApplicationComposition::ProvidersFor<CommunicationInterface>;


    static_assert(
        CommunicationProviders::Count == 2U,
        "Expected both communication providers"
    );


    /// Bootstrap owner for the concrete shared providers selected by this build.
    class ApplicationRuntime final {

        private:

            // Runtime providers selected at the Bootstrap boundary.

            /// Wi-Fi provider participating in the shared capability.
            WifiInterface _wifi;

            /// Bluetooth provider participating in the shared capability.
            BluetoothInterface _bluetooth;

        public:

            // Public lifecycle.

            /// Starts every communication provider owned by the application.
            void Start() {
                _wifi.Start();
                _bluetooth.Start();
            }

            // Public status.

            /// Indicates whether every communication provider is running.
            bool IsRunning() const {
                return _wifi.IsStarted() && _bluetooth.IsStarted();
            }

    };

} // Example


/// Executes the shared-capability example.
int main() {
    Example::ApplicationRuntime application;

    application.Start();

    return application.IsRunning() ? 0 : 1;
}
```

For a plural capability, query `ProvidersFor<TCapability>`.

The Bootstrap still names `WifiInterface` and `BluetoothInterface` because the Bootstrap is the place where concrete runtime objects are intentionally selected and owned.

The rest of the codebase does not need to know those concrete names merely to reason about the `CommunicationInterface` capability.

## Hard-coded equivalent

Without Composition, the same application can directly own the two communication implementations:

```cpp
namespace Example {

    /// Concrete Wi-Fi implementation.
    class WifiInterface final {

        private:

            // Runtime state.

            /// Indicates whether this interface has started.
            bool _started = false;

        public:

            // Public lifecycle.

            /// Starts the Wi-Fi interface.
            void Start() {
                _started = true;
            }

            // Public status.

            /// Indicates whether the Wi-Fi interface has started.
            bool IsStarted() const {
                return _started;
            }

    };


    /// Concrete Bluetooth implementation.
    class BluetoothInterface final {

        private:

            // Runtime state.

            /// Indicates whether this interface has started.
            bool _started = false;

        public:

            // Public lifecycle.

            /// Starts the Bluetooth interface.
            void Start() {
                _started = true;
            }

            // Public status.

            /// Indicates whether the Bluetooth interface has started.
            bool IsStarted() const {
                return _started;
            }

    };


    /// Owns the hard-coded communication implementations.
    class ApplicationRuntime final {

        private:

            // Runtime providers.

            /// Wi-Fi implementation selected directly by this application.
            WifiInterface _wifi;

            /// Bluetooth implementation selected directly by this application.
            BluetoothInterface _bluetooth;

        public:

            // Public lifecycle.

            /// Starts both hard-coded communication implementations.
            void Start() {
                _wifi.Start();
                _bluetooth.Start();
            }

            // Public status.

            /// Indicates whether both communication implementations are running.
            bool IsRunning() const {
                return _wifi.IsStarted() && _bluetooth.IsStarted();
            }

    };

} // Example


/// Executes the hard-coded shared-provider example.
int main() {
    Example::ApplicationRuntime application;

    application.Start();

    return application.IsRunning() ? 0 : 1;
}
```

At this very small scale the runtime ownership looks similar, because both versions must ultimately own both concrete objects somewhere.

The difference appears as soon as other libraries need to reason about communication facilities.

Without Composition, those consumers tend to accumulate knowledge such as `WifiInterface`, `BluetoothInterface`, or application-specific lists of concrete types.

With Composition, the architectural statement is instead:

> This build contains two providers of `CommunicationInterface`.

The concrete list is assembled once in the Bootstrap Composition and can be queried from there.

---

# Example 4 - Consuming Composition from a Reusable Library

This is where Composition provides its largest maintenance benefit.

A reusable library should depend on capabilities rather than concrete platform implementations wherever possible.

The following `TimingService` resolves its clock provider through the supplied Composition.

## Composition version

```cpp
#include <cstdint>

#include "ESPressio_System.hpp"

namespace Example {

    namespace Composition = ESPressio::System::CompositionFramework;

    /// Domain used by the reusable-library example.
    struct PlatformDomain final : Composition::Domain {};


    /// Definitive clock capability consumed by TimingService.
    struct SystemClock final : Composition::ExclusiveCapability<PlatformDomain> {};


    /// Concrete clock provider selected by this application build.
    class ExampleClock final : public Composition::Provider<
        PlatformDomain,
        Composition::Provides<
            Composition::Offer<SystemClock>
        >
    > {

        private:

            // Runtime state.

            /// Current example clock value.
            std::uint64_t _time = 0U;

        public:

            // Public lifecycle.

            /// Initializes the clock value.
            void Initialize() {
                _time = 1000U;
            }

            // Public clock access.

            /// Returns the current example clock value.
            std::uint64_t Now() const {
                return _time;
            }

    };


    /// Complete architecture selected by the application Bootstrap.
    using ApplicationComposition = Composition::Composition<
        PlatformDomain,
        ExampleClock
    >;


    /// Reusable timing consumer that knows only the Composition contract.
    template<class TComposition>
    class TimingService final {

        private:

            // Resolved provider types.

            /// Concrete clock type selected by the supplied Composition.
            using ClockProvider = typename TComposition::template ProviderFor<SystemClock>;

            // Runtime dependencies.

            /// Runtime clock instance supplied by the Bootstrap.
            ClockProvider& _clock;

        public:

            // Public construction.

            /// Creates the service from the clock provider selected by the Composition.
            explicit TimingService(ClockProvider& clock) : _clock(clock) {}

            // Public timing access.

            /// Returns the current time without knowing the concrete clock type in advance.
            std::uint64_t CurrentTime() const {
                return _clock.Now();
            }

    };


    /// Application Bootstrap that owns the selected provider and wires it into the reusable service.
    class ApplicationRuntime final {

        private:

            // Runtime providers.

            /// Clock provider selected by ApplicationComposition.
            ApplicationComposition::ProviderFor<SystemClock> _clock;

            // Runtime consumers.

            /// Timing service consuming the application Composition.
            TimingService<ApplicationComposition> _timing;

        public:

            // Public construction.

            /// Wires Bootstrap-owned providers into Composition-aware consumers.
            ApplicationRuntime() : _timing(_clock) {}

            // Public lifecycle.

            /// Initializes the selected clock provider.
            void Initialize() {
                _clock.Initialize();
            }

            /// Runs the application through the reusable timing service.
            int Run() const {
                return _timing.CurrentTime() > 0U ? 0 : 1;
            }

    };

} // Example


/// Executes the reusable-library Composition example.
int main() {
    Example::ApplicationRuntime application;

    application.Initialize();

    return application.Run();
}
```

`TimingService` does not contain the type name `ExampleClock`.

The concrete clock could instead be declared in another repository entirely:

```text
EDP-System/
    defines SystemClock contract

EDP-ESP32/
    defines Esp32Clock

EDP-RTC/
    defines Ds3231Clock

MyApplication/
    Bootstrap chooses one of them
```

As long as the Bootstrap can include the selected provider and the provider satisfies the Composition contract, `TimingService` does not care where that implementation was defined.

## Hard-coded equivalent

Without Composition, the reusable service itself must name the concrete implementation:

```cpp
#include <cstdint>

namespace Example {

    /// Concrete clock implementation selected by the application.
    class ExampleClock final {

        private:

            // Runtime state.

            /// Current example clock value.
            std::uint64_t _time = 0U;

        public:

            // Public lifecycle.

            /// Initializes the clock value.
            void Initialize() {
                _time = 1000U;
            }

            // Public clock access.

            /// Returns the current example clock value.
            std::uint64_t Now() const {
                return _time;
            }

    };


    /// Timing consumer coupled directly to ExampleClock.
    class TimingService final {

        private:

            // Runtime dependencies.

            /// Specifically required ExampleClock implementation.
            ExampleClock& _clock;

        public:

            // Public construction.

            /// Creates the service from the specifically required ExampleClock implementation.
            explicit TimingService(ExampleClock& clock) : _clock(clock) {}

            // Public timing access.

            /// Returns the current time.
            std::uint64_t CurrentTime() const {
                return _clock.Now();
            }

    };


    /// Application Bootstrap owning the hard-coded clock implementation.
    class ApplicationRuntime final {

        private:

            // Runtime providers.

            /// Concrete clock implementation selected directly by the application.
            ExampleClock _clock;

            // Runtime consumers.

            /// Service whose own declaration is also coupled to ExampleClock.
            TimingService _timing;

        public:

            // Public construction.

            /// Wires the hard-coded clock into the hard-coded timing service.
            ApplicationRuntime() : _timing(_clock) {}

            // Public lifecycle.

            /// Initializes the concrete clock.
            void Initialize() {
                _clock.Initialize();
            }

            /// Runs the application through the timing service.
            int Run() const {
                return _timing.CurrentTime() > 0U ? 0 : 1;
            }

    };

} // Example


/// Executes the hard-coded reusable-library equivalent.
int main() {
    Example::ApplicationRuntime application;

    application.Initialize();

    return application.Run();
}
```

The hard-coded `TimingService` cannot be moved unchanged into an environment that uses `Ds3231Clock`, `DesktopClock`, or `TestClock`. Its declaration itself depends on `ExampleClock`.

The Composition version can.

Only the Bootstrap Composition changes:

```cpp
using ApplicationComposition = Composition::Composition<
    PlatformDomain,
    Ds3231Clock
>;
```

and the Bootstrap-owned member:

```cpp
ApplicationComposition::ProviderFor<SystemClock> _clock;
```

automatically becomes `Ds3231Clock` at compile time.

`TimingService<ApplicationComposition>` resolves the same selected type without ever naming it directly.

This is the intended ESPressio architecture:

> **Reusable libraries define and consume contracts. Bootstrap code selects concrete implementations.**

---

# Qualified Provider Resolution

A `SharedCapability` may legitimately have several concrete providers. Capability identity alone may therefore be insufficient for a reusable consumer that requires one particular provider.

Providers can attach open-ended Attributes to an individual Offer:

```cpp
Composition::Offer<
    Radio,
    Composition::Attribute<
        "Frequency",
        24917U
    >,
    Composition::TextAttribute<
        "Role",
        "Telemetry"
    >
>
```

A consumer can then express the qualifier in its Need:

```cpp
using TelemetryRadioRequirement = Composition::Need<
    Radio,
    Composition::AttributeEquals<
        "Frequency",
        24917U
    >,
    Composition::TextAttributeEquals<
        "Role",
        "Telemetry"
    >
>;
```

A Composition exposes qualified count, predicate, plural, and singular queries:

```cpp
ApplicationComposition::ProviderCountSatisfying<TelemetryRadioRequirement>
ApplicationComposition::HasProviderSatisfying<TelemetryRadioRequirement>
ApplicationComposition::ProvidersSatisfying<TelemetryRadioRequirement>
ApplicationComposition::ProviderSatisfying<TelemetryRadioRequirement>
```

`ProviderSatisfying<TNeed>` requires exactly one matching provider. Zero matches and ambiguous multiple matches are compile-time errors.

Existing `ProviderFor<TCapability>` and `ProvidersFor<TCapability>` semantics remain unchanged.

For the complete contract, including C++20 text Attributes, ambiguity handling, runtime-cost guarantees, and complete examples, see [Qualified Composition Resolution](./COMPOSITION-QUALIFIED-RESOLUTION.MD).

---

# Cross-Domain Architecture Validation

`Requires<...>` remains a same-domain contract and retains its existing semantics.

When a provider in one Domain depends on a capability from another Domain, it declares the dependency separately through `DependsOn<...>`:

```cpp
class TelemetryProvider final : public Composition::Provider<
    ServiceDomain,
    Composition::Provides<
        Composition::Offer<TelemetryService>
    >,
    Composition::Requires<>,
    Composition::DependsOn<
        TelemetryRadioRequirement
    >
> {};
```

Each Domain is still validated independently by its own `Composition<...>`.

A higher-level `Architecture<...>` validates the cross-domain dependencies across those already-valid Compositions:

```cpp
using ApplicationArchitecture = Composition::Architecture<
    RadioComposition,
    ServiceComposition
>;
```

`Architecture` is compile-time structural validation only. It does not construct objects, own provider instances, allocate memory, establish lifecycle order, or perform runtime dependency injection.

For the complete contract, see [Qualified Composition Resolution](./COMPOSITION-QUALIFIED-RESOLUTION.MD).

---

# Compile-Time Validation

The framework rejects invalid architectures as early as possible.

## Missing Required Capability

A provider declares:

```cpp
Composition::Requires<
    Composition::Need<SystemClock>
>
```

but no provider in the Composition offers `SystemClock`.

The Composition is rejected during compilation.

## Duplicate Exclusive Capability

Two providers both offer the same `ExclusiveCapability`.

The Composition is rejected during compilation.

This prevents two implementations from silently competing to become a definitive service such as the System Clock.

## Property Requirement Not Satisfied

A scheduler requires:

```cpp
Composition::AtMost<
    ClockResolutionNanoseconds,
    1000U
>
```

but the selected clock advertises:

```cpp
Composition::PropertyValue<
    ClockResolutionNanoseconds,
    1000000U
>
```

The Composition is rejected during compilation.

## Flag Property Requirement Not Satisfied

A provider advertises a flag-valued Property but does not contain the all/any/none feature relationship required by a `Need`.

That provider does not satisfy the Need and is excluded by the existing provider-matching machinery.

A typed flag advertisement or constraint using the wrong enum domain, an out-of-range flag position, or an empty all/any/none constraint is rejected during compilation.

## Wrong Domain

A capability, provider, or requirement from one Composition Domain is accidentally placed into another Domain.

The Composition is rejected during compilation.

These failures are intentional. Architectural incompatibility should be discovered while the firmware is being built, not after it has been deployed.

---

# Common Mistakes

## Treating Composition as Object Ownership

This is incorrect thinking:

> The Composition owns my clock.

It does not.

This:

```cpp
using ApplicationComposition = Composition::Composition<
    PlatformDomain,
    ExampleClock
>;
```

describes a compile-time architecture.

This:

```cpp
ApplicationComposition::ProviderFor<SystemClock> _clock;
```

creates the runtime object because `_clock` is an ordinary C++ member whose type was resolved through that architecture.

## Expecting Runtime Lookup

There is no call such as:

```cpp
composition.Get<SystemClock>();
```

Provider resolution happens in the type system.

## Putting Concrete Types Back Into Reusable Consumers

This defeats the purpose:

```cpp
class TimingService {

    private:

        /// Hard-coded platform dependency.
        Esp32Clock& _clock;

};
```

If the consumer only needs the System Clock capability, resolve the provider through its Composition contract instead.

## Assuming Provider Location Matters

Composition does not require providers to live beside the capability contract.

A concrete provider can come from another library, another repository, or application-local source code. The build system and Bootstrap must make the type visible; the Composition contract does not care about its physical path.

## Using Shared Capability Resolution as Though It Were Exclusive

For a capability with multiple valid providers, use:

```cpp
ApplicationComposition::ProvidersFor<CommunicationInterface>
```

rather than:

```cpp
ApplicationComposition::ProviderFor<CommunicationInterface>
```

`ProviderFor<TCapability>` requires exactly one provider.

## Using Composition for Runtime State

A property such as clock resolution may be appropriate because it describes what the compiled provider **is**.

A value such as current battery voltage is not appropriate because it describes what the running system **is currently doing**.

---

# Choosing What Belongs in Composition

Composition should describe architectural facts that are known at compile time.

Good examples include:

- this provider supplies a System Clock;
- this provider requires Persistent Storage;
- this radio supports a maximum packet size of 250 bytes;
- this radio supports Broadcast, ReliableDelivery, and ReceiveTimestamp feature flags;
- this scheduler requires a clock resolution of at most one microsecond;
- this application contains three shared communication interfaces.

A finite flag-valued Property is appropriate when several orthogonal compile-time characteristics belong to **one** capability. It must not be used to collapse separate architectural services such as Clock, Persistence, Radio, and Encryption into one feature mask.

Composition should generally **not** describe runtime state.

Poor examples include:

- the current Wi-Fi signal strength;
- whether a device is presently connected;
- how many messages are currently queued;
- the current battery voltage;
- whether initialization succeeded.

Those values change while the program is running and therefore belong to runtime APIs rather than compile-time Composition.

A useful rule is:

> If the value describes what the compiled system **is capable of**, Composition may be appropriate.

> If the value describes what the running system **is currently doing**, it probably belongs elsewhere.

---

# Public Vocabulary Reference

## `Domain`

Base type used to declare one Composition Domain.

```cpp
struct PlatformDomain final : Composition::Domain {};
```

## `ExclusiveCapability<TDomain>`

Declares a capability for which at most one provider may exist in a Composition.

```cpp
struct SystemClock final : Composition::ExclusiveCapability<PlatformDomain> {};
```

## `SharedCapability<TDomain>`

Declares a capability that may be supplied by multiple providers.

```cpp
struct CommunicationInterface final : Composition::SharedCapability<PlatformDomain> {};
```

## `Property<TCapability, TValue>`

Declares a compile-time property belonging specifically to one capability.

```cpp
struct ClockResolutionNanoseconds final : Composition::Property<SystemClock, std::uint32_t> {};
```

## `FlagProperty<TCapability, TFlag, TStorage>`

Declares a compile-time Property whose value is a finite set of typed enum-domain flags.

~~~cpp
struct SupportedFeatures final : Composition::FlagProperty<
    CommunicationInterface,
    CommunicationFeature,
    std::uint8_t
> {};
~~~

## `FlagPropertyValue<TProperty, TFlags...>`

Associates zero or more typed flags with a concrete `FlagProperty` in an `Offer`.

~~~cpp
Composition::FlagPropertyValue<
    SupportedFeatures,
    CommunicationFeature::Broadcast,
    CommunicationFeature::ReceiveTimestamp
>
~~~

## `PropertyValue<TProperty, TValue>`

Associates a compile-time value with a property in an `Offer`.

```cpp
Composition::PropertyValue<
    ClockResolutionNanoseconds,
    1000U
>
```

## `Offer<TCapability, ...>`

Declares that a provider supplies one capability and optionally advertises property values for it.

```cpp
Composition::Offer<
    SystemClock,
    Composition::PropertyValue<
        ClockResolutionNanoseconds,
        1000U
    >
>
```

## `Provides<...>`

Groups the capability offers supplied by one provider.

```cpp
Composition::Provides<
    Composition::Offer<SystemClock>
>
```

## `Need<TCapability, ...>`

Declares one required capability and optionally constrains its properties.

```cpp
Composition::Need<
    SystemClock,
    Composition::AtMost<
        ClockResolutionNanoseconds,
        1000U
    >
>
```

## `Requires<...>`

Groups the requirements belonging to one provider.

```cpp
Composition::Requires<
    Composition::Need<SystemClock>
>
```

## `Provider<TDomain, TProvides, TRequires>`

Base declaration used by a concrete provider to publish its compile-time contract.

```cpp
class ExampleClock final : public Composition::Provider<
    PlatformDomain,
    Composition::Provides<
        Composition::Offer<SystemClock>
    >
> {};
```

## `Composition<TDomain, ...>`

Combines the concrete provider types selected by the Bootstrap and validates the architecture.

```cpp
using ApplicationComposition = Composition::Composition<
    PlatformDomain,
    ExampleClock
>;
```

## `ProviderFor<TCapability>`

Resolves the single provider type supplying a capability.

```cpp
using ClockProvider = ApplicationComposition::ProviderFor<SystemClock>;
```

For a Composition containing `ExampleClock` as the sole `SystemClock` provider, `ClockProvider` is exactly the type `ExampleClock`.

## `ProvidersFor<TCapability>`

Returns the compile-time provider list for a capability that may have multiple providers.

```cpp
using CommunicationProviders = ApplicationComposition::ProvidersFor<CommunicationInterface>;
```

## `Attribute<TName, TValue>`

Attaches one open-ended compile-time descriptor to a capability Offer.

```cpp
Composition::Attribute<
    "Frequency",
    24917U
>
```

## `TextAttribute<TName, TValue>`

Convenience alias for a compile-time textual Attribute.

```cpp
Composition::TextAttribute<
    "StorageType",
    "SD"
>
```

## `AttributeEquals<TName, TValue>`

Requires an Attribute to exactly match one compile-time value.

```cpp
Composition::AttributeEquals<
    "Frequency",
    24917U
>
```

## `TextAttributeEquals<TName, TValue>`

Requires a textual Attribute to exactly match one compile-time string.

```cpp
Composition::TextAttributeEquals<
    "StorageType",
    "SD"
>
```

## `ProviderSatisfying<TNeed>`

Returns the single provider satisfying a complete qualified Need.

```cpp
using RadioProvider = ApplicationComposition::ProviderSatisfying<
    TelemetryRadioRequirement
>;
```

Exactly one provider must satisfy the Need.

## `ProvidersSatisfying<TNeed>`

Returns every provider satisfying a complete qualified Need.

```cpp
using RadioProviders = ApplicationComposition::ProvidersSatisfying<
    BroadRadioRequirement
>;
```

## `DependsOn<...>`

Declares cross-domain capability Needs belonging to one provider without changing the same-domain meaning of `Requires<...>`.

## `Architecture<...>`

Combines already-valid Domain Compositions and validates their providers' cross-domain `DependsOn<...>` requirements.

## Property Constraints

The framework currently provides:

```cpp
Composition::Equals<TProperty, TValue>
Composition::AtLeast<TProperty, TValue>
Composition::AtMost<TProperty, TValue>
Composition::GreaterThan<TProperty, TValue>
Composition::LessThan<TProperty, TValue>
```

Flag-valued Properties additionally provide:

~~~cpp
Composition::HasAllFlags<TProperty, TFlags...>
Composition::HasAnyFlags<TProperty, TFlags...>
Composition::HasNoFlags<TProperty, TFlags...>
~~~

These constraints are evaluated during Composition validation rather than at runtime.
