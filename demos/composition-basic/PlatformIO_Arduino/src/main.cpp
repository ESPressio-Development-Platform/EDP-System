#include <Arduino.h>

#include <cstdint>

#include <ESPressio_System.hpp>

namespace Demo {

    namespace Framework = ESPressio::System::CompositionFramework;

    /// Composition domain used by this demonstration.
    struct Domain final : Framework::Domain {};


    /// Exclusive counter capability selected by application Composition.
    struct Counter final : Framework::ExclusiveCapability<Domain> {};


    /// Concrete provider selected for the Counter capability.
    class CounterProvider final : public Framework::Provider<
        Domain,
        Framework::Provides<
            Framework::Offer<Counter>
        >
    > {
    private:

        // Demonstration state.

        /// Current counter value.
        std::uint32_t _value{0U};

    public:

        // Counter operation.

        /// Increments the demonstration counter.
        void Increment() noexcept {
            ++_value;
        }

        /// Returns the current demonstration counter value.
        std::uint32_t Value() const noexcept {
            return _value;
        }

    };


    /// Complete compile-time application architecture.
    using ApplicationComposition = Framework::Composition<
        Domain,
        CounterProvider
    >;


    static_assert(
        ApplicationComposition::IsValid,
        "Demonstration Composition must be valid"
    );


    /// Runs the Composition Framework demonstration.
    int Run() noexcept {
        ApplicationComposition::ProviderFor<Counter> counter;

        counter.Increment();

        return counter.Value() == 1U ? 0 : 1;
    }

} // Demo

/// Runs the demonstration once during Arduino initialization.
void setup() {
    static_cast<void>(Demo::Run());
}

/// Leaves the demonstration idle after the one-time run.
void loop() {}
