# Dependency Contracts

EDP-System is the root of the current EDP dependency graph and has **no EDP library dependency**.

## Contract supplied to downstream libraries

EDP-System supplies the compile-time Composition vocabulary used by other repositories: `Domain`, capabilities, `Offer`/`Offers`, properties/attributes, `Requirement`, `Contract`, `Provider`, `Composition`, `Architecture`, provider selection and `ProviderReferences`.

It also supplies the universal Type identity vocabulary used by higher domains: exact-width Authority/local/complete identifiers plus the direct semantic-Type identification concept and reader.

The critical runtime rule is that Composition describes and validates architecture but does not own provider objects. Bootstrap owns concrete instances and their lifetimes.

## Universal Type identity dependency boundary

The Type identity subsystem depends only on the C++20 standard library. It does not depend on Localisation, Serialisation, Primitives, Memory, Platform, or a runtime registry.

Higher repositories may consume `System::TypeIdentifier` without causing a reverse dependency into System.

## System Identity dependency boundary

System identity is also foundational. `DeviceIdentifier`, `RuntimeIncarnationId` and `SystemIdentity` do not depend on a higher EDP service. A concrete durable-incarnation mechanism is supplied by the application/platform layer and must commit an incarnation before it is published.

## Downstream compatibility implication

Changes to universal Type identity bytes, component partitioning, validity rules, or declaration/readers are cross-repository contract changes. Likewise, changes to Composition matching, cardinality, property/attribute comparison, provider selection, provider references or lifecycle-order validation are cross-repository contract changes even when no runtime ABI changes.

> Universal Type identity dependency audit baseline: `cb81bb1b9ecb3a28330ad753a6c2d91c571c5961`.
