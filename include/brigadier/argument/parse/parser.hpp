#pragma once

#include <brigadier/string_reader.hpp>
#include <expected>
#include <string>

namespace brigadier {
	struct parse_error {
		size_t cursor_{};
		std::string message_{};
	};

	template <typename Type>
	struct parser {
		using parse_result = std::expected<Type, parse_error>;
	};

	template <typename T, typename P>
	concept is_parser = requires (string_reader& reader) {
		{ T::parse(reader) } -> std::same_as<std::expected<P, parse_error>>;
	};
}
