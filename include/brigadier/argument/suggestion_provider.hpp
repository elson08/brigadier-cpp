#pragma once

#include <brigadier/argument/suggestions_builder.hpp>

namespace brigadier {
	template <typename Argument>
	struct suggestion_provider {
		template <typename Source, typename... Args>
		static constexpr void suggest(const Source&, suggestions_builder&, const Args&...) { }
	};

	template <typename Argument>
	concept multi_word_suggestions = requires { requires suggestion_provider<Argument>::multi_word; };
}
