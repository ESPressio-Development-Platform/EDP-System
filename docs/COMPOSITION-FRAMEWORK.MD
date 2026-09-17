# Composition Framework

The ESPressio Composition Framework describes the compile-time architecture of an application or library.

It allows code to describe, in a form the compiler can verify:

- what capabilities exist;
- which providers supply those capabilities;
- which capabilities each provider requires;
- which compile-time properties a capability provider advertises;
- which property values a consumer requires;
- whether the complete collection of providers is valid.

The framework is deliberately **not** a runtime dependency-injection container. It does not construct provider objects, own provider objects, allocate memory, start services, stop services, or decide runtime initialization order.

Its job is to answer one architectural question before the program runs:

> **Is this collection of components structurally compatible?**

Runtime ownership and lifecycle remain ordinary C++ responsibilities.

---

# Contents

1. [The Problem Composition Solves](#the-problem-composition-solves)
2. [The Basic Concepts](#the-basic-concepts)
3. [Example 1 - Your First Composition](#example-1---your-first-composition)
4. [Example 2 - Requirements and Properties](#example-2---requirements-and-properties)
5. [Example 3 - Shared Capabilities](#example-3---shared-capabilities)
6. [Example 4 - Consuming Composition from a Reusable Library](#example-4---consuming-composition-from-a-reusable-library)
7. [Compile-Time Validation](#compile-time-validation)
8. [Common Mistakes](#common-mistakes)
9. [Choosing What Belongs in Composition](#choosing-what-belongs-in-composition)
10. [Public Vocabulary Reference](#public-vocabulary-reference)

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

Composition allows the library to express that architectural requirement as:

> A `SystemClock` capability must be available.

A concrete platform can separately declare:

> `Esp32Clock` provides `SystemClock`.

The application then combines those providers into a Composition. The compiler verifies whether all declared requirements are satisfied.

Composition can also express stronger requirements. For example:

> A `SystemClock` must exist, and its resolution must be no worse than 1000 nanoseconds.

That requirement can also be checked at compile time.

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

## Constraint

A **Constraint** describes an acceptable value for a required property.

The initial framework provides:

- `Equals<TProperty, TValue>`;
- `AtLeast<TProperty, TValue>`;
- `AtMost<TProperty, TValue>`;
- `GreaterThan<TProperty, TValue>`;
- `LessThan<TProperty, TValue>`.

## Composition

A `Composition` combines providers belonging to one Domain, validates the complete provider set, and exposes compile-time queries for resolving providers and their properties.

---

# Example 1 - Your First Composition

This example contains one Domain, one exclusive capability, one provider, and one runtime owner.

It is a complete program.

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

        private:

            // Runtime state.

            /// Indicates whether runtime initialization has completed.
            bool _initialized = false;

    };


    /// Complete compile-time architecture for the example application.
    using ApplicationComposition = Composition::Composition<
        PlatformDomain,
        ExampleClock
    >;


    /// Owns the runtime objects selected by the compile-time composition.
    class ApplicationRuntime final {

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

        private:

            // Runtime providers.

            /// Provider selected by the composition for the SystemClock capability.
            ApplicationComposition::ProviderFor<SystemClock> _clock;

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

The Composition validates the architecture. It does not own `_clock`.

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

        private:

            // Runtime state.

            /// Indicates whether the clock provider has completed initialization.
            bool _initialized = false;

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

        public:

            // Public lifecycle.

            /// Initializes the scheduler from the selected clock provider.
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
                return _started;
            }

        private:

            // Runtime state.

            /// Time observed during scheduler initialization.
            std::uint64_t _initializationTime = 0U;

            /// Indicates whether scheduler execution has started.
            bool _started = false;

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

        private:

            // Runtime providers.

            /// Provider selected for SystemClock.
            ApplicationComposition::ProviderFor<SystemClock> _clock;

            /// Provider selected for TaskScheduler.
            ApplicationComposition::ProviderFor<TaskScheduler> _scheduler;

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

---

# Example 3 - Shared Capabilities

An exclusive capability has one definitive provider. A shared capability is intentionally plural.

This example supplies the same communication capability through Wi-Fi and Bluetooth.

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

        private:

            // Runtime state.

            /// Indicates whether this interface has started.
            bool _started = false;

    };


    /// Bluetooth implementation of the shared communication capability.
    class BluetoothInterface final : public Composition::Provider<
        PlatformDomain,
        Composition::Provides<
            Composition::Offer<CommunicationInterface>
        >
    > {

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

        private:

            // Runtime state.

            /// Indicates whether this interface has started.
            bool _started = false;

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


    /// Owns both runtime communication providers.
    class ApplicationRuntime final {

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

        private:

            // Runtime providers.

            /// Wi-Fi provider participating in the shared capability.
            WifiInterface _wifi;

            /// Bluetooth provider participating in the shared capability.
            BluetoothInterface _bluetooth;

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

`ProviderFor<TCapability>` is intended for cases where exactly one provider must be resolved.

---

# Example 4 - Consuming Composition from a Reusable Library

A reusable library should depend on capabilities rather than concrete platform implementations wherever possible.

The following `TimingService` resolves its clock provider through the supplied Composition.

```cpp
#include <cstdint>

#include "ESPressio_System.hpp"

namespace Example {

    namespace Composition = ESPressio::System::CompositionFramework;

    /// Domain used by the reusable-library example.
    struct PlatformDomain final : Composition::Domain {};


    /// Definitive clock capability consumed by TimingService.
    struct SystemClock final : Composition::ExclusiveCapability<PlatformDomain> {};


    /// Concrete clock provider selected by this application.
    class ExampleClock final : public Composition::Provider<
        PlatformDomain,
        Composition::Provides<
            Composition::Offer<SystemClock>
        >
    > {

        public:

            // Public lifecycle.

            /// Initializes the clock provider.
            void Initialize() {
                _initialized = true;
            }

            // Public clock access.

            /// Returns a placeholder time value.
            std::uint64_t Now() const {
                if (!_initialized) { return 0U; }

                return 1000U;
            }

        private:

            // Runtime state.

            /// Indicates whether initialization has completed.
            bool _initialized = false;

    };


    /// Compile-time application architecture.
    using ApplicationComposition = Composition::Composition<
        PlatformDomain,
        ExampleClock
    >;


    /// Reusable service consuming whichever provider supplies SystemClock.
    template<class TComposition>
    class TimingService {

        public:

            // Resolved composition types.

            /// Concrete provider selected by the supplied Composition for SystemClock.
            using ClockProvider = typename TComposition::template ProviderFor<SystemClock>;

            // Public lifecycle.

            /// Connects this service to the runtime clock-provider instance.
            void Initialize(ClockProvider& clockProvider) {
                _clockProvider = &clockProvider;
            }

            // Public timing access.

            /// Returns the current time reported by the selected clock provider.
            std::uint64_t Now() const {
                if (_clockProvider == nullptr) { return 0U; }

                return _clockProvider->Now();
            }

        private:

            // Runtime dependencies.

            /// Runtime provider instance supplied by the owning application.
            ClockProvider* _clockProvider = nullptr;

    };


    /// Owns the provider and the reusable service consuming that provider.
    class ApplicationRuntime final {

        public:

            // Public lifecycle.

            /// Initializes the provider before connecting the reusable service to it.
            void Initialize() {
                _clock.Initialize();
                _timing.Initialize(_clock);
            }

            /// Runs the example application.
            int Run() const {
                return _timing.Now() > 0U ? 0 : 1;
            }

        private:

            // Runtime providers and consumers.

            /// Concrete SystemClock provider selected by the Composition.
            ApplicationComposition::ProviderFor<SystemClock> _clock;

            /// Reusable service specialized against the application Composition.
            TimingService<ApplicationComposition> _timing;

    };

} // Example


/// Executes the reusable-library example.
int main() {
    Example::ApplicationRuntime application;

    application.Initialize();

    return application.Run();
}
```

The service is parameterized by a Composition rather than by `ExampleClock` itself. A different application can supply another valid System Clock provider without changing the service implementation.

---

# Compile-Time Validation

The framework deliberately rejects invalid architectures during compilation.

## Missing Required Capability

If a provider declares a `Need<SystemClock>` but the Composition contains no provider for `SystemClock`, compilation fails with:

```text
Composition contains an unsatisfied provider requirement
```

## Duplicate Exclusive Capability

If two providers supply the same `ExclusiveCapability`, compilation fails with:

```text
Composition contains multiple providers for an ExclusiveCapability
```

## Unsatisfied Property Constraint

If a scheduler requires:

```cpp
Composition::AtMost<ClockResolutionNanoseconds, 1000U>
```

but the available clock advertises a larger value, the provider requirement is unsatisfied and compilation fails.

## Wrong Domain

A provider cannot silently offer or require a capability belonging to another Domain.

Cross-domain declarations are rejected during compilation.

## Wrong Property for a Capability

A `PropertyValue` attached to an Offer must belong to the capability being offered.

A property declared for `SystemClock`, for example, cannot be attached to an Offer for `CommunicationInterface`.

---

# Common Mistakes

## Treating Composition as Runtime Dependency Injection

Composition does not contain runtime provider instances. There is no runtime service locator hidden inside it.

Resolve types through the Composition, then let normal C++ own and initialize those types.

## Putting Runtime State into Properties

A compile-time maximum packet size can be a good Property.

Current signal strength is not. Signal strength changes while the application is running and belongs in a runtime API.

## Using an Exclusive Capability for Something Naturally Plural

If several providers are legitimately allowed at the same time, model the capability as `SharedCapability`.

Do not make something exclusive merely because the first implementation happens to have only one provider.

## Querying `ProviderFor` for a Plural Capability

`ProviderFor<TCapability>` requires exactly one matching provider.

For shared capabilities, use `ProvidersFor<TCapability>` when multiple providers may exist.

## Coupling a Reusable Consumer Back to One Platform Provider

This defeats the purpose of composition:

```cpp
class TimingService {
    Esp32Clock* _clock;
};
```

Prefer resolving the provider through the supplied Composition:

```cpp
using ClockProvider = typename TComposition::template ProviderFor<SystemClock>;
```

## Duplicating the Same Offer or Need

A provider must not offer the same capability twice inside one `Provides` declaration.

A provider must not declare the same required capability twice inside one `Requires` declaration.

Place multiple property constraints inside one `Need` instead.

---

# Choosing What Belongs in Composition

Composition should describe architectural facts that are known at compile time.

Good examples include:

- this provider supplies a System Clock;
- this provider requires Persistent Storage;
- this radio supports a compile-time maximum packet size of 250 bytes;
- this scheduler requires a clock resolution no worse than one microsecond;
- this application contains multiple communication-interface providers.

Composition should generally not describe values that change while the program is running.

Poor examples include:

- current Wi-Fi signal strength;
- whether a device is currently connected;
- current queue depth;
- current battery voltage;
- whether initialization succeeded;
- current clock time.

A useful rule is:

> If the value describes what the compiled system **is capable of**, Composition may be appropriate.

> If the value describes what the running system **is currently doing**, it probably belongs in a runtime API.

---

# Public Vocabulary Reference

## `Domain`

Base type for a compile-time composition Domain.

## `ExclusiveCapability<TDomain>`

Base type for a capability that may have at most one provider in a Composition.

## `SharedCapability<TDomain>`

Base type for a capability that may have multiple providers in a Composition.

## `Property<TCapability, TValue>`

Base type for a compile-time property belonging to one capability.

## `PropertyValue<TProperty, TValue>`

Associates a compile-time value with a Property.

## `Offer<TCapability, ...>`

Declares one capability supplied by a provider, optionally with PropertyValue declarations.

## `Provides<...>`

Groups all Offers supplied by a provider.

## `Need<TCapability, ...>`

Declares one capability required by a provider, optionally with property Constraints.

## `Requires<...>`

Groups all Needs declared by a provider.

## `Provider<TDomain, TProvides, TRequires>`

Base declaration describing a provider's compile-time relationship with the framework. `TRequires` defaults to an empty `Requires<>` declaration.

## Property Constraints

The initial framework provides:

- `Equals<TProperty, TValue>`;
- `AtLeast<TProperty, TValue>`;
- `AtMost<TProperty, TValue>`;
- `GreaterThan<TProperty, TValue>`;
- `LessThan<TProperty, TValue>`.

## `Composition<TDomain, ...>`

Validates the complete provider architecture and exposes compile-time queries including:

- `Provides<TCapability>`;
- `ProviderCountFor<TCapability>`;
- `ProviderFor<TCapability>`;
- `ProvidersFor<TCapability>`;
- `PropertiesFor<TCapability>`;
- `PropertiesForProvider<TProvider, TCapability>`;
- `HasProperty<TCapability, TProperty>`;
- `PropertyValue<TCapability, TProperty>`.

---

# Final Guidance

Keep Composition declarations descriptive.

A developer reading a provider declaration should be able to understand it as a short architectural statement:

> This provider supplies these capabilities, with these compile-time characteristics, and requires these other capabilities under these constraints.

The implementation of those capabilities remains ordinary C++.

The ownership and lifecycle of runtime provider objects remain ordinary C++.

The Composition Framework exists to make the relationships between those pieces explicit and compiler-verifiable.
