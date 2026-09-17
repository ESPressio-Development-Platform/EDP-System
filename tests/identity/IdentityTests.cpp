#include <array>
#include <cstdint>
#include <limits>
#include <type_traits>

#include "ESPressio_System.hpp"

namespace ESPressio::System::Tests::Identity {

    namespace Framework = ESPressio::System::CompositionFramework;
    namespace IdentityDomain = ESPressio::System::Identity;


    /// Deterministic test provider for the permanent DeviceIdentity capability.
    class TestDeviceIdentity final : public Framework::Provider<
        IdentityDomain::Domain,
        Framework::Provides<
            Framework::Offer<IdentityDomain::DeviceIdentity>
        >
    > {
    private:

        // Test identity state.

        /// Deterministic device identity returned by this provider.
        IdentityDomain::DeviceIdentifier _identifier;

    public:

        // Construction.

        /// Creates the test provider with a deterministic identifier.
        explicit constexpr TestDeviceIdentity(
            const IdentityDomain::DeviceIdentifier& identifier
        ) noexcept :
            _identifier(identifier) {}


        // Provider operation.

        /// Resolves the permanent device identifier into bootstrap-owned staging storage.
        ///
        /// @param identifier Receives the resolved identifier on success.
        bool Resolve(IdentityDomain::DeviceIdentifier& identifier) const noexcept {
            if (!_identifier.IsValid()) return false;

            identifier = _identifier;
            return true;
        }

    };


    /// Test provider simulating the durable RuntimeIncarnationIdentity capability.
    class TestRuntimeIncarnationIdentity final : public Framework::Provider<
        IdentityDomain::Domain,
        Framework::Provides<
            Framework::Offer<IdentityDomain::RuntimeIncarnationIdentity>
        >
    > {
    private:

        // Simulated durable state.

        /// Last incarnation value considered durably committed by the test provider.
        std::uint32_t _persistedValue{0U};

        /// Indicates whether the simulated persistence prerequisite was initialized.
        bool _initialized{false};

    public:

        // Test prerequisite lifecycle.

        /// Initializes the simulated persistence prerequisite.
        void Initialize() noexcept {
            _initialized = true;
        }


        // Provider operation.

        /// Durably advances and returns the next runtime incarnation.
        ///
        /// @param incarnation Receives the newly committed incarnation on success.
        bool Establish(IdentityDomain::RuntimeIncarnationId& incarnation) noexcept {
            if (!_initialized) return false;
            if (_persistedValue == std::numeric_limits<std::uint32_t>::max()) return false;

            ++_persistedValue;
            incarnation = IdentityDomain::RuntimeIncarnationId(_persistedValue);
            return true;
        }

        /// Returns the simulated last durably committed value for validation.
        std::uint32_t PersistedValue() const noexcept {
            return _persistedValue;
        }

    };


    /// Valid Identity composition used to verify capability resolution.
    using TestIdentityComposition = Framework::Composition<
        IdentityDomain::Domain,
        TestDeviceIdentity,
        TestRuntimeIncarnationIdentity
    >;


    static_assert(
        sizeof(IdentityDomain::DeviceIdentifier) == 16U,
        "DeviceIdentifier must occupy exactly 16 bytes"
    );

    static_assert(
        sizeof(IdentityDomain::RuntimeIncarnationId) == 4U,
        "RuntimeIncarnationId must occupy exactly 4 bytes"
    );

    static_assert(
        sizeof(IdentityDomain::SystemIdentity) == 20U,
        "SystemIdentity must occupy exactly 20 bytes"
    );

    static_assert(
        !std::is_default_constructible_v<IdentityDomain::SystemIdentity>,
        "SystemIdentity must not exist before its identity values are established"
    );

    static_assert(
        !std::is_copy_constructible_v<IdentityDomain::SystemIdentity> &&
        !std::is_copy_assignable_v<IdentityDomain::SystemIdentity>,
        "SystemIdentity must not be copyable"
    );

    static_assert(
        !std::is_move_constructible_v<IdentityDomain::SystemIdentity> &&
        !std::is_move_assignable_v<IdentityDomain::SystemIdentity>,
        "SystemIdentity must not be movable"
    );

    static_assert(
        std::is_trivially_destructible_v<IdentityDomain::SystemIdentity>,
        "SystemIdentity must not require runtime destruction machinery"
    );

    static_assert(
        TestIdentityComposition::IsValid,
        "Expected the Identity test composition to be valid"
    );

    static_assert(
        std::is_same_v<
            TestIdentityComposition::ProviderFor<IdentityDomain::DeviceIdentity>,
            TestDeviceIdentity
        >,
        "Expected DeviceIdentity to resolve to TestDeviceIdentity"
    );

    static_assert(
        std::is_same_v<
            TestIdentityComposition::ProviderFor<IdentityDomain::RuntimeIncarnationIdentity>,
            TestRuntimeIncarnationIdentity
        >,
        "Expected RuntimeIncarnationIdentity to resolve to TestRuntimeIncarnationIdentity"
    );


    /// Executes runtime validation for Identity values, bootstrap ordering, and zero-copy access.
    int RunIdentityTests() noexcept {
        IdentityDomain::DeviceIdentifier invalidDevice;
        if (invalidDevice.IsValid()) return 1;

        IdentityDomain::RuntimeIncarnationId invalidIncarnation;
        if (invalidIncarnation.IsValid()) return 2;

        const IdentityDomain::DeviceIdentifier::Storage deviceBytes{
            0x10U, 0x21U, 0x32U, 0x43U,
            0x54U, 0x65U, 0x76U, 0x87U,
            0x98U, 0xA9U, 0xBAU, 0xCBU,
            0xDCU, 0xEDU, 0xFEU, 0x0FU
        };

        const IdentityDomain::DeviceIdentifier expectedDevice(deviceBytes);
        TestDeviceIdentity deviceProvider(expectedDevice);
        TestRuntimeIncarnationIdentity incarnationProvider;

        IdentityDomain::RuntimeIncarnationId prematureIncarnation;
        if (incarnationProvider.Establish(prematureIncarnation)) return 3;
        if (prematureIncarnation.IsValid()) return 4;

        incarnationProvider.Initialize();

        IdentityDomain::DeviceIdentifier resolvedDevice;
        if (!deviceProvider.Resolve(resolvedDevice)) return 5;
        if (resolvedDevice != expectedDevice) return 6;

        IdentityDomain::RuntimeIncarnationId firstIncarnation;
        if (!incarnationProvider.Establish(firstIncarnation)) return 7;
        if (firstIncarnation.Value() != 1U) return 8;
        if (incarnationProvider.PersistedValue() != firstIncarnation.Value()) return 9;

        IdentityDomain::SystemIdentity identity(resolvedDevice, firstIncarnation);
        if (!identity.IsValid()) return 10;

        const auto* firstDevicePointer = &identity.Device();
        const auto* secondDevicePointer = &identity.Device();
        if (firstDevicePointer != secondDevicePointer) return 11;
        if (*firstDevicePointer != expectedDevice) return 12;

        const auto* firstIncarnationPointer = &identity.Incarnation();
        const auto* secondIncarnationPointer = &identity.Incarnation();
        if (firstIncarnationPointer != secondIncarnationPointer) return 13;
        if (firstIncarnationPointer->Value() != 1U) return 14;

        IdentityDomain::RuntimeIncarnationId secondIncarnation;
        if (!incarnationProvider.Establish(secondIncarnation)) return 15;
        if (secondIncarnation.Value() != 2U) return 16;
        if (identity.Incarnation().Value() != 1U) return 17;

        const IdentityDomain::DeviceIdentifier::Storage laterBytes{
            0x10U, 0x21U, 0x32U, 0x43U,
            0x54U, 0x65U, 0x76U, 0x87U,
            0x98U, 0xA9U, 0xBAU, 0xCBU,
            0xDCU, 0xEDU, 0xFEU, 0x10U
        };

        const IdentityDomain::DeviceIdentifier laterDevice(laterBytes);
        if (!(expectedDevice < laterDevice)) return 18;
        if (laterDevice < expectedDevice) return 19;

        return 0;
    }

} // ESPressio::System::Tests::Identity


/// Executes the host-side System Identity tests.
int main() {
    return ESPressio::System::Tests::Identity::RunIdentityTests();
}
