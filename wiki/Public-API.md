# Public API

Public API is the supported consumer-facing surface reachable through `ESPressio_System.hpp`.

## Composition vocabulary

Key public Types include `Domain`, `ExclusiveCapability`, `SharedCapability`, `Offer`, `Offers`, `Requirement`, `Contract`, `Provider`, `Composition`, `Architecture`, provider-count cardinality constraints, provider-selection policies, same/distinct-provider constraints, and lifecycle ordering declarations such as initialization/shutdown relationships.

A `Requirement` describes one capability requirement including scope, cardinality and qualification constraints. A `Contract` groups the complete requirements of one provider or standalone consumer. A `Composition` validates one Domain's provider set. An `Architecture` validates cross-domain relationships.

## Provider references

Provider-reference Types bind to Bootstrap-owned provider instances without taking ownership. They exist to preserve explicit object lifetime while avoiding runtime lookup.

## FlagSet

`FlagSet` is a bounded typed flag container. Its contract is value-like, fixed-storage and allocation-free.

## Universal Type identity

`TypeAuthorityIdentifier`, `TypeLocalIdentifier` and `TypeIdentifier` are strong exact-width identity values occupying 3, 5 and 8 bytes respectively.

`IdentifiedType<T>` validates that a semantic Type directly declares an exact, constant-expression, valid `System::TypeIdentifier`. `TypeIdentifierOf<T>` is the canonical compile-time reader and emits focused diagnostics when that declaration is malformed.

See [Universal Type Identity](Type-Identity.md) and the corresponding source reference pages for exact semantics.

## Universal Field schema

`FieldIdentifier` is the strong exact one-byte identity of one Field within an owning Type. Every value 0..255 is valid and zero is not a sentinel.

`FieldBinding<&T::Member, id>` is the authoritative compile-time relationship between a concrete non-static data member and its numeric identity. `FieldSet<...>` is the canonical enumerable schema for one owner. `SchemaType<T>` combines universal Type identity with a valid canonical FieldSet.

`FieldsOf<T>`, `FieldIdentifierOf<TField>`, `FieldOwnerOf<TField>`, `FieldValueOf<TField>`, and `ForEachField<T>(callable)` are the common compile-time consumer vocabulary.

See [Universal Field Schema](Field-Schema.md) and the corresponding source reference pages.

## System Identity

`DeviceIdentifier`, `RuntimeIncarnationId` and `SystemIdentity` form the public runtime identity model. Invalid all-zero identifiers are rejected by the identity contract. `SystemIdentity` is immutable and intentionally non-copyable/non-movable once constructed.

For exact signatures, template parameters and members, treat the headers linked from `ESPressio_System.hpp` as authoritative; this Wiki explains their intent and relationships.
