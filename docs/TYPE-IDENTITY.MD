# Universal Type Identity

## Purpose

EDP-System owns the universal identity domain used to name semantic Types across the ESPressio Development Platform.

A Type identity answers only:

> Which semantic Type is this?

It does not encode a Primitive family, repository, schema version, transport, deployment policy, localisation name, C++ name, source path, or runtime registration.

Every allocated identity is an explicit compile-time declaration and remains stable for the lifetime of that semantic Type.

## Canonical 24/40 identity model

A complete `TypeIdentifier` is exactly eight bytes:

```text
byte 0   byte 1   byte 2   byte 3   byte 4   byte 5   byte 6   byte 7
+-----------------------+---------------------------------------------+
| Authority (24 bits)   | authority-local Type value (40 bits)       |
+-----------------------+---------------------------------------------+
```

The two components are represented by strong exact-width Types:

- `TypeAuthorityIdentifier` — exactly three bytes / 24 bits;
- `TypeLocalIdentifier` — exactly five bytes / 40 bits;
- `TypeIdentifier` — exactly eight bytes / 64 bits.

The retained representation is the canonical bytes themselves. It is not a host-endian `std::uint64_t`.

This makes the representation deterministic across architectures and preserves existing consumers which already treat an EDP Type identity as an exact eight-byte value.

## Canonical example

The authored identity:

```text
0x0123456789ABCDEF
```

has the canonical bytes:

```text
01 23 45 67 89 AB CD EF
```

and decomposes as:

```text
Authority       = 01 23 45
Authority-local = 67 89 AB CD EF
```

The integer accessors on the two strong components are explicit convenience views for diagnostics/tooling. Integer representation is not the retained identity format.

## Validity

Zero is reserved at both levels:

- Authority `00 00 00` is Invalid/Unspecified;
- authority-local Type `00 00 00 00 00` is Invalid/Unspecified.

A complete `TypeIdentifier` is valid only when both components are valid and non-zero.

This means an eight-byte value containing a valid Authority but a zero local component is still invalid, as is the inverse.

Invalid values remain observable. Constructors do not silently repair, renumber, hash, or allocate identities.

## Governance

The identity namespace follows these rules:

- Authority identifiers are globally governed.
- Authority zero is permanently reserved.
- Authority values are permanent and never reused.
- Each authority explicitly allocates its own 40-bit local Type values.
- Local zero is permanently reserved inside every Authority.
- Allocated local values are permanent and never reused.
- Identity is not automatically allocated.
- Identity is not assigned at runtime.
- Identity is not derived from names, hashes, paths, Primitive family, repository, localisation symbols, or deployment metadata.

An Authority is a namespace owner, not a repository identifier. One Authority may own Types across multiple repositories or applications.

## Declaring an identified Type

The semantic C++ Type is the authoritative declaration site:

```cpp
struct TemperatureReading final {

    static constexpr ESPressio::System::TypeIdentifier Identifier{
        ESPressio::System::TypeIdentifier::Storage{
            0x01U,
            0x23U,
            0x45U,
            0x67U,
            0x89U,
            0xABU,
            0xCDU,
            0xEFU
        }
    };

};
```

Alternatively, a declaration can be composed from the two strong components:

```cpp
static constexpr ESPressio::System::TypeAuthorityIdentifier Authority{
    ESPressio::System::TypeAuthorityIdentifier::Storage{
        0x01U,
        0x23U,
        0x45U
    }
};

static constexpr ESPressio::System::TypeLocalIdentifier LocalIdentifier{
    ESPressio::System::TypeLocalIdentifier::Storage{
        0x67U,
        0x89U,
        0xABU,
        0xCDU,
        0xEFU
    }
};

struct TemperatureReading final {

    static constexpr ESPressio::System::TypeIdentifier Identifier{
        Authority,
        LocalIdentifier
    };

};
```

The complete retained representation is identical in both forms.

## Universal concept and reader

`IdentifiedType<T>` is the universal predicate for semantic Types carrying direct EDP identity metadata.

It requires:

1. a directly readable static `T::Identifier` declaration;
2. the exact strong `System::TypeIdentifier` Type, not a merely convertible value;
3. a constant-expression declaration;
4. a valid non-zero Authority and authority-local component.

The canonical reader is:

```cpp
constexpr auto identifier =
    ESPressio::System::TypeIdentifierOf<TemperatureReading>;
```

The concept and reader are consumers of metadata declared by the semantic Type. They are not an alternate identity registry or trait-specialisation authority.

Where a foreign/unmodifiable C++ Type cannot carry the required declaration, the intended integration model is an EDP-owned semantic wrapper rather than external identity branding.

## Ordering and equality

Equality compares the exact eight canonical bytes.

Ordering is deterministic lexicographic byte ordering across those same bytes.

This deliberately matches byte-oriented schema/tooling representations and avoids host-endian numeric ordering ambiguity.

## Localisation compatibility

EDP-Localisation already publishes an exact eight-byte Type-identity contract.

In particular:

```text
0x0123456789ABCDEF
    -> 01 23 45 67 89 AB CD EF
```

The System-owned `TypeIdentifier` is designed to preserve that byte contract exactly when Localisation migrates to the universal System Type.

EDP-System itself does not depend on Localisation.

## Resource and runtime characteristics

All three identity Types are fixed-storage value Types:

- no heap allocation;
- no runtime registration;
- no global mutable registry;
- no provider lookup;
- no virtual dispatch;
- no hidden ownership;
- no platform SDK dependency.

`IdentifiedType<T>` and `TypeIdentifierOf<T>` are compile-time facilities.

The subsystem therefore adds no runtime capacity or dynamic-memory dimension to application architecture.

## Concurrency and ISR scope

The value Types contain only immutable-by-convention value state manipulated through ordinary value operations. They do not provide synchronization and make no library-wide ISR-safety claim.

The Type identity facility performs no blocking, allocation, provider access, or time operation.

## Deliberate non-goals

Universal Type identity does not define:

- reflection;
- Field identifiers;
- schema ownership;
- serialisation;
- transmissibility;
- Primitive families;
- transport addressing;
- runtime discovery;
- version negotiation.

Those concerns remain owned by their respective domains.
