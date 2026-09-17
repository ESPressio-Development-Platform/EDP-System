#pragma once

#include "../composition/Capability.hpp"

namespace ESPressio::System::Identity {

    /// Composition domain containing System Identity bootstrap capabilities.
    struct Domain final : CompositionFramework::Domain {};


    /// Exclusive capability that supplies the permanent DeviceIdentifier used by SystemIdentity.
    ///
    /// The concrete provider belongs at the platform or hardware layer best able to establish a
    /// stable, immutable, globally suitable device identity. System deliberately does not prescribe
    /// whether that identity originates from hardware fuses, a factory identifier, a secure device,
    /// provisioned storage, or another platform-appropriate source.
    struct DeviceIdentity final : CompositionFramework::ExclusiveCapability<Domain> {};


    /// Exclusive capability that supplies the durable RuntimeIncarnationId used by SystemIdentity.
    ///
    /// A conforming provider must establish a non-zero incarnation that is exactly one greater than
    /// the previously committed incarnation for the same device. The advancement must be durably
    /// committed before the provider reports success or exposes the new value. Zero is never valid,
    /// values must never be reused, and exhaustion must fail rather than wrap. The mechanism used to
    /// satisfy those guarantees is intentionally outside System and may be provided by Persistence,
    /// monotonic hardware, a secure element, or another future implementation.
    struct RuntimeIncarnationIdentity final : CompositionFramework::ExclusiveCapability<Domain> {};

} // ESPressio::System::Identity
