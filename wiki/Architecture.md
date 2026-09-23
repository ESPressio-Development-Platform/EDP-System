# Architecture

EDP-System provides compile-time architecture description and foundational value types. It deliberately does not own runtime provider lookup, object lifetime, or service location.

## Major subsystems

### Composition Framework

The Composition Framework models Domains, Offers, Requirements, Contracts, Compositions, Architectures, selection policies, provider relationships, and lifecycle-order constraints entirely at compile time. Bootstrap code still owns concrete provider objects.

### Provider references and qualified resolution

Provider-reference helpers allow Bootstrap-owned provider objects to be bound non-owningly to consumers. Qualified resolution validates that a provider selected by type also satisfies the required capability, scope, cardinality and constraints.

### FlagSet

`FlagSet` provides a strongly typed fixed-width flag value with one integer backing store and no heap, registry or vtable requirement.

### Identity

System identity separates durable device identity from runtime incarnation identity. `DeviceIdentifier` is 16 bytes, `RuntimeIncarnationId` is 4 bytes, and `SystemIdentity` combines them into a stable 20-byte runtime identity.

## Architectural boundaries

Composition is descriptive and validating, not a runtime DI container. Runtime objects remain explicit. Platform-specific SDKs do not belong in this library.
