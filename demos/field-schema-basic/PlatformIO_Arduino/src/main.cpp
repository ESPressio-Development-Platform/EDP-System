#include <Arduino.h>

#include <cstddef>
#include <cstdint>

#include <ESPressio_System.hpp>

namespace Demo {

    /// Result of executing the universal Field schema demonstration.
    enum class DemoResult : std::uint8_t {
        Succeeded = 0,
        SchemaInvalid = 1,
        FieldCountMismatch = 2,
        IdentifierMismatch = 3,
        MemberBindingMismatch = 4
    };


    /// Demonstration schema-bearing semantic Type.
    struct TemperatureReading final {

        /// Stable universal Type identity of the demonstration schema.
        static constexpr ESPressio::System::TypeIdentifier Identifier{
            ESPressio::System::TypeIdentifier::Storage{
                0x01U,
                0x23U,
                0x45U,
                0x67U,
                0x89U,
                0xABU,
                0xCDU,
                0xEFU
            }
        };


        // Demonstration Fields.

        /// Temperature reading represented by Field zero.
        float Temperature = 0.0F;

        /// Capture sequence represented by Field one.
        std::uint32_t Sequence = 0U;


        /// Canonical compile-time Field schema.
        using Fields = ESPressio::System::FieldSet<
            ESPressio::System::FieldBinding<
                &TemperatureReading::Temperature,
                0U
            >,
            ESPressio::System::FieldBinding<
                &TemperatureReading::Sequence,
                1U
            >
        >;

    };


    /// Binding for the Temperature member.
    using TemperatureField = ESPressio::System::FieldBinding<
        &TemperatureReading::Temperature,
        0U
    >;


    static_assert(
        ESPressio::System::SchemaType<TemperatureReading>,
        "TemperatureReading must satisfy the universal schema contract"
    );


    /// Runs the universal Field schema demonstration.
    DemoResult Run() noexcept {
        if (!ESPressio::System::SchemaType<TemperatureReading>) return DemoResult::SchemaInvalid;
        if (ESPressio::System::FieldsOf<TemperatureReading>::Count != 2U) return DemoResult::FieldCountMismatch;
        if (ESPressio::System::FieldIdentifierOf<TemperatureField>.Value() != 0U) return DemoResult::IdentifierMismatch;

        TemperatureReading reading;
        reading.*TemperatureField::Member = 23.5F;

        if (reading.Temperature != 23.5F) return DemoResult::MemberBindingMismatch;

        std::size_t visited = 0U;
        std::uint16_t identifierSum = 0U;

        ESPressio::System::ForEachField<TemperatureReading>(
            [&visited, &identifierSum]<class TField>() {
                ++visited;
                identifierSum += TField::Identifier.Value();
            }
        );

        if (visited != 2U) return DemoResult::FieldCountMismatch;
        if (identifierSum != 1U) return DemoResult::IdentifierMismatch;

        return DemoResult::Succeeded;
    }

} // Demo

/// Runs the demonstration once during Arduino initialization.
void setup() {
    static_cast<void>(Demo::Run());
}

/// Leaves the demonstration idle after the one-time run.
void loop() {}
