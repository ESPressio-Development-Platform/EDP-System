#pragma once

namespace ESPressio::System::CompositionFramework {

    /// Selects every provider satisfying one Requirement or provider specification.
    struct SelectAll final {};


    /// Selects exactly one satisfying provider and rejects zero or ambiguous matches.
    struct SelectUnique final {};


    /// Selects zero or one satisfying provider, returning void for zero and rejecting ambiguity.
    struct SelectOptionalUnique final {};


    /// Selects the unique satisfying provider advertising the minimum value for one Property.
    ///
    /// @tparam TProperty Property minimized across satisfying providers.
    template<class TProperty>
    struct SelectMinimum final {

        /// Property used to rank satisfying providers.
        using PropertyType = TProperty;

    };


    /// Selects the unique satisfying provider advertising the maximum value for one Property.
    ///
    /// @tparam TProperty Property maximized across satisfying providers.
    template<class TProperty>
    struct SelectMaximum final {

        /// Property used to rank satisfying providers.
        using PropertyType = TProperty;

    };


    /// Selects every satisfying provider tied for the minimum value of one Property.
    ///
    /// @tparam TProperty Property minimized across satisfying providers.
    template<class TProperty>
    struct SelectAllMinimum final {

        /// Property used to rank satisfying providers.
        using PropertyType = TProperty;

    };


    /// Selects every satisfying provider tied for the maximum value of one Property.
    ///
    /// @tparam TProperty Property maximized across satisfying providers.
    template<class TProperty>
    struct SelectAllMaximum final {

        /// Property used to rank satisfying providers.
        using PropertyType = TProperty;

    };

} // ESPressio::System::CompositionFramework
