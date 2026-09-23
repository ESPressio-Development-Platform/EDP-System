# Resources, Lifecycle and Concurrency

EDP-System is primarily compile-time and value-type infrastructure, so its resource model is intentionally small.

Composition declarations should compile away after validation and should not create runtime registries or ownership structures. `FlagSet` uses one fixed-width backing integer. Identity values use exact fixed-width storage.

System identity has lifecycle semantics even though it is small: a runtime incarnation must be durable before being published, and the resulting `SystemIdentity` is immutable for that runtime.

No library-wide ISR-safety or concurrency guarantee should be inferred merely from being in EDP-System. Individual value types are ordinary value objects unless their specific contract states otherwise.
