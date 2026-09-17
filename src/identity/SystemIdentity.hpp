#pragma once

#include <type_traits>

#include "DeviceIdentifier.hpp"
#include "RuntimeIncarnationId.hpp"

namespace ESPressio::System::Identity {

    /// Immutable, address-stable identity of the currently running application instance.
    ///
    /// SystemIdentity owns exactly one DeviceIdentifier and one RuntimeIncarnationId.
    /// Bootstrap constructs it only after both values have been established successfully.
    /// The object is intentionally non-copyable and non-movable so references and pointers
    /// borrowed by later-initialized consumers remain valid for its application lifetime.
    class SystemIdentity final {
    private:

        // Identity values.

        /// Permanent identity of this device.
        DeviceIdentifier _device;

        /// Durable identity of this running incarnation.
        RuntimeIncarnationId _incarnation;

    public:

        // Construction and lifetime.

        /// Prevents creation of an identity before its required values exist.
        SystemIdentity() = delete;

        /// Creates the immutable identity of the current application runtime.
        ///
        /// Bootstrap must provide a valid DeviceIdentifier and a RuntimeIncarnationId whose
        /// advancement has already been made durable by the RuntimeIncarnationIdentity provider.
        ///
        /// @param device Permanent device identity.
        /// @param incarnation Durably established current runtime incarnation.
        constexpr SystemIdentity(
            const DeviceIdentifier& device,
            const RuntimeIncarnationId& incarnation
        ) noexcept :
            _device(device),
            _incarnation(incarnation) {}

        /// Prevents creation of a second facade by copying the lifetime identity.
        SystemIdentity(const SystemIdentity&) = delete;

        /// Prevents replacement of the lifetime identity by copy assignment.
        SystemIdentity& operator =(const SystemIdentity&) = delete;

        /// Prevents relocation of the lifetime identity after construction.
        SystemIdentity(SystemIdentity&&) = delete;

        /// Prevents relocation of the lifetime identity by move assignment.
        SystemIdentity& operator =(SystemIdentity&&) = delete;


        // Identity access.

        /// Returns the permanent device identity by immutable reference without copying it.
        constexpr const DeviceIdentifier& Device() const noexcept {
            return _device;
        }

        /// Returns the current runtime incarnation by immutable reference without copying it.
        constexpr const RuntimeIncarnationId& Incarnation() const noexcept {
            return _incarnation;
        }

        /// Indicates whether both constituent identity values are valid.
        constexpr bool IsValid() const noexcept {
            return _device.IsValid() && _incarnation.IsValid();
        }

        /// Indicates whether both constituent identity values are valid.
        constexpr explicit operator bool() const noexcept {
            return IsValid();
        }

    };


    static_assert(
        sizeof(SystemIdentity) == sizeof(DeviceIdentifier) + sizeof(RuntimeIncarnationId),
        "SystemIdentity must retain only its 20 bytes of semantic identity state"
    );

    static_assert(
        sizeof(SystemIdentity) == 20U,
        "SystemIdentity must remain an exact 20-byte semantic value"
    );

    static_assert(
        !std::is_copy_constructible_v<SystemIdentity> &&
        !std::is_copy_assignable_v<SystemIdentity> &&
        !std::is_move_constructible_v<SystemIdentity> &&
        !std::is_move_assignable_v<SystemIdentity>,
        "SystemIdentity must remain address-stable after construction"
    );

} // ESPressio::System::Identity
