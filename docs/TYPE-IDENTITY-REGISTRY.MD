# Permanent TypeAuthority and TypeIdentifier Registry

**Namespace owner:** ESPressio Development Platform (EDP Core).
**Issuing authority:** the authorised EDP platform owner; approval recorded on **2026-10-17**.
**Authoritative repository:** `EDP-System` on `main`.
**Wire representation:** exactly eight canonical bytes, **24-bit Authority / 40-bit local Type**, in order, never a host-endian integer.
**Lifecycle:** immutable allocation; deprecated/retired Type identities remain permanently reserved and may never be reused.

This file is a **development-time allocation ledger**, not a runtime registry, allocator, dependency, serialization schema or name-derived identifier. Source Type declarations hold compile-time identity. The EDP-System `IdentifiedType` concept verifies their shape. Explicit allocation and collision checks across all governed types are still required.

## Authority ownership

| Authority (3 bytes) | Owner | Status | Provenance |
| --- | --- | --- | --- |
| `00 00 00` | None | Permanently invalid/reserved | [Type identity rules](TYPE-IDENTITY.MD) |
| **`00 00 01`** | **ESPressio Development Platform — Core** | **Permanently assigned** | Owner approval, 2026-10-17. Pre-existing use by EDP-Command/Event/State Primitive family identities is consistent with this ownership. |

An Authority is *not* a repository or Primitive-family discriminator. Command, Event, State and Logging may allocate semantic Types under the same owner only through this ledger. A PrimitiveFamilyIdentifier is a separate identifier; the family numbers 1/2/3 do **not** allocate 40-bit TypeLocal identifiers.

## Permanent EDP Core semantic Type allocations

| Canonical 8-byte TypeIdentifier | Authority | 40-bit TypeLocalIdentifier | Semantic Type | Status / provenance |
| --- | --- | --- | --- | --- |
| **`00 00 01 00 00 00 00 01`** | `00 00 01` | **`00 00 00 00 01`** | **`ESPressio::Logging::LogRecord`** | **Issued 2026-10-17 by platform owner approval; permanent, never reusable.** Schema implementation status: identity declared, fields 0–14 approved but full typed field binding/encoding still pending; see EDP-System#11 and EDP-Logging#1. |
| `00 00 01 01 00 00 00 01` | `00 00 01` | `01 00 00 00 01` | `ESPressio::Command::NoRequestPayload` | **Pre-existing stable production source allocation**, inventoried from `EDP-Command/src/command/CommandTypes.hpp`; provenance formalisation predates this ledger. Never reuse. |
| `00 00 01 01 00 00 00 02` | `00 00 01` | `01 00 00 00 02` | `ESPressio::Command::NoResponsePayload` | **Pre-existing stable production source allocation**, inventoried from `EDP-Command/src/command/CommandTypes.hpp`; provenance formalisation predates this ledger. Never reuse. |

**Do not add any other allocation without the EDP Core authority owner's explicit approval.** The table inventories known EDP-owned production declarations; not every test, demo, or application-generated Type in the ecosystem has been audited for past identity reuse. The approved LogRecord candidate was checked against EDP source declarations and identified collisions with **unregistered test/demo fixtures**, which must be migrated away from the production value before general test publishing.

## Allocation rules and acceptance checklist

1. Select the existing legitimate Authority owner, or receive an explicit issuance for a new globally governed Authority. Zero at either width is always invalid.
2. Search authoritative registered Type entries **and existing production source declarations** (including downstream generated application code) for duplicate exact eight-byte identities. Previously issued or retired values are unavailable.
3. Obtain and record the Authority owner's explicit approval and semantic Type association. A developer or automated agent does **not** mint production IDs by hashing names, incrementing a counter, assuming a test/example ID is safe, or interpreting a repository name as an Authority.
4. Record full exact canonical bytes, both components, ownership, source declaration, date, approval provenance and active/retired status here. Review changes in the EDP-System owner repository.
5. Declare the same eight immutable bytes directly on the semantic C++ Type as `inline static constexpr System::TypeIdentifier Identifier`. Assert `System::IdentifiedType<T>` and `System::TypeIdentifierOf<T>` and golden-test bytes in the consuming library. **A valid identity alone is not a completed `System::SchemaType`**: field bindings and schema quality must be validated separately.
6. Historical payloads retain the original identity even if the schema evolves; never assign that value to a different semantic Type. Do not automatically assign a new TypeIdentifier for a schema version of the **same** semantic Type.

## Test fixtures and previous usages

The exact LogRecord value was present before issuance in fixture/demo Types in EDP-Serialisation and EDP-Primitives (including two variants of the topology demo). Those are **not authoritative, governed production allocations**. As part of the 2026-10-17 issuance, their fixture values were relocated into distinct **test/demo-only** unregistered values:

| Former fixture owner | Test/demo-only value | Status |
| --- | --- | --- |
| Serialisation host `Nested` | `00 00 01 FE 00 02 00 01` | Fixture; must not be published as production identity |
| Primitives test `PrimitiveA` | `00 00 01 FE 00 00 00 01` | Fixture; must not be published as production identity |
| Primitives topology demos `StartSignal` | `00 00 01 FE 00 01 00 01` | Fixture; must not be published as production identity |

The fixture-only values above are **not newly governed permanent allocations**. A demonstration intended for interoperable or production deployment must obtain a properly issued identity. Other extant fixture identities under Authority 1 are not covered by this limited correction; avoid treating fixture literals as evidence that the corresponding Type is globally registered.

## Implementation boundary

`ESPressio::Logging::LogRecord` is initially **identity-only and non-instantiable** so callers cannot confuse a placeholder with a fully implemented record. Its approved field identifiers 0–14 and semantics remain controlled by the Logging living design handoff. Completion of its actual `System::SchemaType` and serialization comes after binding the approved fields, generated historical schema, and validation. This ledger is not a workaround for those gates.
