#include <Arduino.h>

#include <cstdint>

#include <ESPressio_System.hpp>

namespace Demo {

    /// Runtime feature flags used by this demonstration.
    enum class Feature : std::uint8_t {
        Clock = 0,
        Commands = 1,
        Events = 2,
        State = 3
    };


    /// Compact eight-bit representation used for the demonstration feature set.
    using FeatureFlags = ESPressio::System::FlagSet<Feature, std::uint8_t>;


    static_assert(
        sizeof(FeatureFlags) == sizeof(std::uint8_t),
        "The demonstration FlagSet must add no persistent storage overhead"
    );


    /// Exercises explicit construction and the primary flag mutation/predicate operations.
    int Run() noexcept {
        FeatureFlags flags(
            FeatureFlags::State(
                Feature::Clock,
                true
            ),
            FeatureFlags::State(
                Feature::Commands,
                false
            ),
            FeatureFlags::State(
                Feature::Events,
                true
            )
        );

        if (!flags.HasAll(
            Feature::Clock,
            Feature::Events
        )) return 1;

        flags.Set(Feature::Commands);
        flags.Clear(Feature::Events);
        flags.Flip(Feature::State);

        flags.AssignMask(
            0x03U,
            0x02U
        );

        if (!flags.IsSet(Feature::Commands)) return 2;
        if (flags.IsSet(Feature::Clock)) return 3;
        if (!flags.IsSet(Feature::State)) return 4;

        return flags.Value() == 0x0AU ? 0 : 5;
    }

} // Demo


/// Runs the FlagSet demonstration once during Arduino initialization.
void setup() {
    static_cast<void>(Demo::Run());
}

/// Leaves the demonstration idle after the one-time run.
void loop() {}
