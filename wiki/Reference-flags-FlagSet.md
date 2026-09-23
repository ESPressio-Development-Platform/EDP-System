# src/flags/FlagSet.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `9964ab2820cc5d290de69674000e011faaa684d9`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-System/blob/9964ab2820cc5d290de69674000e011faaa684d9/src/flags/FlagSet.hpp)

## Direct includes

- `concepts`
- `cstddef`
- `cstdint`
- `type_traits`

## Documented declarations

### `TFlag`

**Classification:** PUBLIC API

Describes one explicit desired state for a strongly typed flag.

- **Template parameter `TFlag`:** Enum type defining the flag domain.

```cpp
template<class TFlag>
    class FlagState final
```

### `_flag`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Flag whose desired state is represented.

```cpp
TFlag _flag;
```

### `_isSet`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Desired set/clear state of the flag.

```cpp
bool _isSet;
```

### `FlagState`

**Classification:** PUBLIC API · source access: `public`

Creates an explicit state assignment for one flag.

- **Parameter `flag`:** Flag whose state is being described.
- **Parameter `isSet`:** True when the flag should be set; false when it should be clear.

```cpp
constexpr FlagState(
            TFlag flag,
            bool isSet
        ) noexcept :
            _flag(flag),
```

### `Flag`

**Classification:** PUBLIC API · source access: `public`

Returns the flag whose state is represented.

```cpp
constexpr TFlag Flag() const noexcept
```

### `IsSet`

**Classification:** PUBLIC API · source access: `public`

Indicates whether the represented flag should be set.

```cpp
constexpr bool IsSet() const noexcept
```

### `TFlag`

**Classification:** PUBLIC API

Compact strongly typed set of enum-domain flags backed by one unsigned integer.

Enum values represent zero-based bit positions in the selected storage type.
Values outside the storage capacity are not representable and never mutate storage.

- **Template parameter `TFlag`:** Enum type defining the flag domain and zero-based bit positions.
- **Template parameter `TStorage`:** Unsigned integral type used as the complete persistent representation.

```cpp
template<class TFlag, class TStorage = std::uint32_t>
    class FlagSet final
```

### `FlagUnderlying`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Underlying integer type used by the flag enum.

```cpp
using FlagUnderlying = std::underlying_type_t<TFlag>;
```

### `UnsignedFlagUnderlying`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Unsigned form of the enum's underlying integer type.

```cpp
using UnsignedFlagUnderlying = std::make_unsigned_t<FlagUnderlying>;
```

### `StateType`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Explicit desired state representation used by state-aware construction.

```cpp
using StateType = FlagState<TFlag>;
```

### `sizeof`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Number of independently representable flag positions.

```cpp
static constexpr std::size_t StorageBitCount = sizeof(TStorage) * 8U;
```

### `TStorage _bits{0U};`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Complete bit representation of all current flag states.

```cpp
TStorage _bits{0U};
```

### `IsFlagRepresentable`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether a flag's bit position can be represented by the selected storage type.

- **Parameter `flag`:** Flag whose bit position should be validated.

```cpp
static constexpr bool IsFlagRepresentable(TFlag flag) noexcept
```

### `requires`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether every supplied flag can be represented by the selected storage type.

- **Template parameter `TAdditionalFlags`:** Additional flag argument types, each required to match TFlag.
- **Parameter `flag`:** First flag whose bit position should be validated.
- **Parameter `additionalFlags`:** Additional flags whose bit positions should be validated.

```cpp
template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        static constexpr bool AreFlagsRepresentable(
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) noexcept
```

### `Mask`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Returns the single-bit mask for one representable flag, or zero when it cannot be represented.

- **Parameter `flag`:** Flag whose bit mask should be produced.

```cpp
static constexpr TStorage Mask(TFlag flag) noexcept
```

### `requires`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Returns the combined mask for one or more flags.

- **Template parameter `TAdditionalFlags`:** Additional flag argument types, each required to match TFlag.
- **Parameter `flag`:** First flag whose bit should be represented in the mask.
- **Parameter `additionalFlags`:** Additional flags whose bits should be represented in the mask.

```cpp
template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        static constexpr TStorage Mask(
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) noexcept
```

### `ApplyState`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Applies one explicit flag state to the current representation.

- **Parameter `state`:** Explicit state to apply.

```cpp
constexpr void ApplyState(const StateType& state) noexcept
```

### `Flag`

**Classification:** PUBLIC API · source access: `public`

Enum type defining the flag domain.

```cpp
using Flag = TFlag;
```

### `Storage`

**Classification:** PUBLIC API · source access: `public`

Unsigned integer type used as the complete persistent representation.

```cpp
using Storage = TStorage;
```

### `State`

**Classification:** PUBLIC API · source access: `public`

Explicit per-flag state type accepted by state-aware constructors.

```cpp
using State = StateType;
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Number of independently representable flag positions.

```cpp
static constexpr std::size_t Capacity = StorageBitCount;
```

### `FlagSet`

**Classification:** PUBLIC API · source access: `public`

Creates an empty flag set with every flag clear.

```cpp
constexpr FlagSet() noexcept = default;
```

### `FlagSet`

**Classification:** PUBLIC API · source access: `public`

Creates a flag set from its complete raw storage representation.

- **Parameter `value`:** Complete bit representation to preserve exactly.

```cpp
constexpr explicit FlagSet(Storage value) noexcept :
            _bits(value) {}
```

### `requires`

**Classification:** PUBLIC API · source access: `public`

Creates a flag set with the supplied flags initially set.

Unrepresentable values are ignored so no invalid shift can occur.

- **Template parameter `TAdditionalFlags`:** Additional flag argument types, each required to match TFlag.
- **Parameter `flag`:** First flag to set.
- **Parameter `additionalFlags`:** Additional flags to set.

```cpp
template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        constexpr explicit FlagSet(
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) noexcept :
            _bits(Mask(
                flag,
                additionalFlags...
            )) {}
```

### `requires`

**Classification:** PUBLIC API · source access: `public`

Creates a zero-based flag set and applies explicit state assignments from left to right.

Later assignments to the same flag override earlier assignments.
Unrepresentable values are ignored.

- **Template parameter `TAdditionalStates`:** Additional state argument types, each required to match State.
- **Parameter `state`:** First explicit state assignment.
- **Parameter `additionalStates`:** Additional explicit state assignments.

```cpp
template<class... TAdditionalStates>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalStates>, State>) && ...)
        constexpr explicit FlagSet(
            State state,
            TAdditionalStates... additionalStates
        ) noexcept
```

### `requires`

**Classification:** PUBLIC API · source access: `public`

Creates a flag set from raw storage and then applies explicit state overrides from left to right.

Later assignments to the same flag override earlier assignments.
Unrepresentable values are ignored.

- **Template parameter `TAdditionalStates`:** Additional state argument types, each required to match State.
- **Parameter `value`:** Initial complete bit representation.
- **Parameter `state`:** First explicit state override.
- **Parameter `additionalStates`:** Additional explicit state overrides.

```cpp
template<class... TAdditionalStates>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalStates>, State>) && ...)
        constexpr explicit FlagSet(
            Storage value,
            State state,
            TAdditionalStates... additionalStates
        ) noexcept :
            _bits(value)
```

### `Value`

**Classification:** PUBLIC API · source access: `public`

Returns the complete raw storage representation.

```cpp
constexpr Storage Value() const noexcept
```

### `IsEmpty`

**Classification:** PUBLIC API · source access: `public`

Indicates whether no represented flags are currently set.

```cpp
constexpr bool IsEmpty() const noexcept
```

### `IsRepresentable`

**Classification:** PUBLIC API · source access: `public`

Indicates whether a flag's bit position can be represented by this flag-set type.

- **Parameter `flag`:** Flag whose position should be checked.

```cpp
constexpr bool IsRepresentable(TFlag flag) const noexcept
```

### `Clear`

**Classification:** PUBLIC API · source access: `public`

Clears every represented flag.

```cpp
constexpr void Clear() noexcept
```

### `requires`

**Classification:** PUBLIC API · source access: `public`

Sets one or more flags while preserving every other represented flag.

Unrepresentable values are ignored so no invalid shift can occur.

- **Template parameter `TAdditionalFlags`:** Additional flag argument types, each required to match TFlag.
- **Parameter `flag`:** First flag to set.
- **Parameter `additionalFlags`:** Additional flags to set.

```cpp
template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        constexpr void Set(
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) noexcept
```

### `requires`

**Classification:** PUBLIC API · source access: `public`

Clears one or more flags while preserving every other represented flag.

Unrepresentable values are ignored so no invalid shift can occur.

- **Template parameter `TAdditionalFlags`:** Additional flag argument types, each required to match TFlag.
- **Parameter `flag`:** First flag to clear.
- **Parameter `additionalFlags`:** Additional flags to clear.

```cpp
template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        constexpr void Clear(
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) noexcept
```

### `requires`

**Classification:** PUBLIC API · source access: `public`

Flips one or more flags while preserving every other represented flag.

Unrepresentable values are ignored so no invalid shift can occur.

- **Template parameter `TAdditionalFlags`:** Additional flag argument types, each required to match TFlag.
- **Parameter `flag`:** First flag to flip.
- **Parameter `additionalFlags`:** Additional flags to flip.

```cpp
template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        constexpr void Flip(
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) noexcept
```

### `requires`

**Classification:** PUBLIC API · source access: `public`

Assigns one common set/clear state to one or more flags.

Unrepresentable values are ignored so no invalid shift can occur.

- **Template parameter `TAdditionalFlags`:** Additional flag argument types, each required to match TFlag.
- **Parameter `isSet`:** True to set the supplied flags; false to clear them.
- **Parameter `flag`:** First flag to assign.
- **Parameter `additionalFlags`:** Additional flags to assign.

```cpp
template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        constexpr void Assign(
            bool isSet,
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) noexcept
```

### `AssignMask`

**Classification:** PUBLIC API · source access: `public`

Assigns selected raw bits from a state value while preserving bits outside the mask.

- **Parameter `mask`:** Raw bit mask selecting the bits to replace.
- **Parameter `state`:** Raw state whose selected bits should be assigned.

```cpp
constexpr void AssignMask(
            Storage mask,
            Storage state
        ) noexcept
```

### `IsSet`

**Classification:** PUBLIC API · source access: `public`

Indicates whether one representable flag is currently set.

- **Parameter `flag`:** Flag whose current state should be inspected.

```cpp
constexpr bool IsSet(TFlag flag) const noexcept
```

### `requires`

**Classification:** PUBLIC API · source access: `public`

Indicates whether at least one of the supplied representable flags is currently set.

Unrepresentable values do not contribute a represented bit to the predicate.

- **Template parameter `TAdditionalFlags`:** Additional flag argument types, each required to match TFlag.
- **Parameter `flag`:** First flag to inspect.
- **Parameter `additionalFlags`:** Additional flags to inspect.

```cpp
template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        constexpr bool HasAny(
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) const noexcept
```

### `requires`

**Classification:** PUBLIC API · source access: `public`

Indicates whether every supplied flag is representable and currently set.

- **Template parameter `TAdditionalFlags`:** Additional flag argument types, each required to match TFlag.
- **Parameter `flag`:** First flag to inspect.
- **Parameter `additionalFlags`:** Additional flags to inspect.

```cpp
template<class... TAdditionalFlags>
        requires ((std::same_as<std::remove_cvref_t<TAdditionalFlags>, TFlag>) && ...)
        constexpr bool HasAll(
            TFlag flag,
            TAdditionalFlags... additionalFlags
        ) const noexcept
```

