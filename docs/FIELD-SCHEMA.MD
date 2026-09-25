# Universal Field Schema

## Purpose

EDP-System owns the platform-wide compile-time identity relationship between an identified semantic Type and its concrete C++ data members.

A Field identity answers:

> Which Field of this semantic Type is this?

The complete identity is therefore the pair:

```text
(System::TypeIdentifier, System::FieldIdentifier)
```

Field schema metadata is intentionally independent of Localisation, Serialisation, Command, transport, UI, and runtime registration. Higher domains consume the common schema rather than inventing parallel Field identity systems.

## FieldIdentifier

`System::FieldIdentifier` is an exact one-byte strong value Type. Every numeric value from `0` through `255` is valid within one owning Type. Zero is therefore not an Invalid/Unspecified sentinel.

Because default construction would silently manufacture Field zero, `FieldIdentifier` is not default-constructible. Construction is explicit from its exact `std::uint8_t` storage.

```cpp
constexpr ESPressio::System::FieldIdentifier TemperatureField{0U};
constexpr ESPressio::System::FieldIdentifier RecordedAtField{1U};
```

There is no implicit integer conversion and no configurable FieldIdentifier width.

## FieldBinding

`System::FieldBinding` binds one concrete non-static C++ data member to one stable Type-local FieldIdentifier at compile time.

```cpp
using TemperatureField = ESPressio::System::FieldBinding<
    &TemperatureReading::Temperature,
    0U
>;
```

Each binding exposes `Owner`, `Value`, `Member`, and `Identifier`. A binding contains no runtime object or registration state.

## FieldSet

`System::FieldSet<TFields...>` is the canonical compile-time enumerable Field schema for one semantic Type.

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

    float Temperature = 0.0F;
    std::uint64_t RecordedAt = 0U;

    using Fields = ESPressio::System::FieldSet<
        ESPressio::System::FieldBinding<
            &TemperatureReading::Temperature,
            0U
        >,
        ESPressio::System::FieldBinding<
            &TemperatureReading::RecordedAt,
            1U
        >
    >;

};
```

The declaration directly establishes:

```text
TemperatureReading / Field 0 -> TemperatureReading::Temperature
TemperatureReading / Field 1 -> TemperatureReading::RecordedAt
```

FieldSet validates at compile time that every entry is an exact `FieldBinding`, every binding belongs to the same owner, no numeric identity is duplicated, and no C++ member is bound more than once.

Declaration order is not identity. The numeric FieldIdentifier is authoritative.

An identified schema Type with no Fields can explicitly declare:

```cpp
using Fields = ESPressio::System::FieldSet<>;
```

## SchemaType and readers

`System::SchemaType<T>` identifies a semantic Type which satisfies `IdentifiedType<T>`, declares a nested `Fields` Type, uses an exact `FieldSet`, and binds every non-empty FieldSet member to that exact Type.

Common readers are:

```cpp
ESPressio::System::FieldsOf<TemperatureReading>
ESPressio::System::FieldIdentifierOf<TemperatureField>
ESPressio::System::FieldOwnerOf<TemperatureField>
ESPressio::System::FieldValueOf<TemperatureField>
```

## Generic enumeration

`System::ForEachField<T>` invokes a templated callable once for every FieldBinding in declaration order.

```cpp
ESPressio::System::ForEachField<TemperatureReading>(
    []<class TField>() {
        constexpr auto Identifier = TField::Identifier;
        static_cast<void>(Identifier);
    }
);
```

The facility performs no registry lookup and allocates no memory.

## Schema lifecycle and governance

EDP-System owns current C++ Field identity and binding semantics, not lifecycle history.

A higher schema-inventory/tooling layer may record active/retired state, presentation exposure, provenance, and non-reuse history. Once a numeric FieldIdentifier has represented one semantic Field of a Type, that identity must not later be reused for a different Field.

## Localisation relationship

EDP-Localisation consumes the exact pair `(System::TypeIdentifier, System::FieldIdentifier)` to resolve Type/Field presentation text.

EDP-System itself does not depend on Localisation and does not contain human-readable Field names/descriptions.

## Serialisation and Command relationship

Field identity does not imply serialisability or transmissibility. Future Serialisation and Command facilities may consume the common schema while retaining ownership of their own encoding, execution, deployment, and lifecycle contracts.

## Resource characteristics

The Field schema foundation introduces no heap allocation, runtime registry, RTTI, provider instance, synchronization primitive, background task, or runtime capacity dimension.

`FieldIdentifier` occupies exactly one byte. The remaining schema vocabulary is compile-time metadata.
