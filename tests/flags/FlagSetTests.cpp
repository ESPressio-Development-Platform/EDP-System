#include <concepts>
#include <cstdint>
#include <type_traits>

#include "ESPressio_System.hpp"

namespace ESPressio::System::Tests::Flags {

    /// Flag domain used to validate compact storage and bit-position semantics.
    enum class Feature : std::uint8_t {
        Clock = 0,
        Commands = 1,
        Events = 2,
        Highest = 7,
        OutOfRange = 8
    };


    /// Different flag domain used to verify compile-time domain isolation.
    enum class OtherFeature : std::uint8_t {
        Other = 0
    };


    /// Eight-bit FlagSet used for boundary and runtime behavior tests.
    using Flags8 = ESPressio::System::FlagSet<Feature, std::uint8_t>;

    /// Sixteen-bit FlagSet used to verify zero storage overhead.
    using Flags16 = ESPressio::System::FlagSet<Feature, std::uint16_t>;

    /// Thirty-two-bit FlagSet used to verify the default-size storage class.
    using Flags32 = ESPressio::System::FlagSet<Feature, std::uint32_t>;

    /// Sixty-four-bit FlagSet used to verify zero storage overhead.
    using Flags64 = ESPressio::System::FlagSet<Feature, std::uint64_t>;

    /// Default-storage FlagSet used to verify the public default storage selection.
    using DefaultFlags = ESPressio::System::FlagSet<Feature>;


    /// Indicates whether a FlagSet incorrectly accepts a flag from another enum domain.
    ///
    /// @tparam TFlags Flag-set type whose Set operation should be inspected.
    template<class TFlags>
    concept AcceptsOtherFeature = requires(TFlags flags) {
        flags.Set(OtherFeature::Other);
    };


    static_assert(
        sizeof(Flags8) == sizeof(std::uint8_t),
        "Eight-bit FlagSet must add zero persistent storage overhead"
    );

    static_assert(
        sizeof(Flags16) == sizeof(std::uint16_t),
        "Sixteen-bit FlagSet must add zero persistent storage overhead"
    );

    static_assert(
        sizeof(Flags32) == sizeof(std::uint32_t),
        "Thirty-two-bit FlagSet must add zero persistent storage overhead"
    );

    static_assert(
        sizeof(Flags64) == sizeof(std::uint64_t),
        "Sixty-four-bit FlagSet must add zero persistent storage overhead"
    );

    static_assert(
        std::same_as<typename DefaultFlags::Storage, std::uint32_t>,
        "Default FlagSet storage must remain uint32_t"
    );

    static_assert(
        std::is_trivially_copyable_v<Flags8>,
        "FlagSet must remain trivially copyable"
    );

    static_assert(
        std::is_trivially_destructible_v<Flags8>,
        "FlagSet must not require runtime destruction machinery"
    );

    static_assert(
        std::is_standard_layout_v<Flags8>,
        "FlagSet must remain standard-layout"
    );

    static_assert(
        !std::is_convertible_v<std::uint8_t, Flags8>,
        "Raw storage construction must remain explicit"
    );

    static_assert(
        !AcceptsOtherFeature<Flags8>,
        "A FlagSet must reject flags from another enum domain"
    );


    /// Default-constructed compile-time value used to verify empty-state semantics.
    constexpr Flags8 EmptyFlags;

    static_assert(
        EmptyFlags.IsEmpty(),
        "Default construction must produce the empty set"
    );

    static_assert(
        EmptyFlags.Value() == 0U,
        "Default construction must zero the raw representation"
    );


    /// Raw compile-time value used to verify exact storage preservation.
    constexpr Flags8 RawFlags(0x05U);

    static_assert(
        RawFlags.IsSet(Feature::Clock) && RawFlags.IsSet(Feature::Events),
        "Raw construction must expose the encoded set flags"
    );

    static_assert(
        !RawFlags.IsSet(Feature::Commands),
        "Raw construction must preserve clear flags"
    );


    /// Flag-list compile-time value used to verify direct set construction.
    constexpr Flags8 ListedFlags(
        Feature::Clock,
        Feature::Events
    );

    static_assert(
        ListedFlags.Value() == 0x05U,
        "Flag-list construction must set exactly the requested bits"
    );


    /// Explicit-state compile-time value used to verify per-flag state construction.
    constexpr Flags8 StateFlags(
        Flags8::State(
            Feature::Clock,
            true
        ),
        Flags8::State(
            Feature::Commands,
            false
        ),
        Flags8::State(
            Feature::Events,
            true
        )
    );

    static_assert(
        StateFlags.Value() == 0x05U,
        "Explicit-state construction must honor each requested flag state"
    );


    /// Raw value plus explicit state overrides used to verify clear-state construction has meaning.
    constexpr Flags8 OverriddenRawFlags(
        0xFFU,
        Flags8::State(
            Feature::Clock,
            false
        ),
        Flags8::State(
            Feature::Commands,
            true
        )
    );

    static_assert(
        OverriddenRawFlags.Value() == 0xFEU,
        "Explicit states must override the supplied raw representation from left to right"
    );

    static_assert(
        Flags8::Capacity == 8U,
        "Eight-bit storage must expose eight representable flag positions"
    );

    static_assert(
        EmptyFlags.IsRepresentable(Feature::Highest),
        "The highest valid bit position must be representable"
    );

    static_assert(
        !EmptyFlags.IsRepresentable(Feature::OutOfRange),
        "A bit position beyond storage capacity must not be representable"
    );


    /// Builds a constexpr FlagSet through every primary mutation surface.
    constexpr Flags8 BuildCompileTimeMutation() noexcept {
        Flags8 flags;

        flags.Set(
            Feature::Clock,
            Feature::Commands
        );
        flags.Clear(Feature::Commands);
        flags.Flip(Feature::Events);
        flags.Assign(
            true,
            Feature::Commands
        );
        flags.Assign(
            false,
            Feature::Clock
        );
        flags.AssignMask(
            0x0CU,
            0x08U
        );

        return flags;
    }


    /// Compile-time mutation result used to validate constexpr operation semantics.
    constexpr auto MutatedFlags = BuildCompileTimeMutation();

    static_assert(
        MutatedFlags.Value() == 0x0AU,
        "Mutation operations must produce the expected compact representation"
    );

    static_assert(
        MutatedFlags.HasAny(
            Feature::Clock,
            Feature::Commands
        ),
        "HasAny must detect at least one set flag"
    );

    static_assert(
        MutatedFlags.HasAll(Feature::Commands),
        "HasAll must accept a fully satisfied representable set"
    );

    static_assert(
        !MutatedFlags.HasAll(
            Feature::Commands,
            Feature::OutOfRange
        ),
        "HasAll must reject a request containing an unrepresentable flag"
    );

    static_assert(
        !MutatedFlags.IsSet(Feature::OutOfRange),
        "An unrepresentable flag must never report as set"
    );


    /// Executes runtime validation of boundary-bit mutation and defensive range handling.
    int RunFlagSetTests() noexcept {
        Flags8 flags;

        flags.Set(Feature::Highest);
        if (flags.Value() != 0x80U) return 1;
        if (!flags.IsSet(Feature::Highest)) return 2;

        flags.Set(Feature::OutOfRange);
        if (flags.Value() != 0x80U) return 3;

        flags.Flip(Feature::Highest);
        if (!flags.IsEmpty()) return 4;

        flags.Assign(
            true,
            Feature::Clock,
            Feature::Commands,
            Feature::Events
        );
        if (!flags.HasAll(
            Feature::Clock,
            Feature::Commands,
            Feature::Events
        )) return 5;

        flags.Clear(
            Feature::Clock,
            Feature::Events
        );
        if (flags.Value() != 0x02U) return 6;

        flags.AssignMask(
            0x03U,
            0x01U
        );
        if (flags.Value() != 0x01U) return 7;

        flags.Clear();
        if (!flags.IsEmpty()) return 8;

        return 0;
    }

} // ESPressio::System::Tests::Flags


/// Executes the host-side FlagSet tests.
int main() {
    return ESPressio::System::Tests::Flags::RunFlagSetTests();
}
