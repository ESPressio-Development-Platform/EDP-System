# Build, Test and Source Map

EDP-System targets C++20.

## Source map

`src/` contains the public umbrella and subsystem headers for Composition, identity, flag handling and provider-reference support. `docs/` contains durable design contracts. `tests/` contains host/compile-time validation. `demos/` demonstrates supported usage patterns.

## Validation focus

Tests should protect compile-time requirement matching, provider cardinality, selection, architecture validity, flag representability, identity width/validity and failure diagnostics.

Because EDP-System is foundational, changes should be validated against downstream repositories that consume its Composition vocabulary, not only against local tests.
