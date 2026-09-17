# System Identity

System Identity defines the smallest universal identity model required by ESPressio applications.

The facility is intentionally narrow. It establishes **which device is running** and **which durable runtime incarnation of that device is currently executing**. It does not own transport addressing, cryptographic credentials, product classification, Mesh membership, OTA transaction identity, or any other subsystem-specific identifier.

The design is memory-first. Once established, the complete System-owned runtime identity occupies exactly **20 bytes**:

```text
DeviceIdentifier          16 bytes
RuntimeIncarnationId       4 bytes
                         --------
SystemIdentity            20 bytes
```

No heap allocation, mutex, atomic publication state, registry, callback list, provider pointer, or lifecycle worker is retained by `SystemIdentity`.

## Contents

1. [What System Identity Means](#what-system-identity-means)
2. [Identity Is a System Subdomain](#identity-is-a-system-subdomain)
3. [Memory Model](#memory-model)
4. [DeviceIdentifier](#deviceidentifier)
5. [RuntimeIncarnationId](#runtimeincarnationid)
6. [SystemIdentity](#systemidentity)
7. [Composition Capabilities](#composition-capabilities)
8. [Bootstrap Ordering](#bootstrap-ordering)
9. [Zero-Copy Access and Lifetime](#zero-copy-access-and-lifetime)
10. [Example 1 - Hard-Coded Identity Providers](#example-1---hard-coded-identity-providers)
11. [Example 2 - Composition-Based Identity Providers](#example-2---composition-based-identity-providers)
12. [Example 3 - Complete Bootstrap and Consumer Lifetime](#example-3---complete-bootstrap-and-consumer-lifetime)
13. [Runtime Incarnation Durability Contract](#runtime-incarnation-durability-contract)
14. [What Does Not Belong in System Identity](#what-does-not-belong-in-system-identity)
15. [Testing and Dummy Providers](#testing-and-dummy-providers)
16. [Common Mistakes](#common-mistakes)
17. [Public Vocabulary Reference](#public-vocabulary-reference)

## What System Identity Means

System Identity answers two universal questions:

```text
Which device is this?
        -> DeviceIdentifier

Which running incarnation of that device is this?
        -> RuntimeIncarnationId
```

Together they form:

```text
(DeviceIdentifier, RuntimeIncarnationId)
        -> SystemIdentity
```

The distinction is deliberate.

A device remains the same device across application restarts. Its `DeviceIdentifier` therefore remains stable.

A new application runtime must not be confused with an earlier runtime of the same device. Its `RuntimeIncarnationId` therefore advances durably before the new runtime identity is exposed.

A consumer that needs to distinguish exact runtimes can use both values. A consumer that cares only which physical/logical device it is dealing with can use only the `DeviceIdentifier`.

## Identity Is a System Subdomain

Identity is defined under:

```cpp
ESPressio::System::Identity
```

The namespace owns the universal semantic values and the Composition contracts required to establish them.

It deliberately does **not** own the concrete mechanisms used to create those values.

A platform or hardware integration is responsible for the best available device-identity mechanism for that target. A future persistence implementation, monotonic hardware facility, secure element, or other suitable provider can satisfy the runtime-incarnation contract.

The intended dependency direction is:

```text
EDP-System
    owns identity semantics and capability contracts

Platform / hardware library
    supplies DeviceIdentity

Persistence / hardware durability facility
    supplies RuntimeIncarnationIdentity

Application Bootstrap
    selects providers through Composition
    establishes the two semantic values
    constructs SystemIdentity

Higher-level libraries
    borrow SystemIdentity or the specific immutable identifier they require
```

System does not depend on a specific platform or persistence library merely because those libraries provide concrete implementations.

## Memory Model

Memory conservation is a hard architectural requirement.

The retained System-owned state is intentionally limited to the identity information itself:

| Type | Retained semantic state | Required size |
| --- | ---: | ---: |
| `DeviceIdentifier` | 128-bit device value | 16 bytes |
| `RuntimeIncarnationId` | 32-bit incarnation scalar | 4 bytes |
| `SystemIdentity` | both values | 20 bytes |

The implementation contains compile-time size guards. An implementation change that silently expands these types must therefore fail compilation rather than quietly increasing retained DRAM usage.

`SystemIdentity` contains no:

- heap allocation;
- owning or provider pointer;
- virtual table;
- mutex;
- atomic publication flag;
- observer collection;
- callback registry;
- hidden lifecycle state;
- duplicate validity byte.

Validity is derived from the semantic values already stored. Zero is reserved as Invalid/Unspecified for each constituent identity type, so no separate Boolean is required.

Provider objects used while Bootstrap establishes identity are not retained by `SystemIdentity`. If a provider has no other application responsibility, it may cease to exist after Bootstrap has resolved the semantic value. If the same underlying platform or persistence object is needed elsewhere, its lifetime is owned by that subsystem rather than by System Identity.

## DeviceIdentifier

`DeviceIdentifier` is a transport-independent 128-bit identity of one device.

```cpp
using namespace ESPressio::System::Identity;

const DeviceIdentifier::Storage bytes{
    0x10U, 0x21U, 0x32U, 0x43U,
    0x54U, 0x65U, 0x76U, 0x87U,
    0x98U, 0xA9U, 0xBAU, 0xCBU,
    0xDCU, 0xEDU, 0xFEU, 0x0FU
};

const DeviceIdentifier device(bytes);
```

The all-zero representation is Invalid/Unspecified:

```cpp
DeviceIdentifier device;

if (!device.IsValid()) {
    // No valid device identity has been resolved into this staging value yet.
}
```

A `DeviceIdentifier` is **not** any of the following:

```text
MAC address
IP address
Radio address
Mesh alias
public key
certificate
serial display name
product type
hardware family
OTA target identifier
```

A concrete platform may legitimately use an immutable hardware property such as a factory identifier as input when deriving a `DeviceIdentifier`. That does not make the source property and `DeviceIdentifier` the same semantic type.

System owns the meaning and 128-bit representation. The concrete provider owns the platform-specific derivation policy.

## RuntimeIncarnationId

`RuntimeIncarnationId` is a 32-bit non-zero scalar identifying one running incarnation of a particular device.

```cpp
RuntimeIncarnationId incarnation(42U);
```

Zero is Invalid/Unspecified.

The value is intentionally only 32 bits because it does not require global uniqueness by itself. Its namespace is the permanent device identity:

```text
DeviceIdentifier + RuntimeIncarnationId
```

A 32-bit value therefore supplies 4,294,967,295 valid non-zero incarnation values per device without imposing the retained cost of another 64-bit or 128-bit identifier.

The incarnation is not a random session token. The baseline contract is a durable monotonic counter:

```text
previous durable value
        ↓
verify that another value is available
        ↓
next = previous + 1
        ↓
durably commit next
        ↓
ONLY THEN expose RuntimeIncarnationId(next)
```

If the current durable value is the final representable non-zero value, the provider must fail rather than wrap to zero or reuse an older value.

## SystemIdentity

`SystemIdentity` is the immutable application-lifetime facade containing the permanent device identity and current runtime incarnation.

```cpp
SystemIdentity identity(
    deviceIdentifier,
    runtimeIncarnation
);
```

It is deliberately:

- non-default-constructible;
- non-copyable;
- non-movable;
- immutable through its public API;
- exactly 20 bytes;
- allocation-free;
- synchronization-free.

The object must be constructed in lifetime-stable storage and remain alive until every consumer that borrows from it has finished.

The prohibition on copying and moving the facade is not primarily about the cost of copying 20 bytes. It establishes a stronger lifetime contract: once consumers begin retaining pointers or references to its members, the facade cannot subsequently be relocated through ordinary C++ copy/move operations.

The constituent semantic value types remain copyable because Bootstrap needs temporary staging values and an individual consumer may deliberately choose to own a copy.

## Composition Capabilities

Identity defines its own Composition domain:

```cpp
ESPressio::System::Identity::Domain
```

and two exclusive capabilities:

```cpp
DeviceIdentity
RuntimeIncarnationIdentity
```

### DeviceIdentity

`DeviceIdentity` means:

> This provider can establish the permanent `DeviceIdentifier` for the current device.

Only one provider may satisfy this capability in one Identity composition.

The concrete implementation belongs at the platform/hardware level that can make the best identity decision for that target.

Examples might eventually include hardware-fuse identity, a factory-provisioned immutable identifier, a secure-device identity, or another platform-appropriate source.

The System library deliberately does not choose among them.

The examples use a bootstrap operation shaped like:

```cpp
bool Resolve(
    ESPressio::System::Identity::DeviceIdentifier& identifier
) const noexcept;
```

A successful provider must return a valid non-zero `DeviceIdentifier`.

### RuntimeIncarnationIdentity

`RuntimeIncarnationIdentity` means:

> This provider can establish the next durable `RuntimeIncarnationId` for the current device.

Only one provider may satisfy this capability in one Identity composition.

The name describes the semantic facility rather than one current implementation mechanism. Persistence is the expected software mechanism, but System does not make Persistence itself part of the identity contract.

A future implementation could satisfy the same semantics through a hardware monotonic counter or another durable mechanism without changing System.

The examples use a bootstrap operation shaped like:

```cpp
bool Establish(
    ESPressio::System::Identity::RuntimeIncarnationId& incarnation
) noexcept;
```

A successful provider must not return until the advancement of the value has become durable.

## Bootstrap Ordering

Initialization ordering is a logical invariant of the architecture.

The baseline order is:

```text
platform foundations required by Persistence
        ↓
Persistence / durable incarnation prerequisite
        ↓
System Identity establishment
        ↓
all remaining identity-consuming subsystems
```

Within the System Identity phase:

```text
resolve DeviceIdentifier
        ↓
establish and durably commit RuntimeIncarnationId
        ↓
construct SystemIdentity
        ↓
identity becomes immutable for application lifetime
```

Composition validates that the selected providers exist and obey exclusive-capability rules.

Composition does **not** initialize providers and does not become a runtime dependency-injection container. Bootstrap remains responsible for runtime construction and ordering.

This ordering also eliminates the need for identity-access synchronization.

Nothing is allowed to consume `SystemIdentity` before it exists. After it exists, nothing is allowed to mutate it. Therefore there is no concurrent publish/replace lifecycle for Identity to protect with atomics or locks.

## Zero-Copy Access and Lifetime

The normal accessors return const references:

```cpp
const DeviceIdentifier& Device() const noexcept;
const RuntimeIncarnationId& Incarnation() const noexcept;
```

No identifier bytes are copied by these calls.

A transient consumer can use a reference:

```cpp
void InspectIdentity(
    const ESPressio::System::Identity::SystemIdentity& identity
) noexcept {
    const auto& device = identity.Device();

    if (device.IsValid()) {
        // Inspect the existing value directly.
    }
}
```

A long-lived consumer that needs only the device identifier may retain a pointer:

```cpp
class Consumer final {
private:

    // Borrowed identity state.

    /// Application-lifetime pointer to the System-owned device identity.
    const ESPressio::System::Identity::DeviceIdentifier* _device;

public:

    // Construction.

    /// Borrows the permanent device identity from the application-lifetime facade.
    explicit Consumer(
        const ESPressio::System::Identity::SystemIdentity& identity
    ) noexcept :
        _device(&identity.Device()) {}
};
```

That stores only a pointer rather than another 16-byte identifier.

A consumer that genuinely needs independent ownership can opt into a copy explicitly:

```cpp
const auto copy = identity.Device();
```

The memory rule is therefore:

```text
inspect during one call
    -> const reference
    -> no retained copy

reuse for consumer lifetime
    -> const pointer/reference with guaranteed owner lifetime
    -> no identifier copy

independent ownership genuinely required
    -> explicit value copy
```

The application must keep `SystemIdentity` alive for at least as long as any consumer retains one of those pointers or references. The intended application Bootstrap naturally satisfies that requirement by constructing Identity before its consumers and destroying it after them.

## Example 1 - Hard-Coded Identity Providers

The following approach works, but hard-codes the exact implementations into Bootstrap:

```cpp
class Esp32DeviceIdentity final {
public:

    /// Resolves the concrete ESP32 device identity.
    bool Resolve(
        ESPressio::System::Identity::DeviceIdentifier& identifier
    ) const noexcept;
};

class PersistentIncarnationCounter final {
public:

    /// Establishes the next durable runtime incarnation.
    bool Establish(
        ESPressio::System::Identity::RuntimeIncarnationId& incarnation
    ) noexcept;
};

int Bootstrap() {
    Esp32DeviceIdentity deviceProvider;
    PersistentIncarnationCounter incarnationProvider;

    ESPressio::System::Identity::DeviceIdentifier device;
    ESPressio::System::Identity::RuntimeIncarnationId incarnation;

    if (!deviceProvider.Resolve(device)) return 1;
    if (!incarnationProvider.Establish(incarnation)) return 2;

    ESPressio::System::Identity::SystemIdentity identity(
        device,
        incarnation
    );

    return identity.IsValid() ? 0 : 3;
}
```

The runtime behaviour can be perfectly valid. The coupling problem is architectural:

```text
Bootstrap knows it must use Esp32DeviceIdentity
Bootstrap knows it must use PersistentIncarnationCounter
```

Changing either implementation requires changing the Bootstrap source that names it.

For one tiny application that may be acceptable. Across a reusable platform, however, concrete platform/storage choices spread easily and create long-term coupling.

## Example 2 - Composition-Based Identity Providers

Composition moves those concrete selections into the application architecture declaration.

First, concrete providers advertise the capability they satisfy:

```cpp
namespace Framework = ESPressio::System::CompositionFramework;
namespace Identity = ESPressio::System::Identity;

class Esp32DeviceIdentity final : public Framework::Provider<
    Identity::Domain,
    Framework::Provides<
        Framework::Offer<Identity::DeviceIdentity>
    >
> {
public:

    /// Resolves the concrete platform device identity.
    bool Resolve(Identity::DeviceIdentifier& identifier) const noexcept;
};

class PersistentIncarnationCounter final : public Framework::Provider<
    Identity::Domain,
    Framework::Provides<
        Framework::Offer<Identity::RuntimeIncarnationIdentity>
    >
> {
public:

    /// Establishes the next durable runtime incarnation.
    bool Establish(Identity::RuntimeIncarnationId& incarnation) noexcept;
};
```

The application selects them:

```cpp
using IdentityComposition = Framework::Composition<
    Identity::Domain,
    Esp32DeviceIdentity,
    PersistentIncarnationCounter
>;
```

Bootstrap resolves types by capability:

```cpp
using DeviceProvider =
    IdentityComposition::ProviderFor<Identity::DeviceIdentity>;

using IncarnationProvider =
    IdentityComposition::ProviderFor<Identity::RuntimeIncarnationIdentity>;
```

The concrete types are still known at compile time, so there is no runtime registry or virtual dispatch introduced by Composition.

If a different platform supplies:

```cpp
SecureElementDeviceIdentity
```

Bootstrap's capability-oriented code can continue to ask for:

```cpp
IdentityComposition::ProviderFor<Identity::DeviceIdentity>
```

The architectural selection changes in the composition declaration rather than leaking through every identity consumer.

### Hard-coded equivalent

Without Composition:

```cpp
Esp32DeviceIdentity deviceProvider;
PersistentIncarnationCounter incarnationProvider;
```

With Composition:

```cpp
IdentityComposition::ProviderFor<Identity::DeviceIdentity> deviceProvider;
IdentityComposition::ProviderFor<Identity::RuntimeIncarnationIdentity> incarnationProvider;
```

At runtime, both ultimately instantiate concrete C++ types. The important difference is **where those concrete types are selected**.

## Example 3 - Complete Bootstrap and Consumer Lifetime

A complete lifecycle should make the initialization boundary obvious:

```cpp
namespace Framework = ESPressio::System::CompositionFramework;
namespace Identity = ESPressio::System::Identity;

using IdentityComposition = Framework::Composition<
    Identity::Domain,
    PlatformDeviceIdentity,
    PersistentRuntimeIncarnationIdentity
>;

class Consumer final {
private:

    // Borrowed identity state.

    /// Stable pointer to the identity value required by this consumer.
    const Identity::DeviceIdentifier* _device;

public:

    // Construction.

    /// Creates the consumer only after System Identity has been established.
    explicit Consumer(const Identity::SystemIdentity& identity) noexcept :
        _device(&identity.Device()) {}

    /// Uses the borrowed identifier without copying it.
    bool Ready() const noexcept {
        return _device != nullptr && _device->IsValid();
    }
};

int BootstrapAndRun() {
    using DeviceProvider =
        IdentityComposition::ProviderFor<Identity::DeviceIdentity>;

    using IncarnationProvider =
        IdentityComposition::ProviderFor<Identity::RuntimeIncarnationIdentity>;

    DeviceProvider deviceProvider;
    IncarnationProvider incarnationProvider;

    if (!incarnationProvider.InitializePersistence()) return 1;

    Identity::DeviceIdentifier device;
    Identity::RuntimeIncarnationId incarnation;

    if (!deviceProvider.Resolve(device)) return 2;
    if (!incarnationProvider.Establish(incarnation)) return 3;

    Identity::SystemIdentity identity(
        device,
        incarnation
    );
    if (!identity.IsValid()) return 4;

    Consumer consumer(identity);
    if (!consumer.Ready()) return 5;

    // Run the remainder of the application while identity remains alive.
    return 0;
}
```

The temporary `device` and `incarnation` objects are Bootstrap staging values. Once `SystemIdentity` has been constructed, the System-owned lifetime state is exactly its 20 bytes.

The consumer retains a pointer rather than another copy of the 16-byte device identifier.

### Hard-coded equivalent

The hard-coded lifecycle would differ only in provider selection:

```cpp
PlatformDeviceIdentity deviceProvider;
PersistentRuntimeIncarnationIdentity incarnationProvider;
```

The identity value types and zero-copy consumer behaviour are independent of Composition. Composition exists to keep provider selection from becoming a platform-specific dependency throughout reusable code.

## Runtime Incarnation Durability Contract

The durability rule is normative.

For one permanent device identity, suppose durable storage currently contains:

```text
41
```

A new runtime must not expose incarnation `42` merely because it calculated that value in RAM.

The required sequence is:

```text
read 41
    ↓
calculate 42
    ↓
durably commit 42
    ↓
confirm commit success
    ↓
return RuntimeIncarnationId(42)
```

If power is lost after the durable commit but before `SystemIdentity` is constructed, the next boot will observe `42` and advance to `43`. Incarnation `42` may be unused, but it is never reused. That is safe.

The unsafe order would be:

```text
calculate 42
    ↓
return/expose 42
    ↓
try to persist 42 later
```

Power loss between those steps could cause a later runtime to reuse an incarnation that had already become externally observable.

For that reason, a successful `RuntimeIncarnationIdentity` provider operation means **durability has already been established**.

The exact storage transaction, fault-recovery strategy, wear policy, and platform implementation do not belong in `EDP-System`. Those concerns belong to the concrete provider and its owning library.

## What Does Not Belong in System Identity

The System Identity subdomain must remain universal.

The following concepts are intentionally excluded:

### Transport and Radio addressing

A transport address answers where or how something can currently be reached. It can change without changing device identity.

Examples include MAC addresses used directly as link addresses, IP addresses, radio addresses, connection handles, socket endpoints, and transport-specific peer handles.

A platform may use immutable hardware information as input to derive a permanent `DeviceIdentifier`, but the resulting System identity remains transport-independent.

### Cryptographic credentials

A private key, public key, certificate, or security-session identifier can prove or protect identity, but it is not the `DeviceIdentifier` itself.

Credentials may rotate while the device remains the same device.

### Mesh-specific identity

Any future Mesh membership, routing, alias, topology, group, or participation identity belongs to the future Mesh domain if that redesign demonstrates a need for it.

System contains no Mesh-specific semantics.

### OTA-specific identity

Manifest, release, transaction, artifact, component, generation, or other OTA-only identifiers belong to the OTA domain unless a future cross-domain requirement proves otherwise.

System does not pre-allocate semantic concepts merely because an older implementation contained them.

### Product and hardware classification

"Which individual device is this?" and "what kind of device/build is this?" are different questions.

Product type, hardware family/revision, architecture, software variant, and similar classification data are not part of the 20-byte runtime identity facade. They can be designed separately when a concrete requirement exists.

## Testing and Dummy Providers

`EDP-System` cannot depend on the future concrete Persistence or platform libraries merely to test its contracts.

The Identity test suite therefore supplies small test-only providers:

```text
TestDeviceIdentity
    -> deterministic non-zero DeviceIdentifier

TestRuntimeIncarnationIdentity
    -> simulated initialized persistent counter
```

The runtime-incarnation dummy deliberately does **not** claim to test real power-loss durability. Its responsibilities are limited to validating the System contract shape:

- establishment fails before its persistence prerequisite is initialized;
- successful establishment returns a non-zero value;
- each successful establishment advances by exactly one;
- the value treated as committed is advanced before success is returned;
- the Composition resolves the expected provider type.

Real durability testing belongs to the future concrete provider library.

The System test suite also guards:

- 16-byte `DeviceIdentifier` size;
- 4-byte `RuntimeIncarnationId` size;
- 20-byte `SystemIdentity` size;
- invalid zero semantics;
- deterministic identifier comparison;
- non-copyable/non-movable facade semantics;
- trivially destructible facade behaviour;
- stable addresses returned by repeated zero-copy access;
- immutable facade state after later provider activity;
- Composition resolution of both exclusive Identity capabilities.

## Common Mistakes

### Storing another identifier copy when a borrowed pointer is sufficient

Avoid this for a long-lived consumer that merely interrogates the value:

```cpp
class Consumer final {
private:

    // Duplicated identity state.

    /// Unnecessary retained copy of the device identity.
    ESPressio::System::Identity::DeviceIdentifier _device;

public:

    /// Copies the complete identifier into the consumer.
    explicit Consumer(
        const ESPressio::System::Identity::SystemIdentity& identity
    ) noexcept :
        _device(identity.Device()) {}
};
```

Prefer borrowing when lifetime ordering guarantees safety:

```cpp
class Consumer final {
private:

    // Borrowed identity state.

    /// Pointer to the application-lifetime System-owned identifier.
    const ESPressio::System::Identity::DeviceIdentifier* _device;

public:

    /// Borrows the identifier without copying it.
    explicit Consumer(
        const ESPressio::System::Identity::SystemIdentity& identity
    ) noexcept :
        _device(&identity.Device()) {}
};
```

### Treating DeviceIdentifier as a credential

Do not make key rotation change the permanent device identity merely because Security uses the identity while authenticating a peer.

Identity and proof-of-identity are separate concerns.

### Treating a transport address as permanent identity

A link address can disappear or change. `DeviceIdentifier` must preserve its meaning independently of the active transport.

### Publishing an incarnation before persistence succeeds

The incarnation is not valid for use merely because `previous + 1` was calculated. Durable commitment precedes successful establishment.

### Retaining the Identity provider inside SystemIdentity

Do not add provider pointers to the facade. Providers establish values during Bootstrap. `SystemIdentity` retains only the resulting 20 semantic bytes.

### Adding a Boolean initialized flag

The reserved zero values already encode Invalid/Unspecified. `SystemIdentity::IsValid()` derives validity from its two members, avoiding another retained byte and any padding consequence that byte might cause.

### Making SystemIdentity movable for convenience

Moving the facade would weaken the lifetime guarantee for borrowed pointers and references. Application Bootstrap should construct it directly in its final lifetime location.

### Starting identity consumers too early

An identity-consuming subsystem must not have a normal "identity unavailable yet" operating mode. Bootstrap establishes Identity before initializing those consumers.

## Public Vocabulary Reference

### `DeviceIdentifier`

Exact 16-byte permanent device identity. All-zero is Invalid/Unspecified.

### `RuntimeIncarnationId`

Exact 4-byte durable runtime-incarnation scalar. Zero is Invalid/Unspecified. Values advance by one per new runtime of a device and never wrap or reuse.

### `SystemIdentity`

Exact 20-byte immutable facade containing one `DeviceIdentifier` and one `RuntimeIncarnationId`. Non-copyable and non-movable so borrowed references and pointers remain address-stable for its application lifetime.

### `Identity::Domain`

Composition domain containing the contracts used to establish System Identity.

### `DeviceIdentity`

Exclusive Composition capability representing the provider that establishes the permanent device identity.

### `RuntimeIncarnationIdentity`

Exclusive Composition capability representing the provider that durably establishes the next runtime incarnation. The provider must commit the increment before reporting success; the concrete durability mechanism is intentionally outside System.

## Architectural Summary

The complete facility can be summarized as:

```text
Platform / Hardware
    provides DeviceIdentity
             |
             v
      DeviceIdentifier
          16 bytes
             |
             +------------------+
                                |
Persistence / durable facility  |
    provides RuntimeIncarnationIdentity
             |                  |
             v                  |
    RuntimeIncarnationId        |
           4 bytes              |
             |                  |
             +------------------+
                                |
                                v
                         SystemIdentity
                            20 bytes
                                |
                  immutable application lifetime
                                |
                +---------------+---------------+
                |                               |
          const reference                 const pointer
          transient access                retained borrowing
                |                               |
                +---------------+---------------+
                                |
                         no identifier copy
```

The central rule is:

> **System defines what identity means. Composition selects who establishes it. Bootstrap establishes it once. Consumers borrow it without copying whenever ownership is unnecessary.**
