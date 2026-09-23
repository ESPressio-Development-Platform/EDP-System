# Composition

EDP-System owns the Composition Framework used throughout EDP.

A provider declares its Domain, Offers and Contract. Requirements carry a capability, scope, cardinality and optional constraints. `SameDomain`, `ExternalDomain` and `AnyDomain` express where qualifying providers may be found.

Cardinality is explicit through constructs such as exact, minimum, maximum and bounded provider counts. Selection is therefore never implicit first-match behaviour.

`Composition<Domain,...>` validates one domain. `Architecture<...>` validates flattened cross-domain contracts and lifecycle relationships. Same-provider, distinct-provider, initialization-order and shutdown-order relationships are architectural facts rather than runtime conventions.

Bootstrap remains responsible for constructing and retaining provider objects. Composition never becomes a service locator.
