# Universal Type Identity Demo

This demo shows the common System-owned Type identity contract:

- an exact three-byte `TypeAuthorityIdentifier`;
- an exact five-byte `TypeLocalIdentifier`;
- an exact eight-byte canonical `TypeIdentifier`;
- direct semantic-Type declaration through `T::Identifier`;
- compile-time validation through `IdentifiedType<T>`;
- canonical access through `TypeIdentifierOf<T>`;
- preservation of the canonical bytes `01 23 45 67 89 AB CD EF`.

The demo is provided for Arduino IDE, PIOArduino/PlatformIO Arduino, and PIOArduino/PlatformIO ESP-IDF.
