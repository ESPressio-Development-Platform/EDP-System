#include <cstdint>

#include "ESPressio_System.hpp"

namespace Example {

    namespace Composition = ESPressio::System::CompositionFramework;

    /// Platform capabilities used by this example application.
    struct PlatformDomain final : Composition::Domain {};


    /// Definitive system clock capability for the application.
    struct SystemClock final : Composition::ExclusiveCapability<PlatformDomain> {};


    /// Resolution of the SystemClock capability, expressed in nanoseconds.
    struct ClockResolutionNanoseconds final : Composition::Property<SystemClock, std::uint32_t> {};


    /// Concrete clock provider used by this example application.
    class ExampleClock final : public Composition::Provider<
        PlatformDomain,
        Composition::Offers<
            Composition::Offer<
                SystemClock,
                Composition::PropertyValue<
                    ClockResolutionNanoseconds,
                    1000U
                >
            >
        >
    > {

        private:

            // Runtime state.

            /// Indicates whether the example clock has completed initialization.
            bool _initialized = false;

        public:

            // Public lifecycle.

            /// Initializes the clock provider before it is consumed by the application.
            void Initialize() {
                _initialized = true;
            }

            // Public clock access.

            /// Returns a placeholder time value for this host-side example.
            std::uint64_t Now() const {
                if (!_initialized) { return 0U; }

                return 1000U;
            }

    };


    /// Requirement selecting the unique application SystemClock provider.
    using SystemClockRequirement = Composition::Requirement<
        SystemClock,
        Composition::RequirementScope::SameDomain,
        Composition::ExactlyProviders<1U>
    >;


    /// Complete compile-time architecture for the example application.
    using ApplicationComposition = Composition::Composition<
        PlatformDomain,
        ExampleClock
    >;


    /// Owns and operates the runtime objects selected by the application composition.
    class ApplicationRuntime final {

        private:

            // Runtime providers.

            /// Concrete provider selected by the composition for the SystemClock capability.
            ApplicationComposition::Select<
                SystemClockRequirement,
                Composition::SelectUnique
            > _clock;

        public:

            // Public lifecycle.

            /// Initializes every runtime provider owned by this application.
            void Initialize() {
                _clock.Initialize();
            }

            /// Executes the application after its runtime providers have been initialized.
            int Run() {
                return _clock.Now() > 0U ? 0 : 1;
            }

    };

} // Example


/// Executes the basic Composition Framework example.
int main() {
    Example::ApplicationRuntime application;

    application.Initialize();

    return application.Run();
}
