#include <cstdint>
#include <limits>

#include "ESPressio_System.hpp"

namespace Example {

    namespace Framework = ESPressio::System::CompositionFramework;
    namespace Identity = ESPressio::System::Identity;


    /// Example platform-level provider of a permanent DeviceIdentifier.
    class HardwareDeviceIdentity final : public Framework::Provider<
        Identity::Domain,
        Framework::Provides<
            Framework::Offer<Identity::DeviceIdentity>
        >
    > {
    private:

        // Resolved hardware identity.

        /// Example fixed identity standing in for a platform/hardware-derived value.
        Identity::DeviceIdentifier _identifier;

    public:

        // Construction.

        /// Creates the example hardware identity provider.
        HardwareDeviceIdentity() noexcept :
            _identifier(Identity::DeviceIdentifier::Storage{
                0xA1U, 0xB2U, 0xC3U, 0xD4U,
                0xE5U, 0xF6U, 0x07U, 0x18U,
                0x29U, 0x3AU, 0x4BU, 0x5CU,
                0x6DU, 0x7EU, 0x8FU, 0x90U
            }) {}


        // Provider operation.

        /// Resolves the permanent device identifier for Bootstrap.
        ///
        /// @param identifier Receives the resolved identifier on success.
        bool Resolve(Identity::DeviceIdentifier& identifier) const noexcept {
            if (!_identifier.IsValid()) return false;

            identifier = _identifier;
            return true;
        }

    };


    /// Example persistence-backed provider of the RuntimeIncarnationIdentity capability.
    class PersistentRuntimeIncarnationIdentity final : public Framework::Provider<
        Identity::Domain,
        Framework::Provides<
            Framework::Offer<Identity::RuntimeIncarnationIdentity>
        >
    > {
    private:

        // Example durable state.

        /// Last incarnation value treated as durably committed by this example.
        std::uint32_t _persistedValue{0U};

        /// Indicates whether the persistence prerequisite has been initialized.
        bool _persistenceReady{false};

    public:

        // Prerequisite lifecycle.

        /// Initializes the example persistence prerequisite before Identity establishment.
        bool InitializePersistence() noexcept {
            _persistenceReady = true;
            return true;
        }


        // Provider operation.

        /// Durably advances and exposes the next runtime incarnation.
        ///
        /// A real implementation must commit the increment to durable storage before returning true.
        ///
        /// @param incarnation Receives the newly committed incarnation on success.
        bool Establish(Identity::RuntimeIncarnationId& incarnation) noexcept {
            if (!_persistenceReady) return false;
            if (_persistedValue == std::numeric_limits<std::uint32_t>::max()) return false;

            ++_persistedValue;
            incarnation = Identity::RuntimeIncarnationId(_persistedValue);
            return true;
        }

    };


    /// Example consumer that retains only a pointer to the immutable device identity it needs.
    class IdentityConsumer final {
    private:

        // Borrowed identity state.

        /// Lifetime-stable pointer to the System-owned permanent device identifier.
        const Identity::DeviceIdentifier* _device{nullptr};

    public:

        // Construction.

        /// Creates the consumer after SystemIdentity has already been established.
        ///
        /// @param identity Application-lifetime System Identity facade.
        explicit IdentityConsumer(const Identity::SystemIdentity& identity) noexcept :
            _device(&identity.Device()) {}


        // Consumer operation.

        /// Returns whether the borrowed identity remains valid without copying it.
        bool HasValidDeviceIdentity() const noexcept {
            return _device != nullptr && _device->IsValid();
        }

    };


    /// Identity composition selected by this application Bootstrap.
    using ApplicationIdentityComposition = Framework::Composition<
        Identity::Domain,
        HardwareDeviceIdentity,
        PersistentRuntimeIncarnationIdentity
    >;


    /// Runs the complete example Bootstrap and consumer lifecycle.
    int Run() noexcept {
        using DeviceIdentityProvider =
            ApplicationIdentityComposition::ProviderFor<Identity::DeviceIdentity>;

        using RuntimeIncarnationIdentityProvider =
            ApplicationIdentityComposition::ProviderFor<Identity::RuntimeIncarnationIdentity>;

        DeviceIdentityProvider deviceIdentityProvider;
        RuntimeIncarnationIdentityProvider runtimeIncarnationProvider;

        // Persistence is a prerequisite of runtime-incarnation establishment.
        if (!runtimeIncarnationProvider.InitializePersistence()) return 1;

        // Resolve both semantic values into temporary Bootstrap staging storage.
        Identity::DeviceIdentifier deviceIdentifier;
        if (!deviceIdentityProvider.Resolve(deviceIdentifier)) return 2;

        Identity::RuntimeIncarnationId runtimeIncarnation;
        if (!runtimeIncarnationProvider.Establish(runtimeIncarnation)) return 3;

        // SystemIdentity now owns the only application-lifetime copy required by System.
        Identity::SystemIdentity systemIdentity(
            deviceIdentifier,
            runtimeIncarnation
        );
        if (!systemIdentity.IsValid()) return 4;

        // Identity-dependent consumers are initialized only after SystemIdentity exists.
        IdentityConsumer consumer(systemIdentity);
        if (!consumer.HasValidDeviceIdentity()) return 5;

        return 0;
    }

} // Example


/// Runs the basic System Identity example.
int main() {
    return Example::Run();
}
