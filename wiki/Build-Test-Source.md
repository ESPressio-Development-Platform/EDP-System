# Build, Test and Source Map

EDP-System targets C++20.

## Source map

`src/` contains the public umbrella and subsystem headers for Composition, universal Type identity, System Identity, flag handling and provider-reference support. `docs/` contains durable design contracts. `tests/` contains host/compile-time validation. `demos/` demonstrates supported usage patterns.

Universal Type identity and the universal Field schema reside under `src/types/`.

## Validation focus

Tests protect compile-time requirement matching, provider cardinality, selection, architecture validity, flag representability, System Identity width/validity, universal Type identity exact widths/canonical bytes/component validity, and failure diagnostics.

The Type identity suite includes positive host tests plus intentional compile failures for missing `T::Identifier`, wrong Identifier semantic Type, non-constant Identifier declarations, and reserved invalid identifiers. Field schema coverage validates exact one-byte identity, the complete 0..255 range, explicit zero-Field schemas, concrete member access, metadata readers, and generic traversal; compile-fail cases reject default FieldIdentifier construction, values above 255, non-member bindings, duplicate identities, duplicate members, mixed owners, and SchemaType owner mismatches. Positive regression coverage also proves that identified Types with missing or non-FieldSet `Fields` metadata make `SchemaType<T>` evaluate safely to `false`.

The `type-identity-basic` and `field-schema-basic` demos each have Arduino IDE, PIOArduino Arduino and PIOArduino ESP-IDF variants.

Because EDP-System is foundational, changes should also be validated against downstream repositories that consume its Composition or universal Type identity vocabulary, not only against local tests.
