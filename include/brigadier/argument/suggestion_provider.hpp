#pragma once

#include <brigadier/argument/suggestions_builder.hpp>

namespace brigadier {
	template <typename Argument>
	struct suggestion_provider {
		template <typename Source, typename... Args>
		static constexpr void suggest(const Source&, suggestions_builder&, const Args&...) { }
	};
}
