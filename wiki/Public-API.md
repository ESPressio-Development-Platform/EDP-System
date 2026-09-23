# Public API

Public API is the supported consumer-facing surface reachable through `ESPressio_System.hpp`.

## Composition vocabulary

Key public types include `Domain`, `ExclusiveCapability`, `SharedCapability`, `Offer`, `Offers`, `Requirement`, `Contract`, `Provider`, `Composition`, `Architecture`, provider-count cardinality constraints, provider-selection policies, same/distinct-provider constraints, and lifecycle ordering declarations such as initialization/shutdown relationships.

A `Requirement` describes one capability requirement including scope, cardinality and qualification constraints. A `Contract` groups the complete requirements of one provider or standalone consumer. A `Composition` validates one Domain's provider set. An `Architecture` validates cross-domain relationships.

## Provider references

Provider-reference types bind to Bootstrap-owned provider instances without taking ownership. They exist to preserve explicit object lifetime while avoiding runtime lookup.

## FlagSet

`FlagSet` is a bounded typed flag container. Its contract is value-like, fixed-storage and allocation-free.

## Identity

`DeviceIdentifier`, `RuntimeIncarnationId` and `SystemIdentity` form the public identity model. Invalid all-zero identifiers are rejected by the identity contract. `SystemIdentity` is immutable and intentionally non-copyable/non-movable once constructed.

For exact signatures, template parameters and members, treat the headers linked from `ESPressio_System.hpp` as authoritative; this Wiki explains their intent and relationships.
