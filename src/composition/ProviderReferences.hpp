#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>

#include "Composition.hpp"

namespace ESPressio::System::CompositionFramework {

    /// Declares non-owning runtime bindings for the provider types represented by one compile-time ProviderList.
    ///
    /// @tparam TProviderList ProviderList whose provider instances will be referenced.
    template<class TProviderList>
    class ProviderReferences;


    /// Binds Bootstrap-owned provider instances to one compile-time ProviderList without taking ownership.
    ///
    /// The provider references are stored in exactly the order represented by the ProviderList. The collection
    /// performs no provider construction, heap allocation, runtime capability matching, or type erasure.
    ///
    /// @tparam TProviders Provider types represented by the supplied ProviderList.
    template<class... TProviders>
    class ProviderReferences<ProviderList<TProviders...>> final {

        private:

            // Bound provider references.

            /// Non-owning pointers to the Bootstrap-owned provider instances represented by this collection.
            std::tuple<TProviders*...> _providers;


        public:

            // Provider-reference metadata.

            /// Compile-time provider list represented by this collection.
            using ProviderTypes = ProviderList<TProviders...>;

            /// Number of provider instances referenced by this collection.
            static constexpr std::size_t Count = sizeof...(TProviders);


            // Construction.

            /// Binds the provider instances corresponding exactly to the represented ProviderList.
            ///
            /// The caller retains ownership of every supplied provider instance. Every instance must therefore
            /// outlive this ProviderReferences object and every copy made from it.
            ///
            /// @param providers Provider instances in the same order as their types appear in ProviderTypes.
            constexpr explicit ProviderReferences(
                TProviders&... providers
            ) noexcept :
                _providers(
                    &providers...
                ) {}


            // Provider traversal.

            /// Invokes one callable once for every mutable provider instance represented by this collection.
            ///
            /// Traversal follows ProviderTypes order. The callable must accept every represented provider type.
            /// Return values from the callable are deliberately ignored; result aggregation remains the caller's
            /// responsibility because provider operations may have domain-specific result semantics.
            ///
            /// @tparam TCallable Callable type invoked for every provider reference.
            /// @param callable Callable to invoke for every provider.
            template<class TCallable>
            constexpr void ForEach(TCallable&& callable) {
                constexpr bool isCompatible =
                    (std::is_invocable_v<TCallable&, TProviders&> && ...);

                static_assert(
                    isCompatible,
                    "ProviderReferences::ForEach callable must accept every represented provider type"
                );

                if constexpr (isCompatible) {
                    std::apply(
                        [&callable](TProviders*... providers) {
                            (
                                static_cast<void>(
                                    callable(*providers)
                                ),
                                ...
                            );
                        },
                        _providers
                    );
                }
            }

            /// Invokes one callable once for every provider instance through a const view.
            ///
            /// Traversal follows ProviderTypes order. Each provider is exposed as const, so the callable must
            /// accept every represented provider type through a const-compatible reference.
            ///
            /// @tparam TCallable Callable type invoked for every const provider reference.
            /// @param callable Callable to invoke for every provider.
            template<class TCallable>
            constexpr void ForEach(TCallable&& callable) const {
                constexpr bool isCompatible =
                    (std::is_invocable_v<TCallable&, const TProviders&> && ...);

                static_assert(
                    isCompatible,
                    "ProviderReferences::ForEach const callable must accept every represented provider type as const"
                );

                if constexpr (isCompatible) {
                    std::apply(
                        [&callable](TProviders*... providers) {
                            (
                                static_cast<void>(
                                    callable(
                                        static_cast<const TProviders&>(*providers)
                                    )
                                ),
                                ...
                            );
                        },
                        _providers
                    );
                }
            }

    };

} // ESPressio::System::CompositionFramework
