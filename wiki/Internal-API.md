# Internal API

Internal API covers reusable implementation machinery that supports the public contracts but is not itself a compatibility promise to ordinary consumers.

The Composition implementation contains compile-time matching, flattening, filtering, provider counting, constraint evaluation, selection and architectural validation helpers. These helpers exist to make public constructs such as `Requirement`, `Composition::Select` and `Architecture::IsValid` deterministic compile-time facts.

Identity internals own validation and representation rules for device and runtime-incarnation identifiers. FlagSet internals own representability checks and typed conversion to the selected fixed-width backing value.

Maintainers should preserve the distinction between public declarative vocabulary and internal metaprogramming machinery. Internal helpers may evolve, but public diagnostics and contract semantics must not silently weaken.


## Field schema internals

The Field schema implementation uses member-pointer trait extraction plus compile-time identity/member tokens to validate ownership and uniqueness. These helpers are private implementation machinery even though downstream libraries consume the public FieldIdentifier/FieldBinding/FieldSet/SchemaType surface. They must not become an alternate specialization-based schema registry.
