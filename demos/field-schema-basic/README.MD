# Universal Field Schema Demo

This demo shows the System-owned Field schema contract:

- exact one-byte `FieldIdentifier` values;
- direct concrete-member association through `FieldBinding`;
- canonical per-Type `FieldSet`;
- compile-time validation through `SchemaType<T>`;
- canonical readers through `FieldsOf` and `FieldIdentifierOf`;
- generic allocation-free traversal through `ForEachField`.

The demo is provided for Arduino IDE, PIOArduino/PlatformIO Arduino, and PIOArduino/PlatformIO ESP-IDF.
