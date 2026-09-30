#include <array>
#include <cstdint>
#include <type_traits>

#include "ESPressio_System.hpp"

namespace ESPressio::System::Tests::Types {

    /// Valid semantic Type used to exercise universal Type identification.
    struct ExampleType final {

        /// Stable universal Type identity declared directly by the semantic Type.
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

    };


    /// Semantic Type carrying an explicitly invalid universal identifier.
    struct InvalidExampleType final {

        /// Reserved all-zero identifier used to verify the predicate rejects invalid identities.
        static constexpr TypeIdentifier Identifier{};

    };


    /// Type deliberately omitting Identifier to verify predicate-style rejection is substitution-safe.
    struct MissingIdentifierType final {};


    /// Type deliberately declaring Identifier in the wrong semantic domain.
    struct WrongIdentifierType final {

        /// Invalid Identifier declaration used to verify exact-Type predicate rejection.
        static constexpr std::uint8_t Identifier = 1U;

    };


    static_assert(
        sizeof(TypeAuthorityIdentifier) == 3U,
        "TypeAuthorityIdentifier must occupy exactly three bytes"
    );

    static_assert(
        sizeof(TypeLocalIdentifier) == 5U,
        "TypeLocalIdentifier must occupy exactly five bytes"
    );

    static_assert(
        sizeof(TypeIdentifier) == 8U,
        "TypeIdentifier must occupy exactly eight bytes"
    );

    static_assert(
        IdentifiedType<ExampleType>,
        "ExampleType must satisfy the universal identification contract"
    );

    static_assert(
        !IdentifiedType<InvalidExampleType>,
        "InvalidExampleType must not satisfy the universal identification contract"
    );

    static_assert(
        !IdentifiedType<MissingIdentifierType>,
        "Types without Identifier must evaluate IdentifiedType to false"
    );

    static_assert(
        !IdentifiedType<WrongIdentifierType>,
        "Types with a non-TypeIdentifier Identifier must evaluate IdentifiedType to false"
    );

    static_assert(
        !SchemaType<MissingIdentifierType>,
        "Types without Identifier must evaluate SchemaType to false without a hard error"
    );

    static_assert(
        !SchemaType<WrongIdentifierType>,
        "Types with the wrong Identifier domain must evaluate SchemaType to false"
    );

    static_assert(
        TypeIdentifierOf<ExampleType> == ExampleType::Identifier,
        "TypeIdentifierOf must return the semantic Type's direct identifier"
    );


    /// Executes runtime and constexpr-compatible validation of canonical Type identity semantics.
    int RunTypeIdentifierTests() noexcept {
        const TypeAuthorityIdentifier invalidAuthority;
        if (invalidAuthority.IsValid()) return 1;

        const TypeLocalIdentifier invalidLocalIdentifier;
        if (invalidLocalIdentifier.IsValid()) return 2;

        const TypeIdentifier invalidIdentifier;
        if (invalidIdentifier.IsValid()) return 3;

        const TypeAuthorityIdentifier authority(
            TypeAuthorityIdentifier::Storage{
                0x01U,
                0x23U,
                0x45U
            }
        );

        const TypeLocalIdentifier localIdentifier(
            TypeLocalIdentifier::Storage{
                0x67U,
                0x89U,
                0xABU,
                0xCDU,
                0xEFU
            }
        );

        if (!authority.IsValid()) return 4;
        if (authority.Value() != 0x012345U) return 5;
        if (!localIdentifier.IsValid()) return 6;
        if (localIdentifier.Value() != 0x6789ABCDEFULL) return 7;

        const TypeIdentifier componentIdentifier(
            authority,
            localIdentifier
        );

        const TypeIdentifier::Storage expectedBytes{
            0x01U,
            0x23U,
            0x45U,
            0x67U,
            0x89U,
            0xABU,
            0xCDU,
            0xEFU
        };

        const TypeIdentifier storageIdentifier(expectedBytes);

        if (!componentIdentifier.IsValid()) return 8;
        if (componentIdentifier != storageIdentifier) return 9;
        if (componentIdentifier.Bytes() != expectedBytes) return 10;
        if (componentIdentifier.Authority() != authority) return 11;
        if (componentIdentifier.LocalIdentifier() != localIdentifier) return 12;
        if (componentIdentifier.IsZero()) return 13;

        const TypeIdentifier invalidAuthorityIdentifier(
            TypeAuthorityIdentifier{},
            localIdentifier
        );
        if (invalidAuthorityIdentifier.IsValid()) return 14;

        const TypeIdentifier invalidLocalIdentifierValue(
            authority,
            TypeLocalIdentifier{}
        );
        if (invalidLocalIdentifierValue.IsValid()) return 15;

        const TypeIdentifier laterIdentifier(
            TypeIdentifier::Storage{
                0x01U,
                0x23U,
                0x45U,
                0x67U,
                0x89U,
                0xABU,
                0xCDU,
                0xF0U
            }
        );

        if (!(componentIdentifier < laterIdentifier)) return 16;
        if (laterIdentifier < componentIdentifier) return 17;

        return 0;
    }

} // ESPressio::System::Tests::Types


/// Executes the host-side universal Type identity tests.
int main() {
    return ESPressio::System::Tests::Types::RunTypeIdentifierTests();
}
