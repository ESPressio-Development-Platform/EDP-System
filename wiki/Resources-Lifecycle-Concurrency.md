# Resources, Lifecycle and Concurrency

EDP-System is primarily compile-time and value-Type infrastructure, so its resource model is intentionally small.

Composition declarations should compile away after validation and should not create runtime registries or ownership structures. `FlagSet` uses one fixed-width backing integer. System Identity values use exact fixed-width storage.

`FieldIdentifier` uses exactly one byte. `FieldBinding`, `FieldSet`, `SchemaType`, metadata readers, and Field enumeration introduce no per-instance storage, registry, allocation, synchronization primitive, or runtime capacity.

Universal Type identity uses exactly three bytes for `TypeAuthorityIdentifier`, five bytes for `TypeLocalIdentifier`, and eight bytes for `TypeIdentifier`. No decoded integer cache, heap allocation, registry, synchronization primitive, provider object, or runtime capacity accompanies those values. `IdentifiedType<T>` and `TypeIdentifierOf<T>` are compile-time-only facilities.

System identity has lifecycle semantics even though it is small: a runtime incarnation must be durable before being published, and the resulting `SystemIdentity` is immutable for that runtime.

No library-wide ISR-safety or concurrency guarantee should be inferred merely from being in EDP-System. Individual value Types are ordinary value objects unless their specific contract states otherwise.
