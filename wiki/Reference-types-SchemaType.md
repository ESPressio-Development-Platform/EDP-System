# src/types/SchemaType.hpp

**Primary classification:** PUBLIC API / PRIVATE IMPLEMENTATION

**Source baseline:** `b04ff50c765762842fcbf48ee6c24a7072f5b6d7`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/b04ff50c765762842fcbf48ee6c24a7072f5b6d7/src/types/SchemaType.hpp)

## `SchemaType<TType>`

**Classification:** PUBLIC API

Predicate identifying one `IdentifiedType` which declares a valid canonical nested `Fields` FieldSet owned by that exact Type. Explicit empty FieldSets are valid. The predicate is deliberately safe for arbitrary candidate Types: missing or malformed `Fields` metadata evaluates to `false` rather than making the concept itself ill-formed.

## Readers

### `FieldsOf<TType>`

Returns the canonical nested FieldSet with focused contract diagnostics.

### `FieldIdentifierOf<TField>`

Returns the exact strong FieldIdentifier declared by one FieldBinding.

### `FieldOwnerOf<TField>`

Returns the binding's owning semantic Type.

### `FieldValueOf<TField>`

Returns the represented member value Type.

### `ForEachField<TType>(callable)`

Enumerates the schema's FieldBindings through the canonical FieldSet.

## Detail helpers

**Classification:** PRIVATE IMPLEMENTATION

`IsFieldSet`, `HasFieldsMember`, `IsSchemaType`, `ReadFields`, and `ReadFieldBinding` validate/read direct schema declarations. They are not a specialization-based schema authority.
