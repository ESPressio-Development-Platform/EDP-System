# Universal Field Schema

The universal Field schema is the common EDP contract for identifying and enumerating concrete Fields of a semantic Type.

## Identity model

A Field identity is complete only with its owner:

```text
(TypeIdentifier, FieldIdentifier)
```

`FieldIdentifier` is exactly one byte and every value 0..255 is valid within one Type. Zero is not reserved.

## Concrete member binding

`FieldBinding<&T::Member, id>` binds the numeric Field identity directly to the actual non-static data member. The binding exposes owner Type, value Type, member pointer and strong FieldIdentifier without runtime state.

## Canonical schema

One schema-bearing semantic Type declares exactly one nested `Fields` alias using `FieldSet<...>`. The set rejects duplicate numeric identities, duplicate member bindings and mixed owners.

`SchemaType<T>` combines `IdentifiedType<T>` with that canonical FieldSet. Explicit zero-field schema Types use `FieldSet<>`.

## Consumer surface

- `FieldsOf<T>`
- `FieldIdentifierOf<TField>`
- `FieldOwnerOf<TField>`
- `FieldValueOf<TField>`
- `ForEachField<T>(callable)`

These facilities permit Localisation, future Serialisation, Command, diagnostics and UI tooling to consume one authoritative mapping without a runtime registry.

## Ownership boundaries

EDP-System owns current Type/Field identity and C++ member association. It does not own human presentation, serialization encoding, Command execution, transport deployment, schema lifecycle history, or retired-identity catalogues.

See the durable [Universal Field Schema](../docs/FIELD-SCHEMA.MD) document for the normative contract.
