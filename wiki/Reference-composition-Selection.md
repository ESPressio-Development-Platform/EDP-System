# src/composition/Selection.hpp

**Primary classification:** PUBLIC COMPOSITION API

**Source baseline:** `9964ab2820cc5d290de69674000e011faaa684d9`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/9964ab2820cc5d290de69674000e011faaa684d9/src/composition/Selection.hpp)

## Documented declarations

### `SelectAll`

**Classification:** PUBLIC COMPOSITION API

Selects every provider satisfying one Requirement or provider specification.

```cpp
struct SelectAll final {};
```

### `SelectUnique`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects exactly one satisfying provider and rejects zero or ambiguous matches.

```cpp
struct SelectUnique final {};
```

### `SelectOptionalUnique`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects zero or one satisfying provider, returning void for zero and rejecting ambiguity.

```cpp
struct SelectOptionalUnique final {};
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects the unique satisfying provider advertising the minimum value for one Property.

- **Template parameter `TProperty`:** Property minimized across satisfying providers.

```cpp
template<class TProperty>
    struct SelectMinimum final
```

### `PropertyType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Property used to rank satisfying providers.

```cpp
using PropertyType = TProperty;
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects the unique satisfying provider advertising the maximum value for one Property.

- **Template parameter `TProperty`:** Property maximized across satisfying providers.

```cpp
template<class TProperty>
    struct SelectMaximum final
```

### `PropertyType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Property used to rank satisfying providers.

```cpp
using PropertyType = TProperty;
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects every satisfying provider tied for the minimum value of one Property.

- **Template parameter `TProperty`:** Property minimized across satisfying providers.

```cpp
template<class TProperty>
    struct SelectAllMinimum final
```

### `PropertyType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Property used to rank satisfying providers.

```cpp
using PropertyType = TProperty;
```

### `TProperty`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Selects every satisfying provider tied for the maximum value of one Property.

- **Template parameter `TProperty`:** Property maximized across satisfying providers.

```cpp
template<class TProperty>
    struct SelectAllMaximum final
```

### `PropertyType`

**Classification:** PUBLIC COMPOSITION API · source access: `public`

Property used to rank satisfying providers.

```cpp
using PropertyType = TProperty;
```

