# src/composition/ProviderReferences.hpp

**Primary classification:** PUBLIC COMPOSITION API

**Source baseline:** `9964ab2820cc5d290de69674000e011faaa684d9`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/9964ab2820cc5d290de69674000e011faaa684d9/src/composition/ProviderReferences.hpp)

## Direct includes

- `cstddef`
- `tuple`
- `type_traits`
- `Composition.hpp`

## Documented declarations

### `TProviderList`

**Classification:** PUBLIC COMPOSITION API

Declares non-owning runtime bindings for the provider types represented by one compile-time ProviderList.

- **Template parameter `TProviderList`:** ProviderList whose provider instances will be referenced.

```cpp
template<class TProviderList>
    class ProviderReferences;
```

### `ProviderReferences`

**Classification:** PUBLIC COMPOSITION API

Binds Bootstrap-owned provider instances to one compile-time ProviderList without taking ownership.

The provider references are stored in exactly the order represented by the ProviderList. The collection
performs no provider construction, heap allocation, runtime capability matching, or type erasure.

- **Template parameter `TProviders`:** Provider types represented by the supplied ProviderList.

```cpp
template<class... TProviders>
    class ProviderReferences<ProviderList<TProviders...>> final
```

### `_providers`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Non-owning pointers to the Bootstrap-owned provider instances represented by this collection.

```cpp
std::tuple<TProviders*...> _providers;
```

### `ProviderTypes`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Compile-time provider list represented by this collection.

```cpp
using ProviderTypes = ProviderList<TProviders...>;
```

### `Count`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Number of provider instances referenced by this collection.

```cpp
static constexpr std::size_t Count = sizeof...(TProviders);
```

### `ProviderReferences`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Binds the provider instances corresponding exactly to the represented ProviderList.

The caller retains ownership of every supplied provider instance. Every instance must therefore
outlive this ProviderReferences object and every copy made from it.

- **Parameter `providers`:** Provider instances in the same order as their types appear in ProviderTypes.

```cpp
constexpr explicit ProviderReferences(
                TProviders&... providers
            ) noexcept :
                _providers(
                    &providers...
                ) {}
```

### `TCallable`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Invokes one callable once for every mutable provider instance represented by this collection.

Traversal follows ProviderTypes order. The callable must accept every represented provider type.
Return values from the callable are deliberately ignored; result aggregation remains the caller's
responsibility because provider operations may have domain-specific result semantics.

- **Template parameter `TCallable`:** Callable type invoked for every provider reference.
- **Parameter `callable`:** Callable to invoke for every provider.

```cpp
template<class TCallable>
            constexpr void ForEach(TCallable&& callable)
```

### `TCallable`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Invokes one callable once for every provider instance through a const view.

Traversal follows ProviderTypes order. Each provider is exposed as const, so the callable must
accept every represented provider type through a const-compatible reference.

- **Template parameter `TCallable`:** Callable type invoked for every const provider reference.
- **Parameter `callable`:** Callable to invoke for every provider.

```cpp
template<class TCallable>
            constexpr void ForEach(TCallable&& callable) const
```

