# Architecture

EDP-System provides compile-time architecture description and foundational value Types. It deliberately does not own runtime provider lookup, object lifetime, service location, or higher-domain behaviour.

## Major subsystems

### Composition Framework

The Composition Framework models Domains, Offers, Requirements, Contracts, Compositions, Architectures, selection policies, provider relationships, and lifecycle-order constraints entirely at compile time. Bootstrap code still owns concrete provider objects.

### Provider references and qualified resolution

Provider-reference helpers allow Bootstrap-owned provider objects to be bound non-owningly to consumers. Qualified resolution validates that a provider selected by Type also satisfies the required capability, scope, cardinality and constraints.

### FlagSet

`FlagSet` provides a strongly typed fixed-width flag value with one integer backing store and no heap, registry or vtable requirement.

### Universal Type Identity

Universal Type identity provides a platform-wide stable namespace for semantic Types. The complete identity is exactly eight canonical bytes partitioned into a three-byte globally governed Authority and five-byte authority-local value. The semantic Type declares its own identity; compile-time concepts/readers validate and consume that declaration without a registry.

### System Identity

System identity separates durable device identity from runtime incarnation identity. `DeviceIdentifier` is 16 bytes, `RuntimeIncarnationId` is 4 bytes, and `SystemIdentity` combines them into a stable 20-byte runtime identity.

## Architectural boundaries

Composition is descriptive and validating, not a runtime DI container. Runtime objects remain explicit. Universal Type identity is semantic metadata, not runtime registration. Platform-specific SDKs do not belong in this library.
