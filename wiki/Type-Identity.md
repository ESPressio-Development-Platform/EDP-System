# Universal Type Identity

EDP-System owns the universal Type identity namespace used by ESPressio libraries, applications and compatible third-party libraries.

The durable normative contract is [docs/TYPE-IDENTITY.MD](../docs/TYPE-IDENTITY.MD). This Wiki page adds maintainer context and source navigation.

## Ownership

Universal Type identity belongs to EDP-System because it is not Primitive-specific, Localisation-specific, or Serialisation-specific.

The facility deliberately owns only semantic identity:

- `TypeAuthorityIdentifier` — globally governed 24-bit Authority;
- `TypeLocalIdentifier` — authority-local 40-bit Type value;
- `TypeIdentifier` — complete canonical 64-bit identity;
- `IdentifiedType<T>` — compile-time predicate;
- `TypeIdentifierOf<T>` — canonical compile-time reader.

It does not own general reflection, Field identity, Primitive family, transport exposure, Serialisation, or Localisation presentation.

## Canonical representation

```text
bytes 0..2 : TypeAuthorityIdentifier
bytes 3..7 : TypeLocalIdentifier
```

All three public value Types retain their canonical bytes directly.

For authored identity `0x0123456789ABCDEF`:

```text
Authority       01 23 45
Local           67 89 AB CD EF
Complete        01 23 45 67 89 AB CD EF
```

The byte representation is authoritative. Integer `Value()` accessors on the components are explicit tooling/diagnostic views only.

## Invariants

- exact retained sizes are 3, 5 and 8 bytes;
- zero Authority is invalid;
- zero local value is invalid;
- a complete TypeIdentifier is valid only when both components are valid;
- identity assignment is explicit and compile-time;
- allocated values are permanent and never reused;
- identity is not hash/name/path/family derived;
- equality and ordering operate on canonical bytes;
- no runtime registry exists.

## Semantic-Type authority

A semantic Type declares:

```cpp
static constexpr ESPressio::System::TypeIdentifier Identifier = /* explicit */;
```

That declaration is the identity authority for the Type.

The concept/reader layer validates and consumes that declaration. It never provides an external specialisation path that can brand an otherwise unidentified Type.

## Resource behaviour

All value Types are fixed-storage and allocation-free. The concept/reader machinery is compile-time only. No mutex, heap, registry, provider object or SDK facility is involved.

## Source references

- [TypeAuthorityIdentifier](Reference-types-TypeAuthorityIdentifier)
- [TypeLocalIdentifier](Reference-types-TypeLocalIdentifier)
- [TypeIdentifier](Reference-types-TypeIdentifier)
- [IdentifiedType](Reference-types-IdentifiedType)
- [Types aggregation header](Reference-types-Types)
