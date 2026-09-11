#pragma once

#include <brigadier/argument/parse/parser.hpp>
#include <brigadier/string_reader.hpp>

#include <string_view>

namespace brigadier {
	// separate type to avoid ambiguity with word_parser
	struct greedy_text { };

	template <>
	struct parser<greedy_text> {
		[[nodiscard]] static constexpr std::expected<std::string_view, parse_error> parse(string_reader& reader) noexcept {
			const auto value = reader.remaining();
			reader.set_cursor(reader.cursor() + value.size());
			return value;
		}
	};
}
