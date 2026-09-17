#include <Arduino.h>

#include <cstdint>
#include <limits>

#include <ESPressio_System.hpp>

namespace Demo {

    namespace Framework = ESPressio::System::CompositionFramework;
    namespace Identity = ESPressio::System::Identity;


    /// Demonstration permanent device-identity provider.
    class DeviceIdentityProvider final : public Framework::Provider<
        Identity::Domain,
        Framework::Provides<
            Framework::Offer<Identity::DeviceIdentity>
        >
    > {
    private:

        // Permanent identity.

        /// Fixed demonstration device identifier.
        Identity::DeviceIdentifier _identifier;

    public:

        // Construction.

        /// Creates the demonstration provider with a stable non-zero identifier.
        DeviceIdentityProvider() noexcept :
            _identifier(Identity::DeviceIdentifier::Storage{
                0x10U, 0x20U, 0x30U, 0x40U,
                0x50U, 0x60U, 0x70U, 0x80U,
                0x90U, 0xA0U, 0xB0U, 0xC0U,
                0xD0U, 0xE0U, 0xF0U, 0x01U
            }) {}


        // Provider operation.

        /// Resolves the permanent device identifier.
        bool Resolve(
            Identity::DeviceIdentifier& identifier
        ) const noexcept {
            if (!_identifier.IsValid()) return false;

            identifier = _identifier;
            return true;
        }

    };


    /// Demonstration durable runtime-incarnation provider.
    class RuntimeIncarnationProvider final : public Framework::Provider<
        Identity::Domain,
        Framework::Provides<
            Framework::Offer<Identity::RuntimeIncarnationIdentity>
        >
    > {
    private:

        // Durable demonstration state.

        /// Last value treated as durably committed by this demonstration.
        std::uint32_t _committed{0U};

    public:

        // Provider operation.

        /// Commits and exposes the next runtime incarnation.
        bool Establish(
            Identity::RuntimeIncarnationId& incarnation
        ) noexcept {
            if (_committed == std::numeric_limits<std::uint32_t>::max()) return false;

            ++_committed;
            incarnation = Identity::RuntimeIncarnationId(_committed);
            return true;
        }

    };


    /// Compile-time Identity architecture selected by application Bootstrap.
    using ApplicationComposition = Framework::Composition<
        Identity::Domain,
        DeviceIdentityProvider,
        RuntimeIncarnationProvider
    >;


    /// Runs the System Identity demonstration.
    int Run() noexcept {
        ApplicationComposition::ProviderFor<Identity::DeviceIdentity> deviceProvider;
        ApplicationComposition::ProviderFor<Identity::RuntimeIncarnationIdentity> incarnationProvider;

        Identity::DeviceIdentifier deviceIdentifier;
        Identity::RuntimeIncarnationId incarnation;

        if (!deviceProvider.Resolve(deviceIdentifier)) return 1;
        if (!incarnationProvider.Establish(incarnation)) return 2;

        Identity::SystemIdentity identity(
            deviceIdentifier,
            incarnation
        );

        return identity.IsValid() ? 0 : 3;
    }

} // Demo

/// Runs the demonstration once during Arduino initialization.
void setup() {
    static_cast<void>(Demo::Run());
}

/// Leaves the demonstration idle after the one-time run.
void loop() {}
