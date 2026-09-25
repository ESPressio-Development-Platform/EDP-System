# EDP-System Developer Wiki

EDP-System is the foundational, platform-neutral library for ESPressio system facilities. It owns the compile-time Composition Framework, typed flag infrastructure, provider-reference helpers, qualified provider resolution, universal Type identity, universal Type-local Field schema metadata, and system identity primitives.

This Wiki is maintained beside the code on the same branch it documents. Source code and repository `docs/` remain the normative implementation and durable contract sources; the Wiki is the developer navigation and explanation layer.

## Scope

EDP-System has no mandatory EDP dependency below it. Higher-level EDP libraries depend on its foundational value Types and compile-time architecture vocabulary rather than on runtime dependency injection.

## Primary public entry point

```cpp
#include <ESPressio_System.hpp>
```

## Developer map

Use [Architecture](Architecture.md) for subsystem relationships, [Public API](Public-API.md) for consumer-facing vocabulary, [Internal API](Internal-API.md) and [Implementation](Implementation.md) for maintainer-facing machinery, [Composition](Composition.md) for compile-time contracts, [Universal Type Identity](Type-Identity.md) for the 24/40 Type namespace, [Universal Field Schema](Field-Schema.md) for fixed Field identity and concrete-member bindings, [Resources / Lifecycle / Concurrency](Resources-Lifecycle-Concurrency.md) for behavioural constraints, and [Build / Test / Source](Build-Test-Source.md) for repository navigation and validation.
