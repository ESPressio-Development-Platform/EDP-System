#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "ESPressio_System.hpp"

namespace ESPressio::System::Tests::Types {

    /// Schema-bearing semantic Type used to exercise universal Field identity and binding.
    struct TemperatureReading final {

        /// Stable universal identity of the demonstration schema Type.
        static constexpr TypeIdentifier Identifier{
            TypeIdentifier::Storage{
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


        // Demonstration application Fields.

        /// Temperature value represented by Field zero.
        float Temperature = 0.0F;

        /// Capture sequence represented by the highest possible one-byte Field identity.
        std::uint32_t Sequence = 0U;


        /// Canonical compile-time Field schema for TemperatureReading.
        using Fields = FieldSet<
            FieldBinding<
                &TemperatureReading::Temperature,
                0U
            >,
            FieldBinding<
                &TemperatureReading::Sequence,
                255U
            >
        >;

    };


    /// Identified schema Type deliberately containing no Fields.
    struct EmptySchema final {

        /// Stable universal identity of the empty demonstration schema Type.
        static constexpr TypeIdentifier Identifier{
            TypeIdentifier::Storage{
                0x01U,
                0x23U,
                0x45U,
                0x00U,
                0x00U,
                0x00U,
                0x00U,
                0x01U
            }
        };

        /// Explicit empty canonical Field schema.
        using Fields = FieldSet<>;

    };


    /// First demonstration Field binding.
    using TemperatureField = FieldBinding<
        &TemperatureReading::Temperature,
        0U
    >;

    /// Second demonstration Field binding.
    using SequenceField = FieldBinding<
        &TemperatureReading::Sequence,
        255U
    >;


    static_assert(
        sizeof(FieldIdentifier) == 1U,
        "FieldIdentifier must occupy exactly one byte"
    );

    static_assert(
        !std::is_default_constructible_v<FieldIdentifier>,
        "FieldIdentifier must require an explicit numeric identity"
    );

    static_assert(
        !std::is_convertible_v<std::uint8_t, FieldIdentifier>,
        "FieldIdentifier must not permit implicit integer conversion"
    );

    static_assert(
        FieldIdentifier{0U}.Value() == 0U,
        "Field zero must be a valid explicit identity"
    );

    static_assert(
        FieldIdentifier{255U}.Value() == 255U,
        "Field 255 must be a valid explicit identity"
    );

    static_assert(
        SchemaType<TemperatureReading>,
        "TemperatureReading must satisfy the universal schema contract"
    );

    static_assert(
        SchemaType<EmptySchema>,
        "Explicit zero-Field Types must satisfy the universal schema contract"
    );

    static_assert(
        FieldsOf<TemperatureReading>::Count == 2U,
        "FieldsOf must expose the canonical FieldSet"
    );

    static_assert(
        FieldIdentifierOf<TemperatureField> == FieldIdentifier{0U},
        "FieldIdentifierOf must expose the exact strong Field identity"
    );

    static_assert(
        FieldIdentifierOf<SequenceField> == FieldIdentifier{255U},
        "FieldIdentifierOf must preserve the complete one-byte identity range"
    );

    static_assert(
        std::is_same_v<
            FieldOwnerOf<TemperatureField>,
            TemperatureReading
        >,
        "FieldOwnerOf must expose the exact owning semantic Type"
    );

    static_assert(
        std::is_same_v<
            FieldValueOf<TemperatureField>,
            float
        >,
        "FieldValueOf must expose the represented member value Type"
    );


    /// Executes runtime validation of concrete member access and generic Field enumeration.
    int RunFieldSchemaTests() noexcept {
        TemperatureReading reading;

        reading.*TemperatureField::Member = 21.5F;
        reading.*SequenceField::Member = 42U;

        if (reading.Temperature != 21.5F) return 1;
        if (reading.Sequence != 42U) return 2;

        std::size_t visited = 0U;
        std::uint16_t identifierSum = 0U;

        ForEachField<TemperatureReading>(
            [&visited, &identifierSum]<class TField>() {
                ++visited;
                identifierSum += TField::Identifier.Value();
            }
        );

        if (visited != 2U) return 3;
        if (identifierSum != 255U) return 4;

        return 0;
    }

} // ESPressio::System::Tests::Types


/// Executes the host-side universal Field schema tests.
int main() {
    return ESPressio::System::Tests::Types::RunFieldSchemaTests();
}
