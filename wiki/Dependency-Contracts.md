# Dependency Contracts

EDP-System is the root of the current EDP dependency graph and has **no EDP library dependency**.

## Contract supplied to downstream libraries

EDP-System supplies the compile-time Composition vocabulary used by other repositories: `Domain`, capabilities, `Offer`/`Offers`, properties/attributes, `Requirement`, `Contract`, `Provider`, `Composition`, `Architecture`, provider selection and `ProviderReferences`.

The critical runtime rule is that Composition describes and validates architecture but does not own provider objects. Bootstrap owns concrete instances and their lifetimes.

## Identity dependency boundary

System identity is also foundational. `DeviceIdentifier`, `RuntimeIncarnationId` and `SystemIdentity` do not depend on a higher EDP service. A concrete durable-incarnation mechanism is supplied by the application/platform layer and must commit an incarnation before it is published.

## Downstream compatibility implication

Changes to Composition matching, cardinality, property/attribute comparison, provider selection, provider references or lifecycle-order validation are cross-repository contract changes even when no runtime ABI changes.

> Dependency contract audit baseline: `37d2195583d1fc27b6e229cafa6fdeb138951bcb` (`main`).
